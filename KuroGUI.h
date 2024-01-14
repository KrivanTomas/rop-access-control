//  ██╗  ██╗██╗   ██╗██████╗  ██████╗     ██╗ ██████╗ ██╗   ██╗██╗
//  ██║ ██╔╝██║   ██║██╔══██╗██╔═══██╗   ██╔╝██╔════╝ ██║   ██║██║
//  █████╔╝ ██║   ██║██████╔╝██║   ██║  ██╔╝ ██║  ███╗██║   ██║██║
//  ██╔═██╗ ██║   ██║██╔══██╗██║   ██║ ██╔╝  ██║   ██║██║   ██║██║
//  ██║  ██╗╚██████╔╝██║  ██║╚██████╔╝██╔╝   ╚██████╔╝╚██████╔╝██║
//  ╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ ╚═════╝ ╚═╝     ╚═════╝  ╚═════╝ ╚═╝
//  Made by Tomáš Křivan

#ifndef KuroGUI_h
#define KuroGUI_h
#include "Arduino.h"
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

#define HOME_SCREEN 0x0
#define MENU_SELECT 0x1
#define TOAST       0x2
#define TEXT_EDIT   0x3 

#define INPUT_OK    0x0
#define INPUT_UP    0x1
#define INPUT_DOWN  0x2

class KuroGUI
{
  public:
    KuroGUI();
    void begin(LiquidCrystal_I2C* lcd, RTC_DS1307* rtc);
    void update();
    void handle_input(uint8_t ui_input);
  private:
    LiquidCrystal_I2C* _lcd;
    RTC_DS1307* _rtc;
    uint8_t ui_state;
    unsigned long ui_timer1;
    uint8_t u_cache1;
    uint8_t u_cache2;
    uint8_t u_cache3;
    uint8_t u_cache4;
    uint8_t u_cache5;
    bool b_cache1;
    void create_state(uint8_t state);
};

#endif