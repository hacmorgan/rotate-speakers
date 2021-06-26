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


void printPositions( int positions )
{
  Serial.print("Turning ");
  Serial.print(positions);
  Serial.println(" positions...");
}

void turnMotor( int motorNumber, int positions )
{
  M1Encoder.write(0);
  /* printPositions( positions ); */
  if ( positions > 0 ) {
    setMotorDirection( motorNumber, forward );
    setMotorSpeed( motorNumber, 100 );
    while ( M1Encoder.read() < positions ) { ; }
    setMotorSpeed( motorNumber,   0 );
  }
  else if ( positions < 0 ) {
    setMotorDirection( motorNumber, backward );
    setMotorSpeed( motorNumber, 100 );
    while ( M1Encoder.read() > positions ) { ; }
    setMotorSpeed( motorNumber,   0 );
  }
}


void setup()
{
  setupSerial();
  initMotor();
}


void loop()
{
  M1Encoder.write(0);
  int positions = 0;
  while ( true ) {
    turnMotor( M1, Serial.parseInt() );
  }
}
