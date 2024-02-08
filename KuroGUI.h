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
#include "KuroUTIL.h"
#include "RTClib.h"

#define MENU_SELECT_COUNT  6

#define NO_MENU     0x00
#define HOME_SCREEN 0x01
#define MAIN_MENU   0x02
#define SETTINGS    0x03
#define CALENDAR    0x04
#define USER_DATA   0x05
#define ABOUT       0x06
#define SD_DATA     0x07
#define WIRELESS_DATA 0x08
#define TOAST  0x09
#define TEXT_EDIT   0x0A
#define AUTHORIZATION   0x0B
#define USERS_LIST 0x0C
#define USERS_MENU 0x0D
#define ADD_USER 0x0E

#define SOUND_ERROR 0x01
#define SOUND_COMPLETE 0x02
#define SOUND_WHISTLE 0x03

#define BEFORE_INPUT  0x00
#define AFTER_INPUT   0x01

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
#define ICON_SLIDERS  0x12
#define ICON_WRENCH  0x13
#define ICON_FULL_WHITE  0x14
#define ICON_WIRELESS  0x15
#define ICON_PLUS  0x16


#define REQUEST_NEW_USER  0b10000000

class KuroGUI
{
  public:
    KuroGUI();
    ~KuroGUI();
    void begin(LiquidCrystal_I2C* lcd, RTC_DS1307* rtc, uint8_t buzzer_pin);
    void update();
    void handle_input(uint8_t input, uint8_t input_state = BEFORE_INPUT);
    bool show_toast(char* message, uint16_t timeout = 0);
    void play_sound(uint8_t sound_id);
    bool require_authorization(uint8_t* authority_requirement);
    bool query_requests(uint8_t* requests, uint8_t rfid[12]);
    void comply_with_authorization(uint8_t rfid_token[12]);
    void cancel_authorization();
  private:
    LiquidCrystal_I2C* _lcd;
    RTC_DS1307* _rtc;
    uint8_t _buzzer_pin;
    uint8_t ui_state;
    unsigned long ui_timer1;
    unsigned long toast_timer;
    uint8_t ui_toast_cache;
    char* message_buffer;
    uint8_t user_rfid_token[12];
    uint8_t authority_request;
    uint8_t u_cache1;
    uint8_t u_cache2;
    uint8_t u_cache3;
    uint8_t u_cache4;
    uint8_t u_cache5;
    bool b_cache1;
    uint8_t gui_requests; // flag register = create new user | ? | ? | ? | ? | ? | ? | ? 
    void create_state(uint8_t state);
    uint8_t* icon_buffer;
    uint8_t* fetch_icon(uint8_t icon);
    bool get_menu_item(uint8_t menu_index, uint8_t* icon_out, char** description_out, uint8_t* menu_out, bool* authorize, uint8_t* authority_value);
    void create_icon(uint8_t icon, uint8_t address);
    void destroy_icon(uint8_t icon);
    bool negate_icon(uint8_t icon);
    bool restore_icon(uint8_t icon);
    void delete_char_at(uint8_t column, uint8_t row);
    void clear_icon_buffer();
    bool write_icon(uint8_t icon, uint8_t column, uint8_t row);
    void build_menu(uint8_t item_count);
    void input_to_menu(uint8_t item_count, uint8_t input, uint8_t input_state);
};

#endif