#include <Wire.h>

const auto DRIVER_ADDRESS_TWI = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {

 if (Serial.available())  {
//    Serial.write(Serial.read());//send what has been received
//    Serial.println();   //print line feed character
  }
    
  Wire.beginTransmission(DRIVER_ADDRESS_TWI);
  Wire.write(1);
  Wire.endTransmission();
  delay(1000);
}
