// see: https://starter-kit.nettigo.pl/2011/11/pcf8574-czyli-jak-latwo-zwiekszyc-liczbe-pinow-w-arduino/

#include <PCF8574.h>
#include <Wire.h>

const auto BASE_ADDRESS_PCF = 0b0100000;
const auto DRIVER_ADDRESS_TWI = 0b00000111 | BASE_ADDRESS_PCF;

const uint8_t maskMotorFwd[] { 
  0b00000001,
  0b00000100,
  0b00010000,
  0b01000000,
};

const uint8_t maskMotorBack[] = {
  0b00000010,
  0b00001000,
  0b00100000,
  0b10000000,
};

static_assert(sizeof(maskMotorFwd) == sizeof(maskMotorBack), 
            "size of maskMotorFwd must be equal to size of maskMotorBack");

void setup() {
  Wire.begin();
  Serial.begin(115200);
}

void loop() {

    static uint8_t state = 0xff;

    Serial.print("state: ");
    Serial.println(state);


    Serial.println("Wire.beginTransmission");
    Wire.beginTransmission(DRIVER_ADDRESS_TWI);
    
    Serial.println("Wire.write");
    auto written = Wire.write(state);
    Serial.print("written: ");
    Serial.println(written);
    
    Serial.println("Wire.endTransmission");
    auto status = Wire.endTransmission(false);
    Serial.print("status: ");
    Serial.println(status);
    
    state = state ? 0 : 0xff;

    delay(3000);
  
return;

 static auto motorNumber = 0;
 static uint8_t currentState = 0;

 if (Serial.available())  {
    auto input = Serial.read();
    Serial.write(input);//send what has been received
//    Serial.println();   //print line feed character
    switch (input)
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
    
  }
}

