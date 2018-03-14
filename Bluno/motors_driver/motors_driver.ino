#include <Wire.h>

const auto DRIVER_ADDRESS_TWI = 0;

const int8_t maskMotorFwd[] { 
  0b00000001,
  0b00000100,
  0b00010000,
  0b01000000,
};

const int8_t maskMotorBack[] = {
  0b00000010,
  0b00001000,
  0b00100000,
  0b10000000,
};

static_assert(sizeof(maskMotorFwd) == sizeof(maskMotorBack), 
            "size of maskMotorFwd must be equal to size of maskMotorBack");

void setup() {
  Serial.begin(115200);
}

void loop() {


 static auto motorNumber = 0;
 static int8_t currentState = 0;

 if (Serial.available())  {
    switch (Serial.read())
    {
      // forward
      case 'f':
      case 'F':
        currentState |=  maskMotorFwd[motorNumber];
        currentState &= ~maskMotorBack[motorNumber];
      break;
  
      // reverse
      case 'r':
      case 'R':
        currentState &= ~maskMotorFwd[motorNumber];
        currentState |=  maskMotorBack[motorNumber];
      break;
  
      // soft stop - zero voltage in both directions
      case 's':
      case 'S':
        currentState &= ~maskMotorFwd[motorNumber];
        currentState &= ~maskMotorBack[motorNumber];
      break;
  
      // break - high level in both directions
      case 'b':
      case 'B':
        currentState |= maskMotorFwd[motorNumber];
        currentState |= maskMotorBack[motorNumber];
      break;
    }
    
    Wire.beginTransmission(DRIVER_ADDRESS_TWI);
    Wire.write(currentState);
    Wire.endTransmission();
    
    motorNumber = (motorNumber+1) % 4;
    
//    Serial.write(Serial.read());//send what has been received
//    Serial.println();   //print line feed character
  }

//  delay(1000);
}
