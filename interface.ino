#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define baud_rate 115200

// [ (4b) req-id ][ (1b) opcode ][ (4b) param 1 ][ (4b) param 2 ][ (4b) param 3 ][ (4b) param 3 ]

struct request_t {
  int request_id;
  byte opcode;
  int param1;
  int param2;
  int param3;
  int param4;
};

int get_serial_int() {
  byte b1 = Serial.read();
  byte b2 = Serial.read();
  byte b3 = Serial.read();
  byte b4 = Serial.read();

  return b1 | b2 | b3 | b4;
}

void put_serial_int(int value) {
    byte b1 = (byte)(value & 0x000000FF);
    byte b2 = (byte)(value & 0x0000FF00);
    byte b3 = (byte)(value & 0x00FF0000);
    byte b4 = (byte)(value & 0xFF000000);
  
  Serial.write(b1);
  Serial.write(b2);
  Serial.write(b3);
  Serial.write(b4);
}

void exec_pinmode() {
        byte pin = Serial.read();
        byte mode = Serial.read();
        pinMode(pin, mode == 0 ? INPUT : OUTPUT);  
}

void exec_digitalwrite() {
        byte pin = Serial.read();
        byte val = Serial.read();

        digitalWrite(pin, val == 0 ? LOW : HIGH);
}

void exec_digitalread() {
        byte pin = Serial.read();
        byte rtn = digitalRead(pin);

        Serial.write(rtn == HIGH ? 1 : 0);
}

void exec_analogread() {
        byte pin = Serial.read();
        int rtnVal = analogRead(pin);

        byte b1 = (byte)(rtnVal & 0x000000FF);
        byte b2 = (byte)(rtnVal & 0x0000FF00);

        Serial.write(b1);
        Serial.write(b2);
}

void exec_analogwrite() {
        byte pin = Serial.read();
        byte b1 = Serial.read();
        byte b2 = Serial.read();

        int res = b1 | b2;

        analogWrite(pin, res);
}

void exec_eeprom_length() {
  put_serial_int(EEPROM.length());
}

void exec_eeprom_write() {
  int address = get_serial_int();
  byte value = Serial.read();
  EEPROM.write(address, value);
}

void exec_eeprom_read() {
  int address = get_serial_int();
  byte value = EEPROM.read(address);
  Serial.write(value);
}

void exec_lcd_render() {
  for(int i = 0; i < 20 * 4; i++) {
    lcd.setCursor(i / 20, i % 20);

    byte b = Serial.read();
    lcd.print(b);
  }
}

void (*opTable[])() = {
  &exec_pinmode,
  &exec_digitalread,
  &exec_digitalwrite,
  &exec_analogread,
  &exec_analogwrite,
  &exec_eeprom_length,
  &exec_eeprom_write,
  &exec_eeprom_read,
  &exec_lcd_render
  
};

void setup() {
  Serial.begin(baud_rate); 
  lcd.begin();
  lcd.backlight();
}

void loop() {
  if(Serial.available() > 0) {
    byte op = Serial.read();
    void (*exec)() = opTable[op];

    exec();

  }
}
