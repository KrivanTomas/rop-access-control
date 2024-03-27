//  RFIDROP.cpp
//  Made by Tomáš Křivan

#include "Arduino.h"
#include <HardwareSerial.h>
#include "RFIDROP.h"

/**
 * Reads the token value from a rfid chip via UART.
 * 
 * This function blocks the thread for the duration of the UART transmission, the longest time it will block the thread is the duration of the `timeout` variable (in milliseconds).
 * This function may read a token multiple times if located in a loop, if this behavior is unwanted, use the `read_token_from_uart` function in conjuction with the `discard_frag` function.
 * 
 * @param use_serial A pointer to the `HardwareSerial` object from which to communicate via UART
 * @param token A pointer to the output array, which will contain the read token if valid and the function returns true
 * @param max_attempts A number of invalid reads before giving up and yielding to the thread
 * @param timeout Time in millisecond before giving up and yielding to the thread
 * @return If a token has been read successfuly (the token read will be placed int the array at the `token` pointer)
*/
bool RFIDROP::read_token_from_uart(HardwareSerial* use_serial, uint8_t* token, uint8_t max_attempts, uint8_t timeout) {
  int8_t store_index = -1;
  uint8_t attempt = 0;
  unsigned long start_time = millis();
  while (start_time + timeout >= millis()) {
    if(!use_serial->available()) continue;
    uint8_t token_fragment = use_serial->read();
    switch(token_fragment){
      case 2: // start;
        attempt++;
        start_time = millis();
        store_index = 0;
        break;

      case 3: // end
        if(store_index != RFID_TOKEN_LENGTH || !token_checksum(token)) { 
          if(attempt <= max_attempts) continue;
          return false;
        }
        return true;
        break;

      default: // store fragment
        if(store_index == -1) break;
        token[store_index++] = token_fragment;
        break;
    }
  }
  return false;
}

/**
 * Used in conjunction with the `read_token_from_uart` function to block reapeating reads when in a loop.
 * 
 * It reads and discards the incoming data, so that `use_serial->available()` becomes false and the `read_token_from_uart` 
 * function does not read old rfid tokens from the rfid reader chip register.
 * 
 * Example use:
 * ```
 * if (RFIDROP::discard_frag(&rfid_serial, &rfid_void_frag, &rfid_last_read, 500) && RFIDROP::read_token_from_uart(&rfid_serial, token)) { 
 *  -- SUCCESSFUL READ CODE HERE --
 *  rfid_last_read = millis();
 *  rfid_void_frag = true;
 *  }
 * ```
 * On the first read the `discard_frag` function returns `true` because the value of `do_discard` is false, so the program has to evaluate the second argument (because of the logical and `&&`)
 * and runs the `read_token_from_uart` function, which if read successfuly returns `true` and makes the whole argument in the if statement => `true`. From now on, the `do_discard` now value set to `true`,
 * will cause the function to return false (blocking the other fuction to be evaulued), util time of `discard_duration` will pass from time of `last_read`.
 * 
 * 
 * @param use_serial A pointer to the `HardwareSerial` object from which to communicate via UART
 * @param do_discard A pointer to a helper boolean variable that latches the discarding state
 * @param last_read A pointer to a millisecond time value of the last read (this is updated when UART is still available)
 * @param discard_duration Duration in millisecons from `last_read` when the discarding will be disabled
 * @returns A contol boolean to be used in an if statement with the `read_token_from_uart` function (see example)
*/
bool RFIDROP::discard_frag(HardwareSerial* use_serial, bool* do_discard, unsigned long* last_read, unsigned long discard_duration) {
  if(!*do_discard) return true;
  if(millis() - *last_read >= void_duration) {
    *do_discard = false;
    return true;
  } 
  if(use_serial->available()) {
    use_serial->read(); // discard fragment
    *last_read = millis();
  }
  return false;
}

/**
 * Converts a char in hexadecimal to an integer value.
 * 
 * @param hex The hex value to convert
 * @returns The value of the input in a decimal integer
*/
int RFIDROP::hex_to_int(uint8_t hex) {
  if(hex >= '0' && hex <= '9'){
    return hex - 48;
  }
  return hex - 55;
}

/**
 * Converts the two input characters (a 1 byte value encoded in two hexadecimal digits) into its integer value.
 * 
 * Example:
 * ```
 * hex_pair_to_int('F','F') => 255
 * ```
 * 
 * @param a The first character
 * @param b The second character
 * @returns The converted value
*/
int RFIDROP::hex_pair_to_int(uint8_t a, uint8_t b) {
  return (hex_to_int(a) << 4) + hex_to_int(b);
}

/**
 * Validates if the token checksum matches the calculated checksum.
 * 
 * @param token The token array
 * @returns If the token is valid
*/
bool RFIDROP::token_checksum(uint8_t *token){
  int pair1 = hex_pair_to_int(token[0], token[1]);
  int pair2 = hex_pair_to_int(token[2], token[3]);
  int pair3 = hex_pair_to_int(token[4], token[5]);
  int pair4 = hex_pair_to_int(token[6], token[7]);
  int pair5 = hex_pair_to_int(token[8], token[9]);
  int check = hex_pair_to_int(token[10], token[11]);

  return (pair1 ^ pair2 ^ pair3 ^ pair4 ^ pair5) == check;
}
