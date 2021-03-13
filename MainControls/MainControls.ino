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
#include "EmonLib.h"

//AMMETER VARIABLES
EnergyMonitor emon1;                   // Create an instance for ammeter
double Irms;

//SERIAL COMMUNICATION VARIABLES
int sref=0;



//DIGITAL CORE DEFINITIONS
HX711 forceSensor; //initializes force sensor object
#define dataPin 51
#define clockPin 53

float force = 0,dist = 0; 



//MOTOR CONTROL DEFINITIONS
// Define Motor steps per revolution. Ours is set with the switches on the motor controller
#define MOTOR_STEPS1 800
#define MOTOR_STEPS2 3200
#define RPM1 60 //desired speed
#define RPM2 30
#define MICROSTEPS 1
  /*Since microstepping is set externally, make sure this matches the selected mode
  If it doesn't, the motor will move at a different RPM than chosen
  1=full step, 2=half step etc. */
  
double stepDelay = 60.0/(long(RPM1)*long(MOTOR_STEPS1)*long(MICROSTEPS)*2)*1000000; //speed of delay used to control vertical speed (microseconds)
long distance=0; //step count

// All the wires needed for full functionality; motor 1 (vertical) and motor2 (tool change)
#define stepPin 2
#define dirPin 3
#define stepPin2 4
#define dirPin2 5

// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper2(MOTOR_STEPS2, dirPin2, stepPin2);



//RELAY DEFINITIONS
#define DRILL  8
#define PROBE  9
#define PUMP  10

#define VALVE1  11
#define VALVE2  12
#define VALVE3  13



/////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    //initialize stepper motors
    pinMode(stepPin,OUTPUT);
    pinMode(dirPin,OUTPUT);

    //initializes relays
    pinMode(DRILL,OUTPUT);
    pinMode(PROBE,OUTPUT);
    
    
    stepper2.begin(RPM2, MICROSTEPS);
    

    //This code initializes force sensor
    forceSensor.begin(dataPin, clockPin);
    forceSensor.set_scale(420.0983); // loadcell factor 5 KG; CALIBRATION IS STILL REQUIRED
    forceSensor.tare(); //zeroes load cell
    
    
    //Begin serial communication, for use with MATLAB
    Serial.begin(9600);
    emon1.current(1, 111.1); //for ammeter
}//end setup


/////////////////////////////////////////////////////////////////////////////////////////
void loop() 
{   
    if(Serial.available())
      sref = Serial.parseInt(); //which state are we in? 0 is read for no MATLAB inputs
      
    if(sref==1) drillDown(); //Drilling down
    
    else if(sref==2) retract(); //Pull out then stop

    else if(sref==3) tool1(); 
      
    else if(sref==4) tool2();

    else if(sref==5) tool3(); 

    else if(sref==6) tool4(); 
    
    else if(sref==7) heater(); //Heating Element

    else if(sref==8) pump();
      
} //end main loop function

/////////////////////////////////////////////////////////////////////////////////////////
void drillDown(void)
{  
  digitalWrite(PUMP,LOW);
  digitalWrite(PROBE,LOW);
  digitalWrite(DRILL,HIGH);//turns on relays

  delay(1000); //wait 1sec before starting drill
  
  digitalWrite(dirPin, HIGH);//High for descent
  
  distance = 0;
  while(sref==1||sref==0)
  {
    //one step
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    //digital core
    distance = distance+1;
    
    if(int(distance)%8==0)//Output data every 1/8 turn
    {
      Irms = emon1.calcIrms(1480);  // Calculate Irms only
      force = forceSensor.get_units(1); //averages 1 readings for output
      
      Serial.print(distance);
      Serial.print(" ");
      Serial.print(force);
      Serial.print(" ");
      Serial.print(Irms);
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
  
  while((sref==2||sref==0)&&distance>0) //reverses count to end at starting position (if not stopped externally)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

     if(int(distance)/8==0)//Output data every 1/8 turn
    {
      Irms = emon1.calcIrms(1480);  // Calculate Irms only
      Serial.print(Irms);
      Serial.println(" ");
    }
    
    if(Serial.available())
      sref = Serial.parseInt();
    distance = distance-1;
  }  
}

/////////////////////////////////////////////////////////////////////////////////////////
void heater(void)
{
digitalWrite(DRILL,LOW);
digitalWrite(PUMP, LOW);
digitalWrite(PROBE,HIGH);  
}

void pump(void)
{
  digitalWrite(DRILL,LOW);
  digitalWrite(PUMP, HIGH);
  digitalWrite(PROBE,LOW);  
}

/////////////////////////////////////////////////////////////////////////////////////////
void tool1(void)
{
  stepper2.move(MOTOR_STEPS2/8);
  delay(1000);
  stepper2.move(-MOTOR_STEPS2/6);
}
/////////////////////////////////////////////////////////////////////////////////////////
void tool2(void)
{
  stepper2.move(MOTOR_STEPS2/12);
  delay(1000);
  stepper2.move(-MOTOR_STEPS2/12);
}

/////////////////////////////////////////////////////////////////////////////////////////
void tool3(void)
{
  stepper2.move(-MOTOR_STEPS2/6);
  delay(1000);
  stepper2.move(MOTOR_STEPS2/6);
}

/////////////////////////////////////////////////////////////////////////////////////////
void tool4(void)
{
  stepper2.move(-MOTOR_STEPS2/12);
  delay(1000);
  stepper2.move(MOTOR_STEPS2/12);
}
