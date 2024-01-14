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

void KuroGUI::begin(LiquidCrystal_I2C* lcd, RTC_DS1307* rtc){
  _lcd = lcd;
  _rtc = rtc;
  create_state(ui_state);
}

void KuroGUI::update(){
  switch(ui_state){
    case HOME_SCREEN:
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

void KuroGUI::handle_input(uint8_t ui_input){
  switch(ui_input){
    default:
      Serial.printf("WARN: Unhandled/Unknown input: %u\n", ui_input);
      break;
  }
}

void KuroGUI::create_state(uint8_t state){
  switch(state){
    case HOME_SCREEN:
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
}