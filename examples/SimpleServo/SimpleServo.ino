//
//  
//  SimpleServo.ino
//
// Copyright (c) 2013, Solder Spot
// All rights reserved. 

// This sketch requires the following library:
//   SS_Servorator: https://github.com/solderspot/SS_Servorator

// include needed librarys
#include <Servo.h>
#include <SS_Servorator.h>


#define NUM_SERVOS 6

Servo servo[NUM_SERVOS];
SS_Servorator sr(NUM_SERVOS);

// the servo handler for Servorator
void update_servo( SS_Index index, SS_Angle angle, void *data)
{
  // SS_Angle is in 1000th of a degree
  //Serial.println(angle/1000);
  servo[index].write( angle/1000);
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

