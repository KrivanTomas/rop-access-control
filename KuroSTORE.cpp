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
//#include "FS.h"
//#include <FS.h>
#include <SD.h>
//#include "SPI.h"

#define BLOCK_SIZE 512

KuroSTORE::KuroSTORE(){
  sd_connected = false;
  return;
}

void KuroSTORE::begin(uint8_t sdcard_cs){
  _sdcard_cs = sdcard_cs;
  connect();
}

bool KuroSTORE::connect(){
  if(!SD.begin(_sdcard_cs)){
    Serial.println("No/Corrupted SD card attached");
    return false;
  }
  sd_connected = true;
  Serial.println("SD attached");
  //get_user_by_id();
  return true;
}

bool KuroSTORE::get_user_by_id(uint16_t id, uint8_t* privilage, uint8_t** rfid, char** name){
  File file_r = SD.open("/users.dat", FILE_READ);
  unsigned long file_size = file_r.size();
  bool user_found = false;
  for(unsigned long addr = file_r.position(); addr <= file_size; addr += 40){
    file_r.seek(addr);
    if(file_r.read() == (uint8_t)((id & 0xFF00) >> 8) && file_r.read() == (uint8_t)(id & 0x00FF)) { // == 0
      user_found = true;
      break;
    }
  }
  if(!user_found) {
    file_r.close();
    return false;
  }
  *privilage = file_r.read();
  *rfid = new uint8_t[12];
  for(int i = 0; i < 12; i++){
    (*rfid)[i] = file_r.read();
  }
  *name = new char[25];
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
void KuroSTORE::add_user(uint16_t id, uint8_t privilage, uint8_t* rfid, char* name){
  File file_r = SD.open("/users.dat", FILE_READ);

  unsigned long file_size = file_r.size();
  unsigned long empty_addr;
  bool outside_of = true;
  for(unsigned long addr = file_r.position(); addr <= file_size; addr+=40){
    empty_addr = addr;
    file_r.seek(addr);
    if(!file_r.read() && !file_r.read()) { // == 0
      outside_of = false;
      break;
    }
  }

  file_r.close();
  
  File file_w = SD.open("/users.dat", FILE_WRITE);
  
  if(outside_of){
    empty_addr += 40;
  }

  file_w.seek(empty_addr);

  file_w.write((uint8_t)((id & 0xFF00) >> 8));
  file_w.write((uint8_t)(id & 0x00FF));
  file_w.write(privilage);
  for(int i = 0; i < 12; i++){
    file_w.write(rfid[i]);
  }
  for(int i = 0; i < 25; i++){
    file_w.write(name[i]);
  }
  file_w.close();
}