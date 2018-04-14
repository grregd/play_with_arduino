// see: https://starter-kit.nettigo.pl/2011/11/pcf8574-czyli-jak-latwo-zwiekszyc-liczbe-pinow-w-arduino/

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

void setup()
{
  Wire.begin();
  Serial.begin(115200);
}

class Motors
{
private:
  uint8_t currentState;

public:
  Motors()
  : currentState(0)
  {}

  void fwd(uint8_t motorNumber)
  {
    currentState |=  maskMotorFwd[motorNumber];
    currentState &= ~maskMotorBack[motorNumber];
  }

  void rev(uint8_t motorNumber)
  {
    currentState &= ~maskMotorFwd[motorNumber];
    currentState |=  maskMotorBack[motorNumber];
  }

  // soft stop - zero voltage in both directions
  void stop(uint8_t motorNumber)
  {
    currentState &= ~maskMotorFwd[motorNumber];
    currentState &= ~maskMotorBack[motorNumber];
  }

  // break - high level in both directions
  void brake(uint8_t motorNumber)
  {
    currentState |= maskMotorFwd[motorNumber];
    currentState |= maskMotorBack[motorNumber];
  }

  uint8_t state() const
  {
    return currentState;
  }
};

void handleFromSerial(Motors & motors, uint8_t motorNumber);

void handleFromSerial(Motors & motors, uint8_t motorNumber)
{
    auto input = Serial.read();
    Serial.write(input);//send what has been received
    Serial.println();

    switch (input)
    {
      // forward
      case 'f':
      case 'F':
        motors.fwd(motorNumber);
      break;
  
      // reverse
      case 'r':
      case 'R':
        motors.rev(motorNumber);
      break;
  
      // soft stop - zero voltage in both directions
      case 's':
      case 'S':
        motors.stop(motorNumber);
      break;
  
      // break - high level in both directions
      case 'b':
      case 'B':
        motors.brake(motorNumber);
      break;
    }
    
}

void loop()
{

  static auto motorNumber = 0;
  Motors motors;

  if (Serial.available())
  {
    handleFromSerial(motors, motorNumber);
    
    Serial.print("Current state: ");
    Serial.println(motors.state(), BIN);

    Wire.beginTransmission(DRIVER_ADDRESS_TWI);
    Wire.write(motors.state());
    Wire.endTransmission();
    
    motorNumber = (motorNumber+1) % 4;
    
  }
}

