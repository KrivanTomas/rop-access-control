//  ██╗  ██╗██╗   ██╗██████╗  ██████╗     ██╗██╗   ██╗████████╗██╗██╗     
//  ██║ ██╔╝██║   ██║██╔══██╗██╔═══██╗   ██╔╝██║   ██║╚══██╔══╝██║██║     
//  █████╔╝ ██║   ██║██████╔╝██║   ██║  ██╔╝ ██║   ██║   ██║   ██║██║     
//  ██╔═██╗ ██║   ██║██╔══██╗██║   ██║ ██╔╝  ██║   ██║   ██║   ██║██║     
//  ██║  ██╗╚██████╔╝██║  ██║╚██████╔╝██╔╝   ╚██████╔╝   ██║   ██║███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═════╝    ╚═╝   ╚═╝╚══════╝
//  Made by Tomáš Křivan

#ifndef KuroUTIL_h
#define KuroUTIL_h
#include "Arduino.h"

#define AUTH_NONE 0x00
#define AUTH_ANY 0x01
#define AUTH_D_CLASS 0x02
#define AUTH_USER 0x03
#define AUTH_READER 0x04
#define AUTH_EDITOR 0x05
#define AUTH_ADMINISTRATOR 0xFF

class KuroUTIL
{
  public:
    static char* de_accent_utf8(char* text, uint8_t length);
    static char* fill_space(char* text, uint8_t length);
};

#endif