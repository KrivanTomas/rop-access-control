//  RFIDROP.h
//  Made by Tomáš Křivan   

#ifndef RFIDROP_h
#define RFIDROP_h
#include "Arduino.h"
#include <HardwareSerial.h>

#ifndef RFID_TOKEN_LENGTH
#define RFID_TOKEN_LENGTH 12
#endif

class RFIDROP
{
  public:
    static bool read_token_from_uart(HardwareSerial* use_serial, uint8_t* token, uint8_t max_attempts = 3, uint8_t timeout = 50);
    static bool discard_frag(HardwareSerial* use_serial, bool* do_void, unsigned long* last_read, unsigned long void_duration);
  private:
    static int hex_to_int(uint8_t a);
    static int hex_pair_to_int(uint8_t a, uint8_t b);
    static bool token_checksum(uint8_t* token);
};

#endif