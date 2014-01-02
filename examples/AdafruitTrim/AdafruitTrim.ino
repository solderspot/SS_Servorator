//
//  
//  AdafruitTrim.ino
//
// Copyright (c) 2014, Solder Spot
// All rights reserved. 

// This sketch requires the following libraries:
//   SS_Servorator: https://github.com/solderspot/SS_Servorator
//   SS_ServoTrim: https://github.com/solderspot/SS_ServoTrim
//   Adafruit_PWMServoDriver: https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

// include needed librarys
#include <SS_Servorator.h>
#include <SS_ServoTrim.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define NUM_SERVOS 16

// create the servo driver instance
// change 0x40 to match your servo shield if necessary
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);  

SS_Servorator sr(NUM_SERVOS);
SS_ServoTrim trim(NUM_SERVOS);

// the servo handler for Servorator
void update_servo( SS_Index index, SS_Angle angle, void *data)
{
  // SS_Angle is in 1000th of a degree

  long servo_index = trim.getServoNumber( index );
  long time = trim.getServoPulseTime(index, angle);

  // 4096 ticks is 20,000 us (50Hz)
  long ticks = (4096L*time)/20000L;
  // update the servo channel with the new pusle
  pwm.setPWM(servo_index, 0, ticks);

}

void setup() 
{

  Serial.begin(9600);

  // init Adafruit's driver and set pulse frequency
  pwm.begin();
  pwm.setPWMFreq(50);  

  // register servo handler
  sr.setServoHandler( update_servo, NULL);

  // set all servos at 45 degrees
  // servo 0 is fastest
  SS_Velocity vel = SS_FAST_RATE/2;
  for ( int i=0; i<NUM_SERVOS;i++)
  {
    sr.setServoTargetAngle(i, SS_DEGREES(45)); 
    sr.setServoMaxVelocity( i, vel );
    vel = vel / 2 ;
  }

  // experiment with the following settings to see how
  // the trim library works.

  // set up trims - this are just made up settings
  trim.setServoPulseRange( 0, 1200, 1800);
  trim.setServoPulseRange( 1, 1000, 2000);
  trim.setServoPulseRange( 2, 900, 2100);
  trim.setServoPulseRange( 3, 800, 1600);
  trim.setServoPulseRange( 4, 1500, 1500);
  trim.setServoPulseRange( 5, 500, 2500);

  // set up mappings - use this to remap servos
  trim.setServoNumber( 0, 5);
  trim.setServoNumber( 1, 4);
  trim.setServoNumber( 2, 3);
  trim.setServoNumber( 3, 2);
  trim.setServoNumber( 4, 1);
  trim.setServoNumber( 5, 0);

}

// main loop
void loop()
{
  // make servos ping-pong between 45 and 135 degrees
  for ( int i=0; i<NUM_SERVOS;i++)
  {
    SS_Angle angle = sr.getServoAngle(i); 

    if ( angle <= SS_DEGREES(45))
    { 
      sr.setServoTargetAngle(i, SS_DEGREES(135));
    } 
    else if ( angle >= SS_DEGREES(135))
    { 
      sr.setServoTargetAngle(i, SS_DEGREES(45));
    } 
  }
  // sr.service() needs to be called regularly so that
  // the servos are updated via update_servo()
  sr.service();
}


