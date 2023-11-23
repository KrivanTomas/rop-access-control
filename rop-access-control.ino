#define SERIAL_BAUD 9600
#define BUZZER_PIN 27
#define RFID_UART_PORT 2

#define RFID_TOKEN_LENGTH 12

#include <LiquidCrystal_I2C.h>
#include <HardwareSerial.h>
#include "Kuro.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);          // I2C address 0x27, 16 column and 2 rows
HardwareSerial rfid_serial(RFID_UART_PORT);//Serial port 2
Kuro kuro;

unsigned long rfid_last_active;

void setup() {
  Serial.begin(SERIAL_BAUD);  // debug console
  rfid_serial.begin(SERIAL_BAUD, SERIAL_8N1);
  lcd.init();  // initialize the lcd
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.backlight();
  lcd.print("RFID reader");


  rfid_last_active = millis();
}

void loop() {
  //handle_rfid();
  uint8_t token[RFID_TOKEN_LENGTH];
  unsigned long then = millis();
  if (kuro.read_token_from_uart(&rfid_serial, token)) {
    lcd.clear();
  lcd.print("ID:");
    for (uint8_t frag : token) {
      lcd.write(frag);
    }
  }
}
