/**
 * Control two geared motors with encoders
 */

#include <Encoder.h>

/// Simple struct to hold motor control pins
typedef struct {
  byte enablePin;
  byte directionPin;
} MotorContrl;

/// Motor control pins (printed on the motor driver board)
const int E1Pin = 5;
const int M1Pin = 4;
const int E2Pin = 6;
const int M2Pin = 7;

const MotorContrl MotorPin[] = {
  {E1Pin, M1Pin},
  {E2Pin, M2Pin}
};

/// Motor indices for MotorPin[] 
const int M1 = 0;
const int M2 = 1;
const int MotorNum = 2;

/// Motor direction aliases
const int  forward = LOW;
const int backward = HIGH;

/// Encoder pins
const int M1APhase = 18;
const int M1BPhase = 19;
const int M2APhase = 20;
const int M2BPhase = 21;

Encoder M1Encoder(M1APhase, M1BPhase);
Encoder M2Encoder(M2APhase, M2BPhase);


void setupSerial()
{
  Serial.begin(9600);
}

void initMotor()
{
  for ( int i = 0; i < MotorNum; i++ ) {
    digitalWrite(MotorPin[i].enablePin, LOW);
    pinMode(MotorPin[i].enablePin,    OUTPUT);
    pinMode(MotorPin[i].directionPin, OUTPUT);
  }
}

void rotateTest()
{
  turnMotor(M1,  200);
  turnMotor(M1, -200);
  turnMotor(M2,  200);
  turnMotor(M2, -200);
}

/**  motorNumber: M1, M2
direction:          forward, backward **/
void setMotorDirection( int motorNumber, int direction ) {
  digitalWrite( MotorPin[motorNumber].directionPin, direction);
}

/** speed:  0-100   * */
inline void setMotorSpeed( int motorNumber, int speed ) {
  analogWrite(MotorPin[motorNumber].enablePin, 255.0 * (speed / 100.0) ); //PWM
}


void rampMotorSpeed( int motorNumber, int start, int stop )
{
  int inc;
  if ( start < stop ) { inc =  1; }
  else                { inc = -1; }
  for ( int i = start; i <= stop; i+=inc ) {
    setMotorSpeed( motorNumber, i );
    delay(50);
    Serial.print("Position: ");
    Serial.println(M1Encoder.read());
  }
}


void decodeMessage( String message )
{
  int commaPos = message.indexOf(',');
  if ( commaPos < 0 ) {
    return;
  } else {
    int m1Positions = message.substring(          0,         commaPos ).toInt();
    int m2Positions = message.substring( commaPos+1, message.length() ).toInt();
    turnMotor( M1, m1Positions );
    turnMotor( M2, m2Positions );
    /* Serial.println("Done turning"); */
  }
}


void printPositions( int positions )
{
  Serial.print("Turning ");
  Serial.print(positions);
  Serial.println(" positions...");
}

bool stillTurning( int positions, Encoder* enc, unsigned long startTime, int timeout )
{
  unsigned long dt = (millis() - startTime);
  Serial.println(dt);
  /* return abs(enc->read()) < abs(positions) && dt > 0 && dt < timeout ; */
  return abs(enc->read()) < abs(positions);
}

void turnMotor( int motorNumber, int positions )
{
  Encoder* enc;
  if ( motorNumber == M1 ) {
    enc = &M1Encoder;
  } else {
    enc = &M2Encoder;
  }
  enc->write(0);
  if ( positions > 0 ) {
    setMotorDirection( motorNumber, forward );
  }
  else if ( positions < 0 ) {
    setMotorDirection( motorNumber, backward );
  }
  setMotorSpeed( motorNumber, 100 );
  unsigned long startTime = millis();
  unsigned long timeout = 3000;  // seconds
  while ( stillTurning( positions, enc, startTime, timeout ) ) {
    if ( millis() - startTime > timeout ) {
      break;
    }
    delay(50);
  }
  setMotorSpeed( motorNumber,   0 );
}


void setup()
{
  setupSerial();
  initMotor();
  rotateTest();
}


void loop()
{
  while ( Serial.available() > 0 ) {
    String input = Serial.readString();
    decodeMessage( input );
  }
}
