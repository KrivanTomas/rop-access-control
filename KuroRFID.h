//  ██╗  ██╗██╗   ██╗██████╗  ██████╗     ██╗██████╗ ███████╗██╗██████╗ 
//  ██║ ██╔╝██║   ██║██╔══██╗██╔═══██╗   ██╔╝██╔══██╗██╔════╝██║██╔══██╗
//  █████╔╝ ██║   ██║██████╔╝██║   ██║  ██╔╝ ██████╔╝█████╗  ██║██║  ██║
//  ██╔═██╗ ██║   ██║██╔══██╗██║   ██║ ██╔╝  ██╔══██╗██╔══╝  ██║██║  ██║
//  ██║  ██╗╚██████╔╝██║  ██║╚██████╔╝██╔╝   ██║  ██║██║     ██║██████╔╝
//  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝    ╚═╝  ╚═╝╚═╝     ╚═╝╚═════╝ 
//  Made by Tomáš Křivan   

#ifndef KuroRFID_h
#define KuroRFID_h
#include "Arduino.h"
#include <HardwareSerial.h>

#ifndef RFID_TOKEN_LENGTH
#define RFID_TOKEN_LENGTH 12
#endif

class KuroRFID
{
  public:
    static bool read_token_from_uart(HardwareSerial* use_serial, uint8_t* token, uint8_t max_attempts = 3, uint8_t timeout = 50);
    static bool void_frag(HardwareSerial* use_serial, bool* do_void, unsigned long* last_read, unsigned long void_duration);
  private:
    static int hex_to_int(uint8_t a);
    static int hex_pair_to_int(uint8_t a, uint8_t b);
    static bool token_checksum(uint8_t* token);
};

#endif