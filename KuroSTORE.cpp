//  ██╗  ██╗██╗   ██╗██████╗  ██████╗     ██╗███████╗████████╗ ██████╗ ██████╗ ███████╗
//  ██║ ██╔╝██║   ██║██╔══██╗██╔═══██╗   ██╔╝██╔════╝╚══██╔══╝██╔═══██╗██╔══██╗██╔════╝
//  █████╔╝ ██║   ██║██████╔╝██║   ██║  ██╔╝ ███████╗   ██║   ██║   ██║██████╔╝█████╗  
//  ██╔═██╗ ██║   ██║██╔══██╗██║   ██║ ██╔╝  ╚════██║   ██║   ██║   ██║██╔══██╗██╔══╝  
//  ██║  ██╗╚██████╔╝██║  ██║╚██████╔╝██╔╝   ███████║   ██║   ╚██████╔╝██║  ██║███████╗
//  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═╝  ╚═╝╚══════╝
//  Made by Tomáš Křivan

//https://randomnerdtutorials.com/esp32-microsd-card-arduino/

#include <Arduino.h>
#include <HardwareSerial.h>
#include "KuroSTORE.h"
#include "KuroUTIL.h"
//#include "FS.h"
//#include <FS.h>
#include <SD.h>
#include "RTClib.h"
//#include "SPI.h"

#define BLOCK_SIZE 512

KuroSTORE::KuroSTORE(){
  sd_connected = false;
  return;
}

void KuroSTORE::begin(uint8_t sdcard_cs, RTC_DS1307* rtc){
  _rtc = rtc;
  _sdcard_cs = sdcard_cs;
  last_event_address = 0;
  sd_connected = connect();


  if(sd_connected){
    if(!SD.exists("/users.dat")){
      Serial.println("No user database found");
      Serial.println("Creating a new user database file ...");
      File user_f = SD.open("/users.dat", FILE_WRITE);
      user_f.write(0);
      user_f.write(0);
      user_f.close();
      if(!SD.exists("/users.dat")){
        Serial.println("Could not create a new file!");
      }
      else {
        Serial.println("Users database created.");
      }
    }
    if(!SD.exists("/events.dat")){
      Serial.println("No event database found");
      Serial.println("Creating a new event database file ...");
      File user_f = SD.open("/events.dat", FILE_WRITE);
      user_f.write(0);
      user_f.write(0);
      user_f.close();
      if(!SD.exists("/events.dat")){
        Serial.println("Could not create a new file!");
      }
      else {
        Serial.println("Event database created.");
      }
    }
    if(!SD.exists("/values.dat")){
      Serial.println("No value database found");
      Serial.println("Creating a new value database file ...");
      File user_f = SD.open("/values.dat", FILE_WRITE);
      user_f.write(0);
      user_f.write(0);
      user_f.close();
      if(!SD.exists("/values.dat")){
        Serial.println("Could not create a new file!");
      }
      else {
        Serial.println("Value database created.");
      }
    }
    else {
      File values_r = SD.open("/values.dat", FILE_READ);
      last_event_address = (uint16_t)values_r.read() << 8 | (uint16_t)values_r.read();
      values_r.close();
    }
  }
}

bool KuroSTORE::connect(){
  if(!SD.begin(_sdcard_cs)){
    Serial.println("No/Corrupted SD card attached");
    return false;
  }
  sd_connected = true;
  Serial.println("SD attached");
  return true;
}

bool KuroSTORE::check_connection(){
  sd_connected = SD.exists("/");
  return sd_connected;
}

bool KuroSTORE::get_user_by_id(uint16_t id, uint8_t* privilage, uint8_t** rfid, char** name){
  File file_r = SD.open("/users.dat", FILE_READ);
  unsigned long file_size = file_r.size();
  bool user_found = false;
  for(unsigned long addr = file_r.position(); addr <= file_size; addr += 40){
    file_r.seek(addr);
    if(file_r.read() == (uint8_t)((id & 0xFF00) >> 8) && file_r.read() == (uint8_t)(id & 0x00FF)) {
      user_found = true;
      break;
    }
  }
  if(!user_found) {
    file_r.close();
    return false;
  }
  *privilage = file_r.read();
  if(rfid != nullptr) delete[] rfid;
  *rfid = new uint8_t[12];
  for(int i = 0; i < 12; i++){
    (*rfid)[i] = file_r.read();
  }
  if(name != nullptr) delete[] name;
  *name = new char[25];
  for(int i = 0; i < 25; i++){
    (*name)[i] = file_r.read();
  }
  file_r.close();
  return true;
}

// not tested
bool KuroSTORE::get_user_by_static_id(uint16_t id, uint8_t* privilage, uint8_t** rfid, char** name){ 
  File file_r = SD.open("/users.dat", FILE_READ);
  unsigned long file_size = file_r.size();
  unsigned long start = file_r.position();
  if(start + 40 * (id - 1) > file_size){
    file_r.close();
    return false;
  }
  file_r.seek(start + 40 * (id - 1));
  if(file_r.read() != (uint8_t)((id & 0xFF00) >> 8) || file_r.read() != (uint8_t)(id & 0x00FF)) { // != id
    file_r.close();
    return false;
  }
  *privilage = file_r.read();
  for(int i = 0; i < 12; i++){
    (*rfid)[i] = file_r.read();
  }
  for(int i = 0; i < 25; i++){
    (*name)[i] = file_r.read();
  }
  file_r.close();
  return true;
}

bool KuroSTORE::get_user_by_rfid(uint8_t* rfid, uint16_t* id, uint8_t* privilage, char** name){
  File file_r = SD.open("/users.dat", FILE_READ);
  unsigned long file_size = file_r.size();
  unsigned long user_addr;
  bool token_match;
  bool user_found = false;
  for(unsigned long addr = file_r.position(); addr <= file_size; addr += 40){
    file_r.seek(addr + 3); // 2 id + 1 priv
    token_match = true;
    for(uint8_t token_index = 0; token_index < 12; token_index++){
      if(rfid[token_index] != file_r.read()){
        token_match = false;
        break;
      }
    }
    if(token_match) {
      user_found = true;
      user_addr = addr;
      break;
    }
  }
  if(!user_found) {
    file_r.close();
    return false;
  }
  file_r.seek(user_addr);
  *id = ((uint16_t)file_r.read()) >> 8;
  *id |= ((uint16_t)file_r.read());
  *privilage = file_r.read();
  file_r.seek(file_r.position() + 12);
  *name = new char[25];
  for(int i = 0; i < 25; i++){
    (*name)[i] = file_r.read();
  }
  file_r.close();
  return true;
}


// 2 bytes id | 1 byte privilage | 12 bytes rfid, 25 bytes name => 40 bytes/user raw, max 65 535 users (id 0x0000 for empty entry)
void KuroSTORE::add_user(uint16_t* id, uint8_t privilage, uint8_t* rfid, char* name){
  File file_r = SD.open("/users.dat", FILE_READ);

  unsigned long file_size = file_r.size();
  unsigned long empty_addr = file_r.position();
  bool outside_of = true;
  Serial.printf("Starting user write at 0x%X\n", file_r.position());
  *id = 1; // 0 for empty fields
  for(unsigned long addr = file_r.position(); addr + 39 <= file_size; addr += 40){
    empty_addr = addr;
    file_r.seek(addr);
    if(file_r.read() == 0 && file_r.read() == 0) { // id == 0
      outside_of = false;
      break;
    }
    (*id)++;
  }

  file_r.close();
  
  File file_w = SD.open("/users.dat", FILE_APPEND);
  
  if(outside_of){
    empty_addr += 40;
  }

  Serial.printf("Writing new user at adress 0x%X with id %d\n", empty_addr, *id);
  file_w.seek(empty_addr);

  file_w.write((uint8_t)(((*id) & 0xFF00) >> 8));
  file_w.write((uint8_t)((*id) & 0x00FF));
  file_w.write(privilage);
  for(int i = 0; i < 12; i++){
    file_w.write(rfid[i]);
  }
  for(int i = 0; i < 25; i++){
    file_w.write(name[i]);
  }
  file_w.close();
}

bool KuroSTORE::verify_authority(uint16_t id, uint8_t authority_requirement){
  char* name;
  uint8_t* rfid;
  uint8_t privilage;
  if(!get_user_by_static_id(id, &privilage, &rfid, &name)) return false;
  return privilage >= authority_requirement;
}
// 2 bytes event_id | 2 bytes user_id | 1 byte event_type | 8 bytes unix time | 19 bytes ISO 8601 date string YYYY-MM-DDTHH:mm:ss | 20 bytes custom data => 52 bytes
// rollover recording
void KuroSTORE::record_event(uint16_t user_id, uint8_t event_type, uint8_t custom_data[20]){
  File file_r = SD.open("/events.dat", FILE_READ);

  file_r.read();
  unsigned long base_address = file_r.position() - 1; // 2 bytes foward from begining for some reason
  file_r.close();

  unsigned long address = base_address + 52 * last_event_address;

  File file_w = SD.open("/events.dat", FILE_WRITE);
  
  Serial.printf("Writing new event at adress 0x%X\n", address);
  file_w.seek(address); // does not go back and appends anyway (╯°□°）╯︵ ┻━┻
  file_w.write(last_event_address << 8 & 0xFF);
  file_w.write(last_event_address << 0 & 0xFF);
  file_w.write(user_id << 8 & 0xFF);
  file_w.write(user_id << 0 & 0xFF);
  file_w.write(event_type);
  DateTime now = _rtc->now();
  uint64_t unix = now.unixtime();
  //Serial.println(unix);

  for(uint8_t i = 0; i < 8; i++){
    file_w.write(unix >> (8 * (7 - i)) & 0xFF);
  }

  char iso_buffer[30];
  sprintf(iso_buffer, "%04d-%02d-%02dT%02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

  for(int i = 0; i < 19; i++){
    file_w.write(iso_buffer[i]);
  }

  for(int i = 0; i < 20; i++){
    file_w.write(custom_data[i]);
  }
  Serial.printf("Should be the same x%X == x%X\n\n",  base_address, file_w.position() - 51);
  file_w.close();
  Serial.println("last event");
  Serial.println(last_event_address); // test
  if(last_event_address == UINT16_MAX){
    last_event_address = 0;
  }
  else {
    last_event_address++;
  }

  File values_w = SD.open("/values.dat", FILE_WRITE);
  values_w.write(last_event_address << 8 & 0xFF);
  values_w.write(last_event_address << 0 & 0xFF);


  values_w.close();
}

// 2 bytes event_id | 2 bytes user_id | 1 byte event_type | 8 bytes unix time | 19 bytes ISO 8601 date string YYYY-MM-DDTHH:mm:ss | 20 bytes custom data => 52 bytes
bool KuroSTORE::get_user_event(uint16_t user_id, uint16_t offset, uint16_t* event_id, uint8_t* event_type, uint64_t* unix_time, char date_string[19], uint8_t custom_data[20]){
  File file_r = SD.open("/events.dat", FILE_READ);


  uint16_t event_address = last_event_address;
  file_r.read();
  unsigned long file_size = file_r.size();
  unsigned long base_address = file_r.position() - 1; // 2 bytes foward from begining for some reason
  char *buffer = new char[52];

  Serial.printf("file_size %i\n", file_size);
  Serial.printf("base_address %i\n", base_address);
  bool down = true;
  // one read cycle
  while(true){
   
    unsigned long address = base_address + 52 * event_address;

    //Serial.printf("Reading event at x%X\n", address);

    file_r.seek(address);
    file_r.readBytes(buffer, 52);
    if(buffer[2] == (user_id << 8 & 0xFF) && buffer[3] == (user_id << 0 & 0xFF)){
      if(offset != 0) {
        offset--;
        continue;
      }
      *event_id = (uint16_t)buffer[0] << 8 | buffer[1];
      *event_type = buffer[4];
      *unix_time = 0;
      for(int i = 0; i < 8; i++){
        *unix_time |= (uint64_t)buffer[5 + (7 - i)] << 8 * i;
      }
      for(int i = 0; i < 19; i++){
        date_string[i] = buffer[13 + i];
      }
      for(int i = 0; i < 20; i++){
        custom_data[i] = buffer[32 + i];
      }

      delete[] buffer;
      file_r.close();
      return true;
    }

    if(event_address == 0){
      down = false;
      event_address = last_event_address;
    }
    if(down){
      event_address--;
    }
    else {
      event_address++;
      if (event_address - last_event_address > 2000) {
        return false;
      }
    }
  }

  // end
  delete[] buffer;
  file_r.close();
  return false;
}
