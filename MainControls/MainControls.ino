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
#include<BasicStepperDriver.h> //Stepper driver library (only for tool changer)


//SERIAL COMMUNICATION VARIABLES
int sref=0;



//DIGITAL CORE DEFINITIONS
HX711 forceSensor; //initializes force sensor object
#define dataPin 51
#define clockPin 53

float force = 0,dist = 0; 



//MOTOR CONTROL DEFINITIONS
// Define Motor steps per revolution. Ours is set with the switches on the motor controller
#define MOTOR_STEPS 800
#define RPM 120 //desired speed
#define MICROSTEPS 1
  /*Since microstepping is set externally, make sure this matches the selected mode
  If it doesn't, the motor will move at a different RPM than chosen
  1=full step, 2=half step etc. */
  
double stepDelay = 60.0/(long(RPM)*long(MOTOR_STEPS)*long(MICROSTEPS))*1000000;; //speed of delay used to control vertical speed (microseconds)

// All the wires needed for full functionality; motor 1 (vertical) and motor2 (tool change)
#define stepPin 2
#define dirPin 3
#define stepPin2 4
#define dirPin2 5

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper2(MOTOR_STEPS, dirPin2, stepPin2);



//RELAY DEFINITIONS
#define DRILL  8
#define PUMP  10
#define PROBE  9
#define VALVE1  11
#define VALVE2  12
#define VALVE3  13



//AMMETER DEFINITIONS




/////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    //initialize stepper motors
    pinMode(stepPin,OUTPUT);
    pinMode(dirPin,OUTPUT);

    //initializes relays
    pinMode(DRILL,OUTPUT);
    pinMode(PROBE,OUTPUT);
    
    
    stepper2.begin(RPM, MICROSTEPS);
    

    //This code initializes force sensor
    forceSensor.begin(dataPin, clockPin);
    forceSensor.set_scale(420.0983); // loadcell factor 5 KG; CALIBRATION IS STILL REQUIRED
    forceSensor.tare(); //zeroes load cell
    
    
    //Begin serial communication, for use with MATLAB
    Serial.begin(9600);
}//end setup

/////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{   
    if(Serial.available())
      sref = Serial.parseInt(); //which state are we in? 0 is read for no MATLAB inputs
      
    if(sref==1) drillDown(); //Drilling down
    
    else if(sref==2) retract(); //Pull out then stop
    
    else if(sref==3){//Heating Element
      heater();
      }   
} //end main loop function

/////////////////////////////////////////////////////////////////////////////////////////
void drillDown(void)
{  
  digitalWrite(dirPin, HIGH);//High for descent
  digitalWrite(DRILL,HIGH);//turns on relays

  int count=0; //step count
  int stepDown = 0; //multiplier to prevent going over 30,000
  int stepDown2 = 0; //second multiplier
  
  double distance = 0;
  while(sref==1||sref==0)
  {
    //one step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    //digital core
    count = count+1;

    if(count%(MOTOR_STEPS/8)==0)//Output data every 1/8 turn
    {
      force = forceSensor.get_units(1); //averages 1 readings for output
      
      Serial.print(force);
      Serial.print(" ");
      Serial.print(count);
      Serial.println(" ");
    }
    
    if(Serial.available())
      sref = Serial.parseInt();
      
  } 
}

/////////////////////////////////////////////////////////////////////////////////////////
void retract(void)
{
  digitalWrite(dirPin, LOW);
  
  while(sref==2||sref==0)
  {
    //one step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
    if(Serial.available())
      sref = Serial.parseInt();
      
  }  
}

/////////////////////////////////////////////////////////////////////////////////////////
void heater(void)
{
digitalWrite(DRILL,LOW);
digitalWrite(PROBE,HIGH);  
}
