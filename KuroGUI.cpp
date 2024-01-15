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

#define MENU_ITEMS_COUNT 6

KuroGUI::KuroGUI() {
  return;
}

void KuroGUI::begin(LiquidCrystal_I2C* lcd, RTC_DS1307* rtc, uint8_t buzzer_pin){
  _buzzer_pin = buzzer_pin;
  _lcd = lcd;
  _rtc = rtc;
  icon_buffer = new uint8_t[8]{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
  create_state(HOME_SCREEN);
  play_sound(SOUND_COMPLETE);
}

void KuroGUI::create_state(uint8_t state) {
  clear_icon_buffer();
  ui_state = state;
  switch(state){
    case NO_MENU: {
      _lcd->clear();
      _lcd->backlight();
      _lcd->print("NO STATE SET");
      _lcd->setCursor(0,1);
      _lcd->print("DEF TO HOME ..");
      play_sound(SOUND_ERROR);
      delay(1000);
      create_state(HOME_SCREEN);
    }
    case HOME_SCREEN: {
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
      _lcd->clear();
      _lcd->backlight();

      uint8_t icon;
      uint8_t menu;
      char* desc;
      for(uint8_t i = 0; i < MENU_ITEMS_COUNT; i++){
        get_menu_item(i, &icon, &desc, &menu);
        write_icon(icon, i * 2, 0);
      }
      
      u_cache1 = 0; //menu index
      get_menu_item(u_cache1, &icon, &desc, &menu);
      negate_icon(icon);
      _lcd->setCursor(0, 1);
      _lcd->print(desc);
      break;
    }
  }
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

void KuroGUI::handle_input(uint8_t input, uint8_t input_state){
  switch(ui_state){
    case HOME_SCREEN: {
      if (input_state == BEFORE_INPUT) return;
      create_state(MENU_SELECT);
      break;
    }
    case MENU_SELECT: {
      if (input_state == BEFORE_INPUT) return;
      uint8_t icon;
      uint8_t menu;
      char* desc;
      get_menu_item(u_cache1, &icon, &desc, &menu);
      if(input == INPUT_OK){
        create_state(menu);
        return;
      }
      restore_icon(icon);
      if(input == INPUT_UP) u_cache1 = u_cache1 == 0 ? MENU_ITEMS_COUNT - 1 : u_cache1 - 1;
      if(input == INPUT_DOWN) u_cache1 = u_cache1 == MENU_ITEMS_COUNT - 1 ? 0 : u_cache1 + 1;
      get_menu_item(u_cache1, &icon, &desc, &menu);
      negate_icon(icon);
      _lcd->setCursor(0, 1);
      _lcd->print(desc);
      break;
    }
  }
}



bool KuroGUI::get_menu_item(uint8_t menu_index, uint8_t* _icon, char** _desc, uint8_t* _menu){
  switch(menu_index){
    case 0: {
      *_icon = ICON_HOME;
      *_desc = "Home           ";
      *_menu = HOME_SCREEN;
      break;
    }
    case 1: {
      *_icon = ICON_PERSON;
      *_desc = "User data       ";
      *_menu = NO_MENU;
      break;
    }
    case 2: {
      *_icon = ICON_EJECT;
      *_desc = "SD data         ";
      *_menu = NO_MENU;
      break;
    }
    case 3: {
      *_icon = ICON_WAWES;
      *_desc = "Wireless status ";
      *_menu = NO_MENU;
      break;
    }
    case 4: {
      *_icon = ICON_SLIDERS;
      *_desc = "Settings        ";
      *_menu = NO_MENU;
      break;
    }
    case 5: {
      *_icon = ICON_HEART;
      *_desc = "About           ";
      *_menu = NO_MENU;
      break;
    }
    default: {
      *_icon = 'E';
      *_desc = "Error           ";
      *_menu = NO_MENU;
      return false;
      break;
    }
  }
  return true;
}

bool KuroGUI::write_icon(uint8_t icon, uint8_t column, uint8_t row) {
  for(uint8_t i = 0; i < 8; i++) {
    if(icon_buffer[i] == icon) {
      _lcd->setCursor(column, row);
      _lcd->write(i);
      return true;
    }
    if(icon_buffer[i] == NULL) {
      create_icon(icon, i);
      _lcd->setCursor(column, row); // https://forum.arduino.cc/t/is-it-possible-to-modify-my-custom-lcd-characters-in-the-main-loop/394713/3
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

bool KuroGUI::restore_icon(uint8_t icon) {
  for(uint8_t i = 0; i < 8; i++) {
    if(icon_buffer[i] == icon) {
      uint8_t* icon_data = fetch_icon(icon);
      _lcd->createChar(i, icon_data);
      return true;
    }
  }
  return false;
}

void KuroGUI::delete_char_at(uint8_t column, uint8_t row) {
  _lcd->setCursor(column, row);
  _lcd->write(' ');
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
    case ICON_ANTI_CLOCKWISE: {
      return new uint8_t[8]{ B01111, B00001, B00101, B01101, B11111, B01100, B00100, B00000 };
      break;
    }
    case ICON_PERSON: {
      return new uint8_t[8]{ B01110, B10001, B10001, B10001, B01010, B10001, B11111, B00000 };
      break;
    }
    case ICON_LOCKED: {
      return new uint8_t[8]{ B01110, B01010, B01010, B11111, B11011, B11011, B01110, B00000 };
      break;
    }
    case ICON_UNLOCKED: {
      return new uint8_t[8]{ B01110, B01010, B01000, B11111, B11011, B11011, B01110, B00000 };
      break;
    }
    case ICON_DOWNLOAD: {
      return new uint8_t[8]{ B00100, B00100, B10101, B01110, B00100, B00000, B11111, B00000 };
      break;
    }
    case ICON_BELL: {
      return new uint8_t[8]{ B00100, B01110, B01110, B01110, B11111, B00000, B00100, B00000 };
      break;
    }
    case ICON_HEART: {
      return new uint8_t[8]{ B00000, B01010, B11111, B11111, B01110, B00100, B00000, B00000 };
      break;
    }
    case ICON_FULL_WHITE: {
      return new uint8_t[8]{ B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111 };
      break;
    }
    case ICON_SLIDERS: {
      return new uint8_t[8]{ B00000, B00010, B11111, B00010, B01000, B11111, B01000, B00000 };
      break;
    }
    case ICON_WRENCH: {
      return new uint8_t[8]{ B00000, B01010, B01110, B00100, B00100, B00100, B00000, B00000 };
      break;
    }
  }
}

void KuroGUI::play_sound(uint8_t sound_id) {
  switch(sound_id){
    case SOUND_ERROR: {
      tone(_buzzer_pin, 587, 30);
      tone(_buzzer_pin, 30, 10);
      tone(_buzzer_pin, 100, 10);
      tone(_buzzer_pin, 30, 10);
      tone(_buzzer_pin, 100, 10);
      tone(_buzzer_pin, 30, 10);
      tone(_buzzer_pin, 100, 10);
      tone(_buzzer_pin, 30, 10);
      tone(_buzzer_pin, 100, 10);
      break;
    }
    case SOUND_COMPLETE: {
      tone(_buzzer_pin, 880, 100);
      tone(_buzzer_pin, 784, 100);
      tone(_buzzer_pin, 698, 100);
      tone(_buzzer_pin, 0, 100);
      tone(_buzzer_pin, 784, 100);
      tone(_buzzer_pin, 880, 100);
      tone(_buzzer_pin, 1046, 100);
      break;
    }
  }
}