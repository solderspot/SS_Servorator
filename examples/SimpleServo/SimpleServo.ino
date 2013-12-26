//
//  
//  SimpleServo.ino
//
//  Copyright (c) 2013 Solder Spot. All rights reserved.
//
//  Software License Agreement (BSD License)
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//  1. Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//  3. Neither the name of the copyright holders nor the
//  names of its contributors may be used to endorse or promote products
//  derived from this software without specific prior written permission.
//  
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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
  // SS_Angle is in tenths of a degree
  //Serial.println(angle/10);
  servo[index].write( angle/10);
}

void setup() 
{
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
  SS_AngleRate rate = SS_FAST_RATE;
  for ( int i=0; i<NUM_SERVOS;i++)
  {
    sr.setServoAngle(i, SS_DEGREES(45)); 
    sr.setServoMaxRate( i, rate );
    rate = rate * 2 ;
  }

}

// main loop
void loop()
{
  // make servos ping-pong between 45 and 135 degrees
  for ( int i=0; i<NUM_SERVOS;i++)
  {
    SS_Angle angle = sr.getServoCurrentAngle(i); 

    if ( angle <= SS_DEGREES(45))
    { 
      sr.setServoAngle(i, SS_DEGREES(135));
    } 
    else if ( angle >= SS_DEGREES(135))
    { 
      sr.setServoAngle(i, SS_DEGREES(45));
    } 
  }
  // sr.service() needs to be called regularly so that
  // the servos are updated via update_servo()
  sr.service();
}

