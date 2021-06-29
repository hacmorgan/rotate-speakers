/**
 * Control two geared motors with encoders
 *
 * Reads ascii messages on serial of the form <m1positions>,<m2positions>
 * and turns motors accordingly. 
 * 
 * Conversion to degrees is handled externally
 *
 * @author  Hamish Morgan
 * @license BSD
 * @date    27/06/2021
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
  { E1Pin, M1Pin },
  { E2Pin, M2Pin }
};

/// Motor indices for MotorPin[] 
const int M1 = 0;
const int M2 = 1;
const int NumMotors = 2;

/// Motor direction aliases
const int  FORWARD = LOW;
const int BACKWARD = HIGH;

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

void initialiseMotors()
{
  for ( int i = 0; i < NumMotors; i++ ) {
    digitalWrite(MotorPin[i].enablePin, LOW);
    pinMode(MotorPin[i].enablePin,    OUTPUT);
    pinMode(MotorPin[i].directionPin, OUTPUT);
  }
}

void rotateTest()
{
  int  forward[] = {  200,  200 };
  int backward[] = { -200, -200 };
  turnMotors(  forward );
  turnMotors( backward );
}

void setMotorDirection( int motorNumber, int direction ) {
  digitalWrite( MotorPin[motorNumber].directionPin, direction);
}

inline void setMotorSpeed( int motorNumber, int speed ) {
  analogWrite( MotorPin[motorNumber].enablePin,
               255.0 * (speed / 100.0)          ); //PWM
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
    int motorPositions[] = {
      message.substring(          0,         commaPos ).toInt(),
      message.substring( commaPos+1, message.length() ).toInt()
    };
    turnMotors( motorPositions );
  }
}


void printPositions( int positions )
{
  Serial.print("Turning ");
  Serial.print(positions);
  Serial.println(" positions...");
}

bool stillTurning( int motorNumber, int positions )
{
  if ( motorNumber == M1 ) {
    return abs(M1Encoder.read()) < abs(positions);
  } else {
    return abs(M2Encoder.read()) < abs(positions);
  }
}

bool eitherStillTurning( int* positions )
{
  return abs(M1Encoder.read()) < abs(positions[M1]) ||
         abs(M2Encoder.read()) < abs(positions[M2]);
}

void resetEncoders()
{
  M1Encoder.write(0);
  M2Encoder.write(0);
}

void setMotorDirections( int* positions )
{
  for ( int i = 0; i < NumMotors; i++ ) {
    if ( positions[i] > 0 ) {
        setMotorDirection( i, FORWARD );
    }
    else if ( positions[i] < 0 ) {
        setMotorDirection( i, BACKWARD );
    }
  }
}

void motorsOn( int* positions )
{
  for ( int i = 0; i < NumMotors; i++ ) {
    if ( positions[i] != 0 ) {
      setMotorSpeed( i, 100 );
    } else {
      setMotorSpeed( i,   0 );
    }
  }
}

void motorOff( int motorNumber )
{
  setMotorSpeed( motorNumber, 0 );
}

void motorsOff()
{
  for ( int i = 0; i < NumMotors; i++ ) {
    setMotorSpeed( i, 0 );
  }
}

bool notTimedOut( unsigned long startTime, unsigned long timeout )
{
  return millis() - startTime < timeout;
}

void turnMotors( int positions[] )
{
  resetEncoders();
  setMotorDirections( positions );
  motorsOn( positions );
  unsigned long startTime = millis();
  unsigned long timeout = 3000;  // milliseconds
  while ( eitherStillTurning( positions ) &&
          notTimedOut( startTime, timeout ) ) {
    for ( int i = 0; i < NumMotors; i++ ) {
      if ( ! stillTurning( i, positions[i] ) ) {
        motorOff(i);
      }
    }
    delay(10);
  }
  motorsOff();
}


void setup()
{
  setupSerial();
  initialiseMotors();
  rotateTest();
}


void loop()
{
  while ( Serial.available() > 0 ) {
    String input = Serial.readString();
    decodeMessage( input );
  }
}
