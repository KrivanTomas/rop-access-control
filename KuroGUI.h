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

#define HOME_SCREEN 0x00
#define MENU_SELECT 0x01
#define SETTINGS    0x02
#define CALENDAR    0x03
#define MENU_SELECT 0x04
#define MENU_SELECT 0x05
#define TOAST       0x06
#define TEXT_EDIT   0x07 

#define INPUT_OK    0x00
#define INPUT_UP    0x01
#define INPUT_DOWN  0x02

#define ICON_HOME  0x01
#define ICON_LEFT  0x02
#define ICON_RIGHT  0x03
#define ICON_CHECK  0x04
#define ICON_CROSS  0x05
#define ICON_WRITE  0x06
#define ICON_SPEAKER  0x07
#define ICON_WAWES  0x08
#define ICON_EJECT  0x09
#define ICON_CLOCKWISE  0x0A
#define ICON_ANTI_CLOCKWISE  0x0B
#define ICON_PERSON  0x0C
#define ICON_LOCKED  0x0D
#define ICON_UNLOCKED  0x0E
#define ICON_DOWNLOAD  0x0F
#define ICON_BELL  0x10
#define ICON_HEART  0x11
#define ICON_CELCIUS  0x12
#define ICON_SMILE  0x13


class KuroGUI
{
  public:
    KuroGUI();
    void begin(LiquidCrystal_I2C* lcd, RTC_DS1307* rtc, uint8_t buzzer_pin);
    void update();
    void handle_input(uint8_t ui_input);
  private:
    LiquidCrystal_I2C* _lcd;
    RTC_DS1307* _rtc;
    uint8_t _buzzer_pin;
    uint8_t ui_state;
    unsigned long ui_timer1;
    uint8_t u_cache1;
    uint8_t u_cache2;
    uint8_t u_cache3;
    uint8_t u_cache4;
    uint8_t u_cache5;
    bool b_cache1;
    void create_state(uint8_t state);
    uint8_t* icon_buffer;
    void create_icon(uint8_t icon, uint8_t address);
    void clear_icon_buffer();
    bool write_icon(uint8_t icon, uint8_t row, uint8_t column);
};

#endif