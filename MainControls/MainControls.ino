/*
 * Pitt SOAR
 * This is the comprehensive code for controlling the drill via MATLAB and Arduino
 * Updated 2/6/2021
 */

/*THINGS THAT STILL NEED DONE:
 * Calibration of Load Cell
 * Ammeter library? Initialization, and Code
 * Serial Communication with matlab
 * Linear Actuator Library, Initialization, and Code
 * Relay Code
 */

#include <BasicStepperDriver.h> //library for stepper motors + driver
#include<HX711.h> //Load cell library



//Serial Communication Variable
int SREF = 0;


//DIGITAL CORE DEFINITIONS
HX711 forceSensor; //initializes force sensor object
#define dataPin 3
#define clockPin 4

float force = 0,dist = 0; 



//MOTOR CONTROL DEFINITIONS
//Because our motors are identical, we use the following variables for both

// Define Motor steps per revolution. Ours is set with the switches on the motor controller
#define MOTOR_STEPS 6400
#define RPM 60 //desired speed

#define MICROSTEPS 1
  /*Since microstepping is set externally, make sure this matches the selected mode
  If it doesn't, the motor will move at a different RPM than chosen
  1=full step, 2=half step etc. */

// All the wires needed for full functionality; motor 1 (vertical) and motor2 (tool change)
#define DIR1 2
#define STEP1 3
#define DIR2 4
#define STEP2 5

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper1(MOTOR_STEPS, DIR1, STEP1);
BasicStepperDriver stepper2(MOTOR_STEPS, DIR2, STEP2);



//RELAY DEFINITIONS
#define DRILL = 8
#define PUMP = 9
#define PROBE = 10
#define VALVE1 = 11
#define VALVE2 = 12
#define VALVE3 = 13



//AMMETER DEFINITIONS





void setup() {
    //initialize stepper motors
    stepper1.begin(RPM, MICROSTEPS);
    stepper2.begin(RPM, MICROSTEPS);
    

    //This code initializes force sensor
    forceSensor.begin(dataPin, clockPin);
    forceSensor.set_scale(420.0983); // loadcell factor 5 KG; CALIBRATION IS STILL REQUIRED
    forceSensor.tare(); //zeroes load cell
    
    
    //Begin serial communication, for use with MATLAB
    Serial.begin(9600);
}

void loop() {

   
    if(SREF==0){ //Initialization state; everything is stopped (Coded E-STOP)
      
      }
    else if(SREF==1){ //Drilling
      digitalWrite (DIR1, HIGH);//change from low to high if we need to change direction of motor
      digitalWrite (DIR2, LOW);
      analogWrite (DRILL, 255);
       if (force>125){
      digitalWrite (DIR1, HIGH);//change from low to high if we need to change direction of motor
      digitalWrite (DIR2, LOW);
      analogWrite (DRILL, 180);//changed speed of the motor
    }
      
      }
    else if(SREF==2){//Heat Probe
      
      }
    else if(SREF==3){//Pump
      
      }
    else if(SREF==4){//Valves??
      
      }
      
}
