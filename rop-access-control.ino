//  ________ _________  _______   ________   ___  __    ________     
// |\   ____\\___   ___\\  ___ \ |\   ___  \|\  \|\  \ |\   __  \    
// \ \  \___\|___ \  \_\ \   __/|\ \  \\ \  \ \  \/  /|\ \  \|\  \   
//  \ \  \       \ \  \ \ \  \_|/_\ \  \\ \  \ \   ___  \ \  \\\  \  
//   \ \  \____   \ \  \ \ \  \_|\ \ \  \\ \  \ \  \\ \  \ \  \\\  \ 
//    \ \_______\  \ \__\ \ \_______\ \__\\ \__\ \__\\ \__\ \_______\
//     \|_______|   \|__|  \|_______|\|__| \|__|\|__| \|__|\|_______|                                            
//
// Made by: Tomáš Křivan
//
// some sources:
// ESP32 Dev Module: https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
// RTClib: https://www.elecrow.com/wiki/index.php?title=File:RTC.zip



// TODO setting
// date and time (timezones?)
// 


// SOON
// text edit
// new user screen
// usage database
// user display total time


#define SERIAL_BAUD 115200
#define RFID_BAUD 9600

#define BUZZER_PIN 12

//encoder
// #define ENC_CLK 4
// #define ENC_DT 0
// #define ENC_SW 2
#define BTN_OK 26
#define BTN_UP 25
#define BTN_DOWN 27

#define RFID_UART_PORT 2

#define SDCARD_CS 5

//rfid
#define RFID_TOKEN_LENGTH 12
#define VOID_DURATION 500

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
#include "KuroRFID.h"
#include "KuroGUI.h"
#include "KuroSTORE.h"
#include "KuroUTIL.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);           // I2C address 0x27, 16 column and 2 rows
HardwareSerial rfid_serial(RFID_UART_PORT);   // Serial port 2
RTC_DS1307 rtc;
KuroGUI gui;
KuroSTORE store;

unsigned long rfid_last_read;
bool rfid_void_frag;

uint8_t ui_status = HOME_SCREEN;
unsigned long ui_timer;
uint8_t ui_value1;
bool ui_request = false;

void setup() {
  Serial.begin(SERIAL_BAUD);  // debug console
  rfid_serial.begin(RFID_BAUD, SERIAL_8N1);
  lcd.init();

  Wire.begin();
  rtc.begin();
  gui.begin(&lcd, &rtc, BUZZER_PIN);
  store.begin(SDCARD_CS);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  rfid_last_read = millis();
  rfid_void_frag = true;
}

bool ok_pressed = false;
bool up_pressed = false;
bool down_pressed = false;

void loop() {
 
  // if(!store.check_connection()){ // TODO dont do this
  //   gui.show_toast("NO SD");
  // }

  uint8_t token[RFID_TOKEN_LENGTH];
  if (KuroRFID::void_frag(&rfid_serial, &rfid_void_frag, &rfid_last_read, VOID_DURATION) && KuroRFID::read_token_from_uart(&rfid_serial, token)) {
    uint16_t id;
    char* name;
    uint8_t privilage;


    uint8_t authority_requirement;
    if(gui.require_authorization(&authority_requirement)){
      Serial.printf("Authority of at least 0x%X needed\n", authority_requirement);
      if(authority_requirement == AUTH_ANY){
        Serial.println("Token read and passed");
        gui.play_sound(SOUND_WHISTLE);
        gui.comply_with_authorization(token);
      }
      else if(store.get_user_by_rfid(token, &id, &privilage, &name)){
        if(privilage >= authority_requirement){
          Serial.println("Access granted");
          gui.play_sound(SOUND_WHISTLE);
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
    else if(store.get_user_by_rfid(token, &id, &privilage, &name)){
      Serial.printf("id:%d name:%s\n", id, name);
    
      char* lcd_name = KuroUTIL::de_accent_utf8(name, 25);
      gui.show_toast(lcd_name);

      delete[] name;
      delete[] lcd_name;
      tone(BUZZER_PIN, 1000, 50);
    }
    else {
      gui.show_toast("User not found");
      //uint16_t new_id;
      //store.add_user(&new_id, 0, token, "John Doe");
      gui.play_sound(SOUND_ERROR);
    }
    rfid_last_read = millis();
    rfid_void_frag = true;
  }

  send_input_to_gui();
  gui.update();

  uint8_t requests;
  uint8_t rfid[12];
  if(gui.query_requests(&requests, rfid)){
    Serial.printf("Fulfilling requests %X\n", requests);
    if(requests & REQUEST_NEW_USER){
      uint16_t new_id;
      store.add_user(&new_id, AUTH_USER, rfid, "John Doe");
    }
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

