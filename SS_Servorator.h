//
//  
//  SS_Servorator.h
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


#ifndef _SS_Servorator_H
#define _SS_Servorator_H

struct SS_Servo;     // private struct

typedef long SS_AngleRate;       // number of milliseconds per degree
typedef long SS_AccelRate;       // number of milliseconds per degree per millisecond
typedef long SS_Angle;           // in tenths of degrees
typedef int SS_Index;           // value in range 0..numServos()-1
typedef unsigned long SS_Time;  // time in ms since startup

// servo handler is a user provide function that handles updating servo positions
// <servo> is index of the servo. <new_angle> is the new position of the servo in tenths of degrees
// <data> is the point that was passed into SS_Servorator::setServoHandler()
typedef void SS_ServoHandler( SS_Index servo, SS_Angle new_angle, void *data);

// Rate is the time in ms it takes to move 1 degree 
#define SS_RATE(ANGLE,MS)    (((MS)*10L)/(ANGLE))     // angles are in tenths of degrees
#define SS_DEGREES(D)        ((long)((D)*10))

#define SS_FASTEST      1
#define SS_FAST_RATE    SS_RATE(SS_DEGREES(180),1000)
#define SS_NORMAL_RATE  SS_RATE(SS_DEGREES(180),3000)         // default rate - 180 degrees in 3 seconds
#define SS_SLOW_RATE    SS_RATE(SS_DEGREES(180),8000)

#define SS_NO_ANGLE     -1      

class SS_Servorator 
{

 public:

  // create servorator with num_servo instacnes
  SS_Servorator(int num_servos);

  void setServoHandler( SS_ServoHandler *handler, void *data); // handler to call to set PWM signal for individual servos
  void setUpdateInterval( SS_Time interval);    // default is 20ms (50hz)

  // global servo functions
  void setMaxRate( SS_AngleRate max );          // set max rate for all servos (in milliseconds per degree)
  void setAngle( SS_Angle new_angle);           // set new anlge for all servos (in tenths of degrees)

  // individual servo functions - servo must be between 0..numServos()-1
  void setServoAngle( SS_Index servo, SS_Angle new_angle);          // set new angle for servo at index
  void setServoMaxRate( SS_Index servo, SS_AngleRate max );         // set max rate for all servos (in milliseconds per degree)

  int numServos( void ) { return _num_servos;}

  void service( void ); // service and update all servos


 private:

  SS_Servo          *_servo;
  int               _num_servos;
  SS_ServoHandler   *_handler;
  void              *_handler_data;
  SS_Time           _last_service_time;
  SS_Time           _service_interval;

  SS_Servo          *get_servo(SS_Index index);
  void              update_servo( SS_Servo *servo, SS_Time now );

};

#endif

