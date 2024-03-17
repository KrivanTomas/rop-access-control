//  DataStoreROP.h
//  Made by Tomáš Křivan

#ifndef DataStoreROP_h
#define DataStoreROP_h
#include <Arduino.h>
#include <HardwareSerial.h>
#include "UtilROP.h"
#include <FS.h>
#include <SD.h>
#include "RTClib.h"

#define EVENT_JOIN 0x01
#define EVENT_LEAVE 0x02


class DataStoreROP
{
  public:
    DataStoreROP();
    void begin(uint8_t sdcard_cs, RTC_DS1307* rtc);
    bool check_connection();
    void add_user(uint16_t* id, uint8_t privilage, uint8_t* rfid, char* name);
    void record_event(uint16_t user_id, uint8_t event_type, uint8_t custom_data[20]);
    bool get_user_event(uint16_t user_id, uint16_t offset, uint16_t* event_id, uint8_t* event_type, uint64_t* unix_time, char date_string[19], uint8_t custom_data[20]);
    bool get_user_by_id(uint16_t id, uint8_t* privilage, uint8_t** rfid, char** name);
    bool get_user_by_static_id(uint16_t id, uint8_t* privilage, uint8_t** rfid, char** name);
    bool get_user_by_rfid(uint8_t* rfid, uint16_t* id, uint8_t* privilage, char** name);
    bool verify_authority(uint16_t id, uint8_t authority_requirement);
  private:
    RTC_DS1307* _rtc;
    uint16_t last_event_address;
    uint8_t _sdcard_cs;
    bool sd_connected;
    bool connect();
};

#endif