//  UtilROP.h
//  Made by Tomáš Křivan

#ifndef UtilROP_h
#define UtilROP_h
#include "Arduino.h"

#define AUTH_NONE 0x00
#define AUTH_ANY 0x01

#define AUTH_USER 0x03
#define AUTH_READER 0x04
#define AUTH_EDITOR 0x05
#define AUTH_ADMINISTRATOR 0xFF

class UtilROP
{
  public:
    static char* de_accent_utf8(char* text, uint8_t length);
    static char* fill_space(char* text, uint8_t length);
};

#endif