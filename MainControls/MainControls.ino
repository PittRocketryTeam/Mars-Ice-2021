/*
 * Pitt SOAR
 * This is the comprehensive code for controlling the drill via MATLAB and Arduino
 * Updated 2/28/2021
 */

/*THINGS THAT STILL NEED DONE:
 * Calibration of Load Cell
 * Ammeter library? Initialization, and Code
 * Linear Actuator Library, Initialization, and Code
 * Relay Code
 */
#include<HX711.h> //Load cell library



//SERIAL COMMUNICATION VARIABLE
int sref=0;



//DIGITAL CORE DEFINITIONS
HX711 forceSensor; //initializes force sensor object
#define dataPin 6
#define clockPin 7

float force = 0,dist = 0; 



//MOTOR CONTROL DEFINITIONS
// Define Motor steps per revolution. Ours is set with the switches on the motor controller
#define MOTOR_STEPS 800
#define RPM 60 //desired speed

#define MICROSTEPS 1
  /*Since microstepping is set externally, make sure this matches the selected mode
  If it doesn't, the motor will move at a different RPM than chosen
  1=full step, 2=half step etc. */

// All the wires needed for full functionality; motor 1 (vertical) and motor2 (tool change)
#define stepPin 2
#define dirPin 3
#define DIR2 4
#define STEP2 5

// 2-wire basic config, microstepping is hardwired on the driver
//BasicStepperDriver stepper2(MOTOR_STEPS, DIR2, STEP2);



//RELAY DEFINITIONS
#define DRILL  8
#define PUMP  10
#define PROBE  9
#define VALVE1  11
#define VALVE2  12
#define VALVE3  13
#define vRef  7



//AMMETER DEFINITIONS





void setup() {
    //initialize stepper motors
    pinMode(stepPin,OUTPUT);
    pinMode(dirPin,OUTPUT);

    //initializes relays
    pinMode(DRILL,OUTPUT);
    pinMode(vRef,OUTPUT);
    pinMode(PROBE,OUTPUT);
    
    
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
    //used to break loops if value changes
    
    sref = Serial.parseInt(); //which state are we in? 0 is read for no MATLAB inputs
      
    if(sref==1) drillDown(); //Drilling down
    
    else if(sref==2) retract(); //Pull out then stop
    
    else if(sref==3){//Heating Element
      heater();
      } 
      
} //end loop function

void drillDown(void)
{
  digitalWrite(dirPin, HIGH);
  
  digitalWrite(DRILL,HIGH);
  
  while(sref==0 || sref==1)
  {
    //one step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
    if(Serial.available())
      sref = Serial.parseInt();
      
  } 
}

void retract(void)
{
  digitalWrite(dirPin, LOW);
  stepCount=0;
  while(sref==0 || sref==2)
  {
    //one step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500);
    if(Serial.available())
      sref = Serial.parseInt();
      
  }  
}

void heater(void)
{
digitalWrite(DRILL,LOW);
digitalWrite(PROBE,HIGH);  
}
