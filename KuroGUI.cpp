//  ██╗  ██╗██╗   ██╗██████╗  ██████╗     ██╗ ██████╗ ██╗   ██╗██╗
//  ██║ ██╔╝██║   ██║██╔══██╗██╔═══██╗   ██╔╝██╔════╝ ██║   ██║██║
//  █████╔╝ ██║   ██║██████╔╝██║   ██║  ██╔╝ ██║  ███╗██║   ██║██║
//  ██╔═██╗ ██║   ██║██╔══██╗██║   ██║ ██╔╝  ██║   ██║██║   ██║██║
//  ██║  ██╗╚██████╔╝██║  ██║╚██████╔╝██╔╝   ╚██████╔╝╚██████╔╝██║
//  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═════╝  ╚═════╝ ╚═╝
//  Made by Tomáš Křivan

#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include "KuroGUI.h"
#include "RTClib.h"

KuroGUI::KuroGUI() {
  return;
}

void KuroGUI::begin(LiquidCrystal_I2C* lcd, RTC_DS1307* rtc, uint8_t buzzer_pin){
  _buzzer_pin = buzzer_pin;
  _lcd = lcd;
  _rtc = rtc;
  icon_buffer = new uint8_t[8]{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  create_state(ui_state);
  
}

void KuroGUI::update(){
  switch(ui_state){
    case HOME_SCREEN: {
      if(millis() - ui_timer1 > 1000){
        ui_timer1 = millis();
        b_cache1 = !b_cache1;

        DateTime now = _rtc->now();
        if(now.year() != u_cache1) {
          _lcd->setCursor(11, 0);
          _lcd->printf("%2u%c%02u", now.hour(), b_cache1 ? ':' : ' ', now.minute(), now.day(), now.month(), now.year());
          _lcd->setCursor(0, 1);
          _lcd->printf("%u\.%u\.%u", now.day(), now.month(), now.year());
        } 
        else if (now.month() != u_cache2){
          _lcd->setCursor(11, 0);
          _lcd->printf("%2u%c%02u", now.hour(), b_cache1 ? ':' : ' ', now.minute());
          _lcd->setCursor(0, 1);
          _lcd->printf("%u\.%u", now.day(), now.month());
        }
        else if (now.day() != u_cache3){
          _lcd->setCursor(11, 0);
          _lcd->printf("%2u%c%02u", now.hour(), b_cache1 ? ':' : ' ', now.minute());
          _lcd->setCursor(0, 1);
          _lcd->printf("%u", now.day());
        }
        else if (now.hour() != u_cache4){
          _lcd->setCursor(11, 0);
          _lcd->printf("%2u%c%02u", now.hour(), b_cache1 ? ':' : ' ', now.minute());
        }
        else if (now.minute() != u_cache5){
          _lcd->setCursor(13, 0);
          _lcd->printf("%c%02u", b_cache1 ? ':' : ' ', now.minute());
        }
        else{
          _lcd->setCursor(13, 0);
          _lcd->print(b_cache1 ? ":" : " ");
        }
      }
      break;
    }
  }
}

void KuroGUI::handle_input(uint8_t ui_input){
  switch(ui_input){
    case INPUT_OK: {
      if(ui_state == HOME_SCREEN) create_state(MENU_SELECT);
      break;
    }
    default: {
      Serial.printf("WARN: Unhandled/Unknown input: %u\n", ui_input);
      break;
    }
  }
}

void KuroGUI::create_state(uint8_t state){
  clear_icon_buffer();
  switch(state){
    case HOME_SCREEN: {
      ui_state = HOME_SCREEN;
      b_cache1 = true;
      _lcd->clear();
      _lcd->backlight();
      DateTime now = _rtc->now();
      _lcd->printf("On Standby %2u:%02u", now.hour(), now.minute());
      _lcd->setCursor(0, 1);
      _lcd->printf("%u\.%u\.%u", now.day(), now.month(), now.year());
      u_cache1 = now.year();
      u_cache2 = now.month();
      u_cache3 = now.day();
      u_cache4 = now.hour();
      u_cache5 = now.minute();
      ui_timer1 = millis();
      break;
    }
    case MENU_SELECT: {
      ui_state = MENU_SELECT;
      _lcd->clear();
      _lcd->backlight();
      //_lcd->createChar(ICON_HOME, new uint8_t[8]{ B00000, B00100, B01110, B11111, B01010, B01110, B00000, B00000 });
      //_lcd->write(ICON_HOME);

      write_icon(ICON_HOME, 0, 0);
      write_icon(ICON_LEFT, 1, 0);
      write_icon(ICON_RIGHT, 2, 0);
      write_icon(ICON_CHECK, 3, 0);
      write_icon(ICON_CROSS, 4, 0);
      write_icon(ICON_WRITE, 5, 0);
      write_icon(ICON_SPEAKER, 6, 0);
      write_icon(ICON_WAWES, 7, 0);
      write_icon(ICON_EJECT, 8, 0);
      write_icon(ICON_CLOCKWISE, 9, 0);
      delay(1000);
      negate_icon(ICON_HOME);
      break;
    }
  }
}

bool KuroGUI::write_icon(uint8_t icon, uint8_t row, uint8_t column) {
  for(uint8_t i = 0; i < 8; i++) {
    if(icon_buffer[i] == icon) {
      _lcd->write(i);
      return true;
    }
    if(icon_buffer[i] == NULL) {
      create_icon(icon, i);
      _lcd->setCursor(row, column); // https://forum.arduino.cc/t/is-it-possible-to-modify-my-custom-lcd-characters-in-the-main-loop/394713/3
      _lcd->write(i);
      return true;
    }
  }
  return false;
}

void KuroGUI::clear_icon_buffer() {
  for(uint8_t i = 0; i < 8; i++) {
    icon_buffer[i] = NULL;
  }
}

void KuroGUI::create_icon(uint8_t icon, uint8_t address) {
  icon_buffer[address] = icon;
  _lcd->createChar(address, fetch_icon(icon));
}

void KuroGUI::destroy_icon(uint8_t icon) {
  for(uint8_t i = 0; i < 8; i++) {
    if(icon_buffer[i] == icon) {
      icon_buffer[i] = NULL;
      return;
    }
  }
}

bool KuroGUI::negate_icon(uint8_t icon) {
  for(uint8_t i = 0; i < 8; i++) {
    if(icon_buffer[i] == icon) {
      uint8_t* icon_data = fetch_icon(icon);
      for(uint8_t i_data = 0; i_data < 8; i_data++) {
        icon_data[i_data] = ~icon_data[i_data];
      }
      _lcd->createChar(i, icon_data);
      return true;
    }
  }
  return false;
}

uint8_t* KuroGUI::fetch_icon(uint8_t icon) {
  switch(icon) {
    case ICON_HOME: {
      return new uint8_t[8]{ B00000, B00100, B01110, B11111, B01010, B01110, B00000, B00000 };
      break;
    }
    case ICON_LEFT: {
      return new uint8_t[8]{ B00000, B00010, B00110, B01110, B00110, B00010, B00000, B00000 };
      break;
    }
    case ICON_RIGHT: {
      return new uint8_t[8]{ B00000, B01000, B01100, B01110, B01100, B01000, B00000, B00000 };
      break;
    }
    case ICON_CHECK: {
      return new uint8_t[8]{ B00000, B00000, B00001, B00010, B10100, B01000, B00000, B00000 };
      break;
    }
    case ICON_CROSS: {
      return new uint8_t[8]{ B00000, B00000, B01010, B00100, B01010, B00000, B00000, B00000 };
      break;
    }
    case ICON_WRITE: {
      return new uint8_t[8]{ B00001, B00011, B00111, B01110, B10100, B11000, B00111, B00000 };
      break;
    }
    case ICON_SPEAKER: {
      return new uint8_t[8]{ B00001, B00011, B01111, B01111, B01111, B00011, B00001, B00000 };
      break;
    }
    case ICON_WAWES: {
      return new uint8_t[8]{ B01000, B00100, B10010, B01010, B10010, B00100, B01000, B00000 };
      break;
    }
    case ICON_EJECT: {
      return new uint8_t[8]{ B00000, B00100, B01110, B11111, B00000, B11111, B00000, B00000 };
      break;
    }
    case ICON_CLOCKWISE: {
      return new uint8_t[8]{ B00100, B00110, B11111, B10110, B10100, B10000, B11110, B00000 };
      break;
    }
  }
}