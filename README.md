#SS_Servorator

An Arduino library that provides simple speed control for servos.

##How To Use

In your sketch you need to include the library so:


	#import <SS_Servorator.h>

Create an instance of SS_Servorator and pass in the number of servos it to control:

	#define NUM_SERVOS  6

	SS_Servorator sr(NUM_SERVOS);

###Set Up

You can create multiple instances if you needed to manage groups of servos.

For each instance you create you need to provided a servo handling routine of type **SS_ServoHandler**:

	// a servo handler for Servorator
	void update_servo( SS_Index index, SS_Angle angle, void *data)
	{
  		// SS_Angle is in 1000ths of a degree
  		servo[index].write( angle/1000);
	}

The routine is responsible for making the appropriate calls to whatever servo driver interface you are using. In the code above we are calling the Arduino's Servo library write() function. 

**SS_Index** is a servo index, a value between 0 and *NUM_SERVOS-1*. **SS_Angle** is in 1000ths of degrees and has the range **0 to 180000** (180 degrees). 

You can get the number of servos using **numServos()**:

	int num = sr.numServos();

The servo handler routine needs to be registered with the Servorator instance using **setServoHandler()**:

	void setup()
	{
  		:
  		// register servo handler
  		// void setServoHandler( SS_ServoHandler *handler, void *user_data)
  		sr.setServoHandler( update_servo, NULL);
  		:	
	}

The second parameter is a pointer to void. Whatever pointer you pass will in turn be passed through to the update routine in the 'data' param shown above. You can use that param to associate data with the handler.

For the handler to get called you need to regularly call Servorator's **service()** function:

	// main loop
	void loop()
	{
  		:
  		sr.service();
  		:
	}
	
If service() is not called frequently servo movement will become erratic.

###Control

To move a servo to a new postion use **setServoTargetAngle()**:

	// void setServoTargetAngle( SS_Index servo, SS_Angle new_angle )
    sr.setServoTargetAngle( 0, SS_DEGREES(175));

The **SS_DEGREES()** macro helps make code a little more readable. You can also type SS_DEGREES(175.5) but keep in mind that Servorator is only accurate to a 1000th of a degree so SS_DEGREES(175.572987) is identical to SS_DEGREES(175.572).

The max speed at which the servos turn is set using **setServoMaxVelocity()**:

	// void setServoMaxVelocity( SS_Index servo, SS_Velocity vel )
    sr.setServoMaxVelocity( 0, SS_DEGREES(60));

**SS_Velocity** is in 1000ths of degrees per second.

###Query Status

You can read the current and target servo angles using **getServoAngle** and **getServoTargetAngle()**:

	// SS_Angle getServoAngle( SS_Index servo )
    SS_Angle current = sr.getServoAngle( 0 );
    
	// SS_Angle getServoTargetAngle( SS_Index servo )
    SS_Angle target = sr.getServoTargetAngle( 0 );

'target' is the angle set in the last call to setServoTargetAngle(). 'current' is the angle the servo is currently at.

##Example

SimpleServo.ino example:

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
      servo[index].write( angle/1000);
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




