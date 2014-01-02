//
//
// SS_Servorator.h
//
// Copyright (c) 2013-2014, Solder Spot
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or
//   other materials provided with the distribution.
// 
// * Neither the name of the copyright holders nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef _SS_Servorator_H
#define _SS_Servorator_H

#define SS_ServoratorVersion        010100          // V1.1.0

struct SS_Servo;     // private struct

// in Servorator all angles are in 1000th of degrees.

typedef long SS_Velocity;       // in 1000th of degrees per second
typedef long SS_Acceleration;   // in 1000th of degrees per second per second
typedef long SS_Angle;          // in 1000th of degrees
typedef int SS_Index;           // value in range 0..numServos()-1
typedef unsigned long SS_Time;  // time in ms since startup

// servo handler is a user provide function that handles updating servo positions
// <servo> is index of the servo. <new_angle> is the new position of the servo in 1000th of degrees
// <data> is the point that was passed into SS_Servorator::setServoHandler()
typedef void SS_ServoHandler( SS_Index servo, SS_Angle new_angle, void *data);


// useful macros
#ifndef SS_DEGRESS
#define SS_DEGREES(D)        ((long)(((long)(D))*1000L))
#endif

#define SS_FAST_RATE    SS_DEGREES(180)             // 180 degrees per second
#define SS_NORMAL_RATE  SS_DEGREES(60)              // default rate - 60 dps
#define SS_SLOW_RATE    SS_DEGREES(15)              // 15 dps

#define SS_FAST_ACCEL   SS_DEGREES(180)             // 180 dpsps
#define SS_NORMAL_ACCEL SS_DEGREES(60)              // 18 dpsps
#define SS_SLOW_ACCEL   SS_DEGREES(15)              // 15 dpsps

#define SS_NO_ANGLE     -1      
#ifndef SS_ALL
#define SS_ALL          -1
#endif

class SS_Servorator 
{

 public:

  // create servorator with num_servo instacnes
  SS_Servorator(int num_servos);

  void setServoHandler( SS_ServoHandler *handler, void *data); // handler to call to set PWM signal for individual servos
  void setUpdateInterval( SS_Time interval);    // default is 20ms (50hz)

  // individual servo functions - servo index must be between 0..numServos()-1
  // setters
  void setServoTargetAngle( SS_Index servo, SS_Angle new_angle);        // set new target angle for servo
  void setServoMaxVelocity ( SS_Index servo, SS_Velocity vel );         // set new max velocity for servo
  void setServoAcceleration( SS_Index servo, SS_Acceleration acc );     // set new acceleration for servo

  // getters
  SS_Angle          getServoTargetAngle( SS_Index servo);               // get servo's current target angle
  SS_Velocity       getServoMaxVelocity( SS_Index servo );              // get servo's current max velocity
  SS_Velocity       getServoVelocity( SS_Index servo );                 // get servo's current velocity
  SS_Angle          getServoAngle( SS_Index servo);                     // get servo's current angle 
  SS_Acceleration   getServoAcceleration( SS_Index servo );             // get servo's current acceleration
  SS_Time           getServoTimeTillTarget( SS_Index servo);            // returns the estimated time till target angle is reached

  // other
  int numServos( void ) { return _num_servos;}
  void service( void );                                                 // service and update all servos as needed


 private:

  SS_Servo          *_servo;
  int               _num_servos;
  SS_ServoHandler   *_handler;
  void              *_handler_data;
  SS_Time           _last_service_time;
  SS_Time           _service_interval;

  SS_Servo          *get_servo(SS_Index index);
  void              update_servo( SS_Servo *servo, SS_Time now );
  void              update_accel( SS_Servo *servo );
  void              stopped( SS_Servo *servo );
  SS_Angle          time_till_target( SS_Servo *servo );

};

#endif

