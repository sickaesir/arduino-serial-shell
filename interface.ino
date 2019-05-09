
#define baud_rate 115200

void setup() {
  Serial.begin(baud_rate);

}

void loop() {
  if(Serial.available() > 0) {
    byte op = Serial.read();

    switch(op) {
      case 0:
      {
        byte pin = Serial.read();
        byte mode = Serial.read();
        pinMode(pin, mode == 0 ? INPUT : OUTPUT);  
      }
      break;

      case 1:
      {
        byte pin = Serial.read();
        byte rtn = digitalRead(pin);

        Serial.write(rtn == HIGH ? 1 : 0);
      }
      break;

      case 2:
      {
        byte pin = Serial.read();
        byte val = Serial.read();

        digitalWrite(pin, val == 0 ? LOW : HIGH);
      }
      break;
      
      case 3:
      {
        byte pin = Serial.read();
        int rtnVal = analogRead(pin);

        byte b1 = (byte)(rtnVal & 0x000000FF);
        byte b2 = (byte)(rtnVal & 0x0000FF00);

        Serial.write(b1);
        Serial.write(b2);
      }
      break;

      case 4:
      {
        byte pin = Serial.read();
        byte b1 = Serial.read();
        byte b2 = Serial.read();

        int res = (b1 << 8) | b2;

        analogWrite(pin, res);
      }
      break;
    }
    
  }
}
