/*
 * Pitt SOAR
 * This is the comprehensive code for controlling the drill via MATLAB and Arduino
 * Updated 2/25/2021
 */

/*THINGS THAT STILL NEED DONE:
 * Calibration of Load Cell
 * Ammeter library? Initialization, and Code
 * Linear Actuator Library, Initialization, and Code
 * Relay Code
 */
#include <BasicStepperDriver.h> //library for stepper motors + driver
#include<HX711.h> //Load cell library



//SERIAL COMMUNICATION VARIABLE
int sref=0,oldRef=0;



//DIGITAL CORE DEFINITIONS
HX711 forceSensor; //initializes force sensor object
#define dataPin 6
#define clockPin 7

float force = 0,dist = 0; 



//MOTOR CONTROL DEFINITIONS
// Define Motor steps per revolution. Ours is set with the switches on the motor controller
#define MOTOR_STEPS 6400
#define RPM 60 //desired speed

#define MICROSTEPS 1
  /*Since microstepping is set externally, make sure this matches the selected mode
  If it doesn't, the motor will move at a different RPM than chosen
  1=full step, 2=half step etc. */

// All the wires needed for full functionality; motor 1 (vertical) and motor2 (tool change)
#define step1 2
#define dir1 3
#define DIR2 4
#define STEP2 5

// 2-wire basic config, microstepping is hardwired on the driver
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
    pinMode(step1,OUTPUT);
    pinMode(dir1,OUTPUT);
    
    //stepper2.begin(RPM, MICROSTEPS);
    

    //This code initializes force sensor
    forceSensor.begin(dataPin, clockPin);
    forceSensor.set_scale(420.0983); // loadcell factor 5 KG; CALIBRATION IS STILL REQUIRED
    forceSensor.tare(); //zeroes load cell
    
    
    //Begin serial communication, for use with MATLAB
    Serial.begin(9600);
}

void loop() 
{
    oldRef = sref; //used to break loops if value changes
    sref = Serial.parseInt(); //which state are we in? 0 is read for no MATLAB inputs
      
    if(sref==1) drillDown(); //Drilling down
    
    else if(sref==2) retract(); //Pull out then stop
    
    else if(sref==3){//Heating Element
      
      } 
      
} //end loop function

void drillDown(void)
{
  digitalWrite(dir1, HIGH);
  
  while(1)
  {
    //one step
    digitalWrite(step1, HIGH);
    delayMicroseconds(400);
    digitalWrite(step1, LOW);
    
    sref = Serial.parseInt();
    //if(sref !=0 && sref!=1) break;
  } 
}

void retract(void)
{
  
}
