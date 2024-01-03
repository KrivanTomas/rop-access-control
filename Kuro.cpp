/*
  Kuro.cpp - An API for a project I am making :)
*/


#include "Arduino.h"
#include <HardwareSerial.h>
#include "Kuro.h"

Kuro::Kuro() {
  return;
}

bool Kuro::read_token_from_uart(HardwareSerial *use_serial, uint8_t *token, int max_attempts, int timeout) {
  int store_index = -1;
  int attempt = 0;
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

bool Kuro::void_frag(HardwareSerial *use_serial, bool *do_void, unsigned long *last_read, unsigned long void_duration) {
  if(!*do_void) return true;
  if(millis() - *last_read >= void_duration) {
    *do_void = false;
    return true;
  } 
  if(use_serial->available()) {
    use_serial->read(); //void fragment
    *last_read = millis();
  }
  return false;
}

int Kuro::hex_to_int(uint8_t a) {
  if(a >= '0' && a <= '9'){
    return a - 48;
  }
  return a - 55;
}

int Kuro::hex_pair_to_int(uint8_t a, uint8_t b) {
  return (hex_to_int(a) << 4) + hex_to_int(b);
}

bool Kuro::token_checksum(uint8_t *token){
  int pair1 = hex_pair_to_int(token[0], token[1]);
  int pair2 = hex_pair_to_int(token[2], token[3]);
  int pair3 = hex_pair_to_int(token[4], token[5]);
  int pair4 = hex_pair_to_int(token[6], token[7]);
  int pair5 = hex_pair_to_int(token[8], token[9]);
  int check = hex_pair_to_int(token[10], token[11]);

  return (pair1 ^ pair2 ^ pair3 ^ pair4 ^ pair5) == check;
}
