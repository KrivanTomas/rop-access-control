// rop-access-control.ino
// Made by: Tomáš Křivan
//
// some sources:
// ESP32 Dev Module: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
// RTClib: https://www.elecrow.com/wiki/index.php?title=File:RTC.zip


#define SERIAL_BAUD 115200
#define RFID_BAUD 9600


#define BTN_OK 0
#define BTN_UP 2
#define BTN_DOWN 4
#define BUZZER_PIN 12
#define RELAY_PIN 15

#define RFID_UART_PORT 2
#define SDCARD_CS 5

//rfid
#define RFID_TOKEN_LENGTH 12
#define DISCARD_DURATION 500

//common libraries
//#include "SPI.h"
#include <Wire.h>

//LCD (i2c)
#include <LiquidCrystal_I2C.h>

//RFID (UART)
#include <HardwareSerial.h>

//Micro SD card (spi)
//#include "FS.h"
#include "SD.h"

//RTC (i2c)
#include "RTClib.h"

//Custom libraries
#include "RFIDROP.h"
#include "GUIROP.h"
#include "DataStoreROP.h"
#include "UtilROP.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);           // I2C address 0x27, 16 column and 2 rows
HardwareSerial rfid_serial(RFID_UART_PORT);   // Serial port 2
RTC_DS1307 rtc;
GUIROP gui;
DataStoreROP dataStore;

unsigned long rfid_last_read;
bool rfid_void_frag;

uint8_t ui_status = HOME_SCREEN;
unsigned long ui_timer;
uint8_t ui_value1;
bool ui_request = false;

#define MAX_ACTIVE_USERS 30
uint16_t active_users[MAX_ACTIVE_USERS];
uint16_t active_users_time[MAX_ACTIVE_USERS];
unsigned long second_timer;
bool any_user_active = false;

void setup() {
  Serial.begin(SERIAL_BAUD);  // debug console
  rfid_serial.begin(RFID_BAUD, SERIAL_8N1);
  lcd.init();

  Wire.begin();
  rtc.begin();
  dataStore.begin(SDCARD_CS, &rtc);
  gui.begin(&lcd, &rtc, &dataStore, BUZZER_PIN);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  rfid_last_read = millis();
  second_timer = millis();
  rfid_void_frag = true;
}

bool ok_pressed = false;
bool up_pressed = false;
bool down_pressed = false;

void loop() {
 
  // if(!dataStore.check_connection()){ // TODO dont do this
  //   gui.show_toast("NO SD");
  // }

  uint8_t token[RFID_TOKEN_LENGTH];
  if (RFIDROP::discard_frag(&rfid_serial, &rfid_void_frag, &rfid_last_read, DISCARD_DURATION) && RFIDROP::read_token_from_uart(&rfid_serial, token)) {
    uint16_t id;
    char* name;
    uint8_t privilage;


    uint8_t authority_requirement;
    if(gui.require_authorization(&authority_requirement)){
      Serial.printf("Authority of at least 0x%X needed\n", authority_requirement);
      if(authority_requirement == AUTH_ANY){
        Serial.println("Token read and passed");
        gui.play_sound(SOUND_COMPLETE);
        gui.comply_with_authorization(token);
      }
      else if(dataStore.get_user_by_rfid(token, &id, &privilage, &name)){
        if(privilage >= authority_requirement){
          Serial.println("Access granted");
          gui.play_sound(SOUND_COMPLETE);
          gui.comply_with_authorization(token);
        }
        else {
          Serial.printf("Authority of 0x%X is not high enough\n", privilage);
          gui.play_sound(SOUND_ERROR);
          gui.cancel_authorization();
        }
        delete[] name;
      }
      else {
        Serial.print("Unknown user, no authority");
        gui.play_sound(SOUND_ERROR);
        gui.cancel_authorization();
      }
    }
    else if(dataStore.get_user_by_rfid(token, &id, &privilage, &name)){
      Serial.printf("id:%d name:%s\n", id, name);

      char* lcd_name = UtilROP::de_accent_utf8(name, 25);
      
      uint8_t custom_data[20];
      for(int i = 0; i < 20; i++){
        custom_data[i] = 0;
      }

      bool user_active = false;
      for(uint16_t active_id : active_users){
        if(active_id == id){
          user_active = true;
          break;
        }
      }
      if(user_active) {
        gui.show_toast("Leaving...",500);
        for(int i = 0; i < MAX_ACTIVE_USERS; i++){
          if(active_users[i] == id){
            active_users[i] = 0;
            custom_data[0] = active_users_time[i] << 8 & 0xFF;
            custom_data[1] = active_users_time[i] << 0 & 0xFF;
            char buffer[20];
            
            Serial.println("use time");
            Serial.println(active_users_time[i]); // test
          }
        }  
        
        dataStore.record_event(id, EVENT_LEAVE, custom_data);
        tone(BUZZER_PIN, 200, 50);
      }else {
        gui.show_toast(lcd_name);
        for(int i = 0; i < MAX_ACTIVE_USERS; i++){
          if(active_users[i] == 0){
           active_users[i] = id;
           active_users_time[i] = 0;
           break;
          }
        }
        //dataStore.record_event(id, EVENT_JOIN, custom_data);
      }

      gui.set_operation(check_for_users());
      
      delete[] name;
      delete[] lcd_name;
      tone(BUZZER_PIN, 1000, 50);
    }
    else {
      gui.show_toast("User not found");
      Serial.printf("unknown user rifd: %s\n", token);
      //uint16_t new_id;
      //dataStore.add_user(&new_id, 0, token, "John Doe");
      gui.play_sound(SOUND_ERROR);
    }
    rfid_last_read = millis();
    rfid_void_frag = true;
  }

  send_input_to_gui();
  gui.update();

  uint8_t requests;
  uint8_t rfid[12];
  char name[25];
  if(gui.query_requests(&requests, rfid, name)){
    Serial.printf("Fulfilling requests %X\n", requests);
    if(requests & REQUEST_NEW_USER){
      uint16_t new_id;
      dataStore.add_user(&new_id, AUTH_USER, rfid, name);
    }
  }

  if(millis() - second_timer >= 1000){
    tick_users();
    second_timer = millis();
  }
}

void send_input_to_gui(){
  if(!ok_pressed && digitalRead(BTN_OK) == LOW){
    ok_pressed = true;
    gui.handle_input(INPUT_OK, BEFORE_INPUT);
  }
  else if (digitalRead(BTN_OK) == HIGH && ok_pressed){
    gui.handle_input(INPUT_OK, AFTER_INPUT);
    ok_pressed = false;
  }
  if(!up_pressed && digitalRead(BTN_UP) == LOW){
    up_pressed = true;
    gui.handle_input(INPUT_UP, BEFORE_INPUT);
  }
  else if (digitalRead(BTN_UP) == HIGH && up_pressed){
    gui.handle_input(INPUT_UP, AFTER_INPUT);
    up_pressed = false;
  }
  if(!down_pressed && digitalRead(BTN_DOWN) == LOW){
    down_pressed = true;
    gui.handle_input(INPUT_DOWN, BEFORE_INPUT);
  }
  else if (digitalRead(BTN_DOWN) == HIGH && down_pressed){
    gui.handle_input(INPUT_DOWN, AFTER_INPUT);
    down_pressed = false;
  }
}

void tick_users(){
  for(uint8_t i = 0; i < MAX_ACTIVE_USERS; i++){
    if(active_users[i] != 0){
      if(active_users_time[i] >= 7200){ //2 hours
        // unsign user
        uint8_t custom_data[20];
        for(int i = 0; i < 20; i++){
          custom_data[i] = 0;
        }
        custom_data[0] = active_users_time[i] << 8 & 0xFF;
        custom_data[1] = active_users_time[i] << 0 & 0xFF;
        dataStore.record_event(active_users[i], EVENT_LEAVE, custom_data);
        active_users[i] = 0;
        gui.set_operation(check_for_users());
      }
      active_users_time[i]++;
    }
  }
}

bool check_for_users(){
  any_user_active = false;
  for(uint16_t user : active_users){
    if(user != 0){
      any_user_active = true;
      digitalWrite(RELAY_PIN, HIGH);
      return true;
    }
  }
  digitalWrite(RELAY_PIN, LOW);
  return false;
}

