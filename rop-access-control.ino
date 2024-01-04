#define SERIAL_BAUD 9600
#define BUZZER_PIN 27
#define RFID_UART_PORT 2
#define SDCARD_CS 5

#define ENC_CLK 4
#define ENC_DT 0
#define ENC_SW 2

#define RFID_TOKEN_LENGTH 12

#define VOID_DURATION 500

#include <LiquidCrystal_I2C.h>
#include <HardwareSerial.h>
// #include <SD.h>
#include "Kuro.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);          // I2C address 0x27, 16 column and 2 rows
HardwareSerial rfid_serial(RFID_UART_PORT);//Serial port 2
Kuro kuro;
// File myFile;
unsigned long rfid_last_read;
bool rfid_void_frag;
// bool sd_connected;

#include <IRremote.hpp>

#define IR_RECEIVE_PIN    12
#if !defined(STR_HELPER)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#endif
void setup() {
  Serial.begin(SERIAL_BAUD);  // debug console
  rfid_serial.begin(SERIAL_BAUD, SERIAL_8N1);
  lcd.init();  // initialize the lcd
  pinMode(BUZZER_PIN, OUTPUT);

  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);

  lcd.backlight();
  //lcd.print("RFID reader");
  home_screen();

  rfid_last_read = millis();
  rfid_void_frag = true;

  // if (!SD.begin(SDCARD_CS)) {
  //   Serial.println(F("SD CARD FAILED, OR NOT PRESENT!"));
  //   sd_connected = false;
  // }
  // else {
  //   Serial.println(F("SD CARD DETECTED"));
  //   sd_connected = true;
  // }

  // if(sd_connected) {
  //   if (!SD.exists("arduino.txt")) {
  //     Serial.println(F("arduino.txt doesn't exist. Creating arduino.txt file..."));
  //       // create a new file by opening a new file and immediately close it
  //       myFile = SD.open("arduino.txt", FILE_WRITE);
  //       myFile.close();
  //   }

  //   // recheck if file is created or not
  //   if (SD.exists("arduino.txt"))
  //     Serial.println(F("arduino.txt exists on SD Card."));
  //   else
  //     Serial.println(F("arduino.txt doesn't exist on SD Card."));
  // }
}


bool on_input = false;
char current_letter = ' ';
int cursor_pos = 0;

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.resume(); // Enable receiving of the next value
    if (IrReceiver.decodedIRData.command == 0x55 && on_input) { //left
        slide_letter(&current_letter, -1, cursor_pos);
        tone(BUZZER_PIN, 200, 10);
    } else if (IrReceiver.decodedIRData.command == 0x4E && on_input) { //right
        slide_letter(&current_letter, 1, cursor_pos);
        tone(BUZZER_PIN, 200, 10);
    } else if (IrReceiver.decodedIRData.command == 0x1F) { //ok
        if(!on_input) {
          text_input();
        on_input = true;
        cursor_pos = 0;
        current_letter = ' ';
        }
        else {
          cursor_pos++;
          lcd.setCursor(cursor_pos, 0);
        }
        tone(BUZZER_PIN, 200, 10);
    }
  }

  uint8_t token[RFID_TOKEN_LENGTH];
  if (kuro.void_frag(&rfid_serial, &rfid_void_frag, &rfid_last_read, VOID_DURATION) && kuro.read_token_from_uart(&rfid_serial, token)) {
    lcd.clear();
    lcd.print("ID:");
    for (uint8_t frag : token) {
      lcd.write(frag);
    }
    tone(BUZZER_PIN, 1000, 50);
    rfid_last_read = millis();
    rfid_void_frag = true;
    no_text_input();
    on_input = false;
  }
}

void home_screen(){
  lcd.clear();
  lcd.home();
  lcd.print("Pohotovost 13:20");
}

void text_input(){
  lcd.cursor();
  lcd.clear();
}

void slide_letter(char *letter, uint8_t amount, int cur){
  if(*letter == ' '){
    *letter += amount;
    if(*letter > ' ') *letter = 'A';
    else *letter = 'Z';
  }
  else {
    *letter += amount;
    if(*letter > 'Z'|| *letter < 'A') *letter = ' ';
  }
  lcd.write(*letter);
  lcd.setCursor(cur, 0);
}

void no_text_input(){
  lcd.noCursor();
}
