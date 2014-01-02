//
//  
//  SS_Servorator.cpp
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

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <stdlib.h> // for malloc and free

#include "SS_Servorator.h"


struct SS_Servo
{
     SS_Index               index;

     SS_Velocity            max_velocity; // max veocity allowed

     SS_Velocity            velocity;       // current velocity vector (sign means direction)
     SS_Acceleration        acceleration;   // acceleration

     SS_Angle               current_angle;      // angle servo is currently positioned at
     SS_Angle               target_angle;       // angle we want the servo to reach

     SS_Time                last_update;        // time of last update
     SS_Time                a_time;             // accelration time
     SS_Time                s_time;             // sustain velocity time
     SS_Time                d_time;             // deacceleration time
     SS_Time                b_time;             // time to break (to change direction)

};

//----------------------------------------
//
//----------------------------------------

SS_Servorator::SS_Servorator(int count)
{
    // create servo array
    _servo = (SS_Servo*)malloc(sizeof(SS_Servo)*count);
    _num_servos = count;

    // init servo data
    SS_Servo *s = _servo;
    for ( int i=0; i<_num_servos;i++,s++)
    {
        s->index = i;
        s->max_velocity = SS_NORMAL_RATE;
        s->acceleration = 0;
        s->current_angle = SS_NO_ANGLE;
        s->target_angle = SS_NO_ANGLE;
        s->last_update = 0;
        stopped(s);
    }
    _handler = NULL;
    _handler_data = NULL;
    _service_interval = 20;
    _last_service_time = 0;

}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::setServoHandler( SS_ServoHandler *handler, void *data)
{
    _handler = handler;
    _handler_data = data;
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::setServoTargetAngle( SS_Index index, SS_Angle new_angle)
{
    if ( index == SS_ALL)
    {
        for (int i=0;i<_num_servos;i++ )
        {
            setServoTargetAngle( i, new_angle);
        }

        return;
    }

    SS_Servo *servo = get_servo(index);
    if ( servo && servo->target_angle != new_angle )
    {
        servo->target_angle = new_angle;
        update_accel( servo );
    }
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::setServoMaxVelocity( SS_Index index, SS_Velocity new_max )
{
    if ( new_max < 0)
    {
        return;
    }

    if (index == SS_ALL) 
    {
        for (int i=0;i<_num_servos;i++ )
        {
            setServoMaxVelocity( i, new_max);
        }

        return;
    }

    SS_Servo *servo = get_servo(index);
    if ( servo && servo->max_velocity != new_max)
    {
        servo->max_velocity = new_max;
        update_accel( servo);
    }
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::setServoAcceleration( SS_Index index, SS_Acceleration new_acc )
{
    if ( index == SS_ALL)
    {
        for (int i=0;i<_num_servos;i++ )
        {
            setServoAcceleration( i, new_acc);
        }

        return;
    }

    SS_Servo *servo = get_servo(index); 
    if ( servo && servo->acceleration != new_acc)
    {
        servo->acceleration = new_acc;
        update_accel( servo);
    }
}

//----------------------------------------
//
//----------------------------------------

SS_Angle SS_Servorator::getServoAngle( SS_Index index)
{
    SS_Servo *servo = get_servo(index);
    if ( servo )
    {
        return servo->current_angle;
    }

    return SS_NO_ANGLE;
}

//----------------------------------------
//
//----------------------------------------

SS_Angle SS_Servorator::getServoTargetAngle( SS_Index index)
{
    SS_Servo *servo = get_servo(index);
    if ( servo )
    {
        return servo->target_angle;
    }

    return SS_NO_ANGLE;
}

//----------------------------------------
//
//----------------------------------------

SS_Velocity SS_Servorator::getServoMaxVelocity( SS_Index index )
{
    SS_Servo *servo = get_servo(index);
    if ( servo )
    {
        return servo->max_velocity;
    }

    return 0;
}

//----------------------------------------
//
//----------------------------------------

SS_Velocity SS_Servorator::getServoVelocity( SS_Index index )
{
    SS_Servo *servo = get_servo(index);
    if ( servo )
    {
        return servo->velocity;
    }

    return 0;
}

//----------------------------------------
//
//----------------------------------------

SS_Acceleration SS_Servorator::getServoAcceleration( SS_Index index )
{
    SS_Servo *servo = get_servo(index);
    if ( servo )
    {
        return servo->acceleration;
    }

    return 0;
}

//----------------------------------------
//
//----------------------------------------

SS_Time SS_Servorator::getServoTimeTillTarget( SS_Index index )
{
    SS_Servo *servo = get_servo(index);
    if ( servo )
    {
        return time_till_target( servo );
    }

    return 0;
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::update_servo( SS_Servo *servo, SS_Time now )
{

    if ( servo->current_angle == servo->target_angle)
    {
        // nothing to do
        return;
    }

    if ( servo->target_angle < 0)
    {
        // no angle set
        servo->current_angle = servo->target_angle;
        servo->last_update = now;
        stopped(servo);
        return;
    }

    if ( servo->current_angle < 0 )
    {
        // we don't have a current angle so go directly to the target position
        servo->current_angle = servo->target_angle;
        servo->last_update = now;
        stopped(servo);
        _handler( servo->index, servo->current_angle, _handler_data);
        return;
    }

    // we have an angle we need to move the servo to
    // however we need to move at the appropriate rate

    signed long delta = (signed long) (now - servo->last_update);
    
    SS_Angle increment = (servo->velocity*delta)/1000L;
    //Serial.println(increment);
    if ( increment != 0 )
    {
        servo->current_angle += increment;
        servo->last_update = now;

        // clip the change
        if ( increment > 0 )
        {
            servo->current_angle = servo->current_angle > servo->target_angle ? servo->target_angle : servo->current_angle; 
        }
        else
        {
            servo->current_angle = servo->current_angle < servo->target_angle ? servo->target_angle : servo->current_angle; 
        }

        if ( servo->current_angle == servo->target_angle)
        {
            stopped(servo);
        }
        _handler( servo->index, servo->current_angle, _handler_data);
    }

    if ( servo->s_time )
    {
        servo->s_time -= delta > servo->s_time ? servo->s_time : delta;
    }
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::update_accel( SS_Servo *servo  )
{
    //Serial.println("update_accel");
    SS_Angle diff = servo->target_angle - servo->current_angle;

    if ( diff == 0 || servo->max_velocity == 0)
    {
        // trivial case
        stopped( servo );
        return;
    }

    long dir = diff < 0 ? -1 : 1;
    SS_Angle mag = diff < 0 ? -diff : diff;

    if ( servo->acceleration == 0)
    {
        // trivial case
        //Serial.println("accel = 0");
        servo->velocity = servo->max_velocity*dir;
        servo->a_time = servo->d_time = servo->b_time = 0;
        servo->s_time = servo->velocity != 0 ? (mag*1000L)/(servo->velocity < 0 ? -servo->velocity : servo->velocity) : 0;
        //Serial.print("velocity = ");
        //Serial.println(servo->velocity);
        //Serial.print("time = ");
        //Serial.println(servo->s_time);
        return;
    }

    #if 0
    SS_

    if ( servo->velocity)
    {
        // we are already in motion so we need to see if we can 
    }
    SS_Angle a_move;
    SS_Angle b_move;

    servo->a_time = calc_deltas( servo->max_velocity - servo->velocity, servo->acceleration, &a_move );
    servo->d_time = calc_deltas( servo->max_velocity, servo->acceleration, &d_move );

    if (a_move + b_move <= mag )
    {
        // trival case
        servo->s_time = ((mag-a_move-d_move)*1000L)/servo->max_velocity;
        return;
    }

    // not enough space to perform full accel/deaccel

    if (servo->velocity == 0) 
    {
        // just split the remaing space between accel and deaccel
        servo->a_time = servo->b_time = calc_accel_time( 0, servo->acceleration, mag/2);
        servo->deacceleration = servo->acceleration;
        servo->s_time = 0;
        return;

    }

    if ( servo->velocity == servo->max_velocity )
    {
        // just deaccel
        servo->deacceleration = calc_accel( servo->velocity, 0, mag, &servo->d_time);

    }
    // calc the movement needed to perform normal deaccel from current velocity
    // this is the hard one....
    #endif

}

//----------------------------------------
//
//----------------------------------------

SS_Angle SS_Servorator::time_till_target( SS_Servo *servo  )
{
    return servo->a_time + servo->s_time + servo->d_time + servo->b_time;
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::stopped( SS_Servo *servo  )
{
    servo->velocity = 0;
    servo->a_time = servo->b_time = servo->s_time = servo->d_time = 0;
}


//----------------------------------------
//
//----------------------------------------

void SS_Servorator::service( void )
{
    SS_Servo *s = _servo;
    SS_Time now = millis();
    SS_Time delta = now - _last_service_time;

    if ( delta >= _service_interval )
    {
        for (int i=0; i<_num_servos; i++)
        {
            update_servo( s++, now );
        }

        _last_service_time = now;
    }
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::setUpdateInterval( SS_Time interval)
{
    _service_interval = interval > 1 ? interval : 1;
}

//----------------------------------------
//
//----------------------------------------

SS_Servo *SS_Servorator::get_servo( SS_Index index)
{
    if ( index >= 0 && index < _num_servos && _servo )
    {
        return _servo+index;
    }

    return NULL;
}


