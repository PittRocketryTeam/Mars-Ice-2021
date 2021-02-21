/*
 * Copyright (C)2015-2017 Laurentiu Badea
 * This file may be redistributed under the terms of the MIT license.
 * A copy of this license has been included with this distribution in the file LICENSE.
 */

#include <BasicStepperDriver.h>

// Define Motor steps per revolution. Ours is set with the switches on the motor controller
#define MOTOR_STEPS 6400
#define RPM 60 //desired speed

// Since microstepping is set externally, make sure this matches the selected mode
// If it doesn't, the motor will move at a different RPM than chosen
// 1=full step, 2=half step etc.
#define MICROSTEPS 1

// All the wires needed for full functionality
#define DIR 8
#define STEP 9

//#define SLEEP 13 //Uncomment line to use enable/disable functionality

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);

//BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, SLEEP); //Uncomment line to use enable/disable functionality

void setup() {
    stepper.begin(RPM, MICROSTEPS);
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    // stepper.setEnableActiveState(LOW);
}

void loop() {
    // Moving motor one full revolution using the degree notation
    stepper.rotate(360);

    //Moving motor to original position using steps
    stepper.move(-MOTOR_STEPS*MICROSTEPS);

    delay(2500);

//Additional methods \/
    // pause and allow the motor to be moved by hand
    // stepper.disable();

    // energize coils - the motor will hold position
    // stepper.enable();
}
