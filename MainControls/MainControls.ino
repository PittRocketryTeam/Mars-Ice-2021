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
#define MOTOR_STEPS1 800
#define MOTOR_STEPS2 3200
#define RPM1 120 //desired speed
#define RPM2 30
#define MICROSTEPS 1
  /*Since microstepping is set externally, make sure this matches the selected mode
  If it doesn't, the motor will move at a different RPM than chosen
  1=full step, 2=half step etc. */
  
double stepDelay = 60.0/(long(RPM1)*long(MOTOR_STEPS1)*long(MICROSTEPS))*1000000;; //speed of delay used to control vertical speed (microseconds)
int count=0; //step count

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



//AMMETER DEFINITIONS




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
      
} //end main loop function

/////////////////////////////////////////////////////////////////////////////////////////
void drillDown(void)
{  
  digitalWrite(PUMP,LOW);
  digitalWrite(PROBE.LOW);
  digitalWrite(DRILL,HIGH);//turns on relays

  digitalWrite(dirPin, HIGH);//High for descent
  
  count = 0;
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
    
    if(count%(MOTOR_STEPS1/8)==0)//Output data every 1/8 turn
    {
      force = forceSensor.get_units(1); //averages 1 readings for output
      
      Serial.print(count);
      Serial.print(" ");
      Serial.print(force);
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
  
  while((sref==2||sref==0)&&count>0) //reverses count to end at starting position (if not stopped externally)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
    if(Serial.available())
      sref = Serial.parseInt();
    count = count-1;
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
