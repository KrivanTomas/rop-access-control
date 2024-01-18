//  ██╗  ██╗██╗   ██╗██████╗  ██████╗     ██╗███████╗████████╗ ██████╗ ██████╗ ███████╗
//  ██║ ██╔╝██║   ██║██╔══██╗██╔═══██╗   ██╔╝██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗██╔════╝
//  █████╔╝ ██║   ██║██████╔╝██║   ██║  ██╔╝ ███████╗   ██║   ██║   ██║██████╔╝█████╗  
//  ██╔═██╗ ██║   ██║██╔══██╗██║   ██║ ██╔╝  ╚════██║   ██║   ██║   ██║██╔══██╗██╔══╝  
//  ██║  ██╗╚██████╔╝██║  ██║╚██████╔╝██╔╝   ███████║   ██║   ╚██████╔╝██║  ██║███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚══════╝
//  Made by Tomáš Křivan

#ifndef KuroSTORE_h
#define KuroSTORE_h
#include <Arduino.h>
#include <HardwareSerial.h>
#include <FS.h>
#include <SD.h>

class KuroSTORE
{
  public:
    KuroSTORE();
    void begin(uint8_t sdcard_cs);
    void add_user(uint16_t id, uint8_t privilage, uint8_t* rfid, char* name);
    bool get_user_by_id(uint16_t id, uint8_t* privilage, uint8_t** rfid, char** name);
    bool get_user_by_rfid(uint8_t* rfid, uint16_t* id, uint8_t* privilage, char** name);
  private:
    uint8_t _sdcard_cs;
    bool sd_connected;
    bool connect();
};

#endif