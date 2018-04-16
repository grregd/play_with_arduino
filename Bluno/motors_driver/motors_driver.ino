// see: https://starter-kit.nettigo.pl/2011/11/pcf8574-czyli-jak-latwo-zwiekszyc-liczbe-pinow-w-arduino/


// Makeblock IR device codes:
// FFA25D A
// FF629D B
// FFE21D C
// FF22DD D
// FFC23D E
// FF6897 0
// FF30CF 1
// FF18E7 2
// FF7A85 3
// FF10EF 4
// FF38C7 5
// FF5AA5 6
// FF42BD 7
// FF4AB5 8
// FF52AD 9
// FF02FD UP
// FF9867 DOWN
// FFE01F LEFT
// FF906F RIGHT
// FFA857 GEAR



#include <Wire.h>
#include <IRremote.h>

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

IRrecv irrecv(/*pinNum*/4);

void setup()
{
  Wire.begin();
  Serial.begin(115200);
  irrecv.enableIRIn();
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

class TankMotors
{
  private:
    const uint8_t M1 = 0;
    const uint8_t M2 = 1;
    Motors motors;

  public:
    void forward()
    {
      motors.fwd(M1);
      motors.fwd(M2);
    }
    
    void reverse()
    {
      motors.rev(M1);
      motors.rev(M2);
    }
    
    void turnLeft()
    {
      motors.fwd(M1);
      motors.rev(M2);
    }
    
    void turnRight()
    {
      motors.rev(M1);
      motors.fwd(M2);
    }

    void stop()
    {
      motors.stop(M1);
      motors.stop(M2);
    }

    uint8_t state() const
    {
      return motors.state();
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

void hanldeFromIR(TankMotors & motors, IRrecv & irrecv);

void hanldeFromIR(TankMotors & motors, IRrecv & irrecv)
{
  decode_results results;
  static decltype(results.value) lastValue = 0;
  
 if (irrecv.decode(&results))
 {
    Serial.println(results.value, HEX);

    decltype(results.value) value = (results.value == 0xFFFFFFFF) 
      ? lastValue : results.value;

    switch (value)
    {
    case 0xFF02FD: // UP
      motors.forward();
    break;
    
    case 0xFF9867: // DOWN
      motors.reverse();
    break;
    
    case 0xFFE01F: // LEFT
      motors.turnLeft();
    break;
    
    case 0xFF906F: // RIGHT
      motors.turnRight();
    break;
    
    case 0xFFA857: // GEAR
      motors.stop();
    break;
    }
    
    Serial.print("Motors state: ");
    Serial.println(motors.state(), BIN);

    lastValue = value;
    
    irrecv.resume();
 }
 else
 {
//    motors.stop();
 }
}

  static TankMotors motors;
void loop()
{

  static auto motorNumber = 0;

//  if (Serial.available())
  {
    // handleFromSerial(motors, motorNumber);
    hanldeFromIR(motors, irrecv);
    
//    Serial.print("Current state: ");
//    Serial.println(motors.state(), BIN);

    Wire.beginTransmission(DRIVER_ADDRESS_TWI);
    Wire.write(motors.state());
    Wire.endTransmission();
    
//    motorNumber = (motorNumber+1) % 4;
    delay(0);
  }
}

