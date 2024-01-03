#ifndef Kuro_h
#define Kuro_h
#include "Arduino.h"
#include <HardwareSerial.h>

#ifndef RFID_TOKEN_LENGTH
#define RFID_TOKEN_LENGTH 12
#endif

class Kuro
{
  public:
    Kuro();
    bool read_token_from_uart(HardwareSerial *use_serial, uint8_t *token, int max_attempts = 3, int timeout = 50);
    bool void_frag(HardwareSerial *use_serial, bool *do_void, unsigned long *last_read, unsigned long void_duration);
  private:
    int hex_to_int(uint8_t a);
    int hex_pair_to_int(uint8_t a, uint8_t b);
    bool token_checksum(uint8_t *token);
};

#endif