//
//  
//  SimpleServoTrim.ino
//
// Copyright (c) 2013-2014, Solder Spot
// All rights reserved. 

// This sketch requires the following libraries:
//   SS_Servorator: https://github.com/solderspot/SS_Servorator
//   SS_ServoTrim: https://github.com/solderspot/SS_ServoTrim

// include needed librarys
#include <Servo.h>
#include <SS_Servorator.h>
#include <SS_ServoTrim.h>


#define NUM_SERVOS 6

Servo servo[NUM_SERVOS];
SS_Servorator sr(NUM_SERVOS);
SS_ServoTrim trim(NUM_SERVOS);

// the servo handler for Servorator
void update_servo( SS_Index index, SS_Angle angle, void *data)
{
  // SS_Angle is in 1000th of a degree

  long servo_index = trim.getServoNumber( index );
  long time = trim.getServoPulseTime(index, angle);
  servo[servo_index].writeMicroseconds( time );

  // delete or comment out following lines out to remove
  // serial output 
  {
      Serial.print("servo[");
      Serial.print(servo_index);
      Serial.print("] ");
      Serial.print(angle/1000);
      Serial.print(" is ");
      Serial.print(time);
      Serial.println(" us");
  }
}

void setup() 
{

  Serial.begin(9600);
  // assign PWM pins to servos
  servo[0].attach(3);
  servo[1].attach(5);
  servo[2].attach(6);
  servo[3].attach(9);
  servo[4].attach(10);
  servo[5].attach(11);
  
  // register servo handler
  sr.setServoHandler( update_servo, NULL);

  // set all servos at 45 degrees
  // servo 0 is fastest
  SS_Velocity vel = SS_FAST_RATE;
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

