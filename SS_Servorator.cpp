//
//  
//  SS_Servorator.cpp
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

     SS_AngleRate           max_rate;

     SS_Angle               current_angle;
     SS_Angle               new_angle;

     SS_Time                last_update;
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
        s->max_rate = SS_NORMAL_RATE <= 1 ? 1 : SS_NORMAL_RATE ;
        s->current_angle = SS_NO_ANGLE;
        s->new_angle = SS_NO_ANGLE;
    }
    _handler = NULL;
    _handler_data = NULL;
    _service_interval = 20;
    _last_service_time = 0;

}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::setMaxRate( SS_AngleRate max )
{
    for ( SS_Index i=0; i<_num_servos; i++)
    {
        setServoMaxRate( i, max);
    }
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::setAngle( SS_Angle new_angle )
{
    for ( SS_Index i=0; i<_num_servos; i++)
    {
        setServoAngle( i, new_angle);
    }
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

void SS_Servorator::setServoAngle( SS_Index index, SS_Angle new_angle)
{
    SS_Servo *servo = get_servo(index);
    if ( servo )
    {
        servo->new_angle = new_angle;
    }
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::setServoMaxRate( SS_Index index, SS_AngleRate max )
{
    SS_Servo *servo = get_servo(index);
    if ( servo )
    {
        servo->max_rate = (max <= 0 ? 1 : max);
    }
}

//----------------------------------------
//
//----------------------------------------

void SS_Servorator::update_servo( SS_Servo *servo, SS_Time now )
{

    if ( servo->current_angle == servo->new_angle)
    {
        // nothing to do
        return;
    }

    if ( servo->new_angle < 0)
    {
        // no angle set
        servo->current_angle = servo->new_angle;
        servo->last_update = now;
        return;
    }

    if ( servo->current_angle < 0 )
    {
        // we don't have a current angle so just set the new angle
        servo->current_angle = servo->new_angle;
        servo->last_update = now;
        _handler( servo->index, servo->current_angle, _handler_data);
        return;
    }

    // we have an angle we need to move the servo to
    // however we need to move at the appropriate rate
    // for now we are just going to deal with the max rate

    SS_Time delta = now - servo->last_update;
    //SS_Time time = delta < servo->max_rate ? delta : servo->max_rate;
    
    SS_Angle increment = (delta*10)/servo->max_rate;
    
    SS_Angle diff = servo->new_angle - servo->current_angle;
        
    SS_Angle dir = diff < 0 ? -1 : 1;
    SS_Angle mag = diff < 0 ? -diff : diff;
        
    increment = increment > mag ? mag : increment ;

    if ( increment > 0 )
    {
        servo->current_angle += increment*dir;
        servo->last_update = now;
        _handler( servo->index, servo->current_angle, _handler_data);
    }
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


