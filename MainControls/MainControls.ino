/*
* Pitt SOAR
* This is the comprehensive code for controlling the drill via MATLAB and Arduino
* Updated 03/13/2021
*/

 

/*THINGS THAT STILL NEED DONE:
* Calibration of Load Cell
* Linear Actuator Library, Initialization, and Code
*/

#include<HX711.h> //Load cell library
#include<BasicStepperDriver.h> //Stepper driver library (only for tool changer)
#include "EmonLib.h"

#define setPin(b) ( (b)<8 ? PORTD |=(1<<(b)) : PORTB |=(1<<(b-8)) )
#define clrPin(b) ( (b)<8 ? PORTD &=~(1<<(b)) : PORTB &=~(1<<(b-8)) )

//AMMETER VARIABLES
EnergyMonitor emon1; // Create an instance for ammeter
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
#define RPM1 120 //desired speed
#define RPM2 15
#define MICROSTEPS 1
  /*Since microstepping is set externally, make sure this matches the selected mode
  If it doesn't, the motor will move at a different RPM than chosen
  1=full step, 2=half step etc. */

double stepDelay = 60.0/(long(RPM1)*long(MOTOR_STEPS1)*long(MICROSTEPS)*2)*1000000; //speed of delay used to control vertical speed (microseconds)
long distance=0; //step count

int toolDegrees = 0; //angle for tool changer;
int toolDistance = 30400;

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
    pinMode(PUMP,OUTPUT);
    pinMode(PROBE,OUTPUT);
    pinMode(VALVE1,OUTPUT);
    pinMode(VALVE2,OUTPUT);
    pinMode(VALVE3,OUTPUT);
    
    digitalWrite(DRILL,HIGH);
    digitalWrite(PUMP,HIGH);//all relays should be HIGH to be off
    digitalWrite(PROBE,HIGH);
    digitalWrite(VALVE1,HIGH);
    digitalWrite(VALVE2,HIGH);
    digitalWrite(VALVE3,HIGH);
   
    //Tool changer stepper
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

    if(sref==2) retract(); //Pull out then stop

    if(sref==3) tool1(); 

    if(sref==4) tool2();

    if(sref==5) tool3();

    if(sref==6) tool4();

    if(sref==7) heater(); //Heating Element

    if(sref==8) pump();

    if(sref==9) valve1();
} //end main loop function

 

/////////////////////////////////////////////////////////////////////////////////////////
void drillDown(void)
{
  digitalWrite(DRILL,LOW);//turns on drill relays
  delay(1000); //wait 1sec before starting MOVE

  digitalWrite(dirPin, HIGH);//High for descent

  distance = 0;

  while(sref==1||sref==0) //ADD A CALCULATED DISTANCE MAX
  {
    //one step
    setPin(stepPin);
    delayMicroseconds(stepDelay);
    clrPin(stepPin);
    delayMicroseconds(stepDelay);

    //digital core
    distance = distance+1;

    if(int(distance)%200==0)//Output data every 1/8 turn
    {
      Irms = emon1.calcIrms(5);  // Calculate Irms only
      force = forceSensor.get_units(1); //averages 1 readings for output

      Serial.print(distance);
      Serial.print(" ");
      Serial.print(force);
      Serial.print(" ");
      Serial.println(Irms);
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
    
     if(int(distance)/200==0)//Output data every 1/8 turn
    {
      Irms = emon1.calcIrms(5);  // Calculate Irms only
      Serial.print(distance);
      Serial.print(" ");
      Serial.print(force);
      Serial.print(" ");
      Serial.print(Irms);
    }
    
    if(Serial.available())
      sref = Serial.parseInt();

    distance = distance-1;
  } 
  if(sref!=10)
    digitalWrite(DRILL,HIGH); //turns off drill AFTER full retraction (and only if stop isn't pressed)
}

 

/////////////////////////////////////////////////////////////////////////////////////////
void heater(void)
{
  digitalWrite(PROBE,LOW); 

  while(sref == 7 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(PROBE,HIGH);
}

 

void pump(void)
{
  digitalWrite(PUMP, LOW);

  while(sref == 8 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(PUMP,HIGH);
}

 

/////////////////////////////////////////////////////////////////////////////////////////
void tool1(void)
{
toolDegrees = 149;
  int stepsMove = MOTOR_STEPS2/360*toolDegrees; //converts from degrees to steps

  stepper2.move(stepsMove);
  delay(500);
  
  //move down drill
  digitalWrite(dirPin,HIGH);
  double count = 0;
  for(int i; i<toolDistance; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    count = count+1;
  }

  delay(3000);

  //move back up to same position
  digitalWrite(dirPin,LOW);
  while(count>0)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    count = count-1;
  }

  //return to original angle
  stepper2.move(-1*stepsMove);
}



/////////////////////////////////////////////////////////////////////////////////////////
void tool2(void)
{
  toolDegrees = 90;
  int stepsMove = MOTOR_STEPS2/360*toolDegrees; //converts from degrees to steps

  stepper2.move(stepsMove);
  delay(500);
  
  //move down drill
  digitalWrite(dirPin,HIGH);
  double count = 0;
  for(int i; i<toolDistance; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    count = count+1;
  }

  delay(3000);

  //move back up to same position
  digitalWrite(dirPin,LOW);
  while(count>0)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    count = count-1;
  }

  //return to original angle
  stepper2.move(-stepsMove);
}

 

/////////////////////////////////////////////////////////////////////////////////////////

void tool3(void)

{
toolDegrees = -149;
  int stepsMove = MOTOR_STEPS2/360*toolDegrees; //converts from degrees to steps

  stepper2.move(stepsMove);
  delay(500);
  
  //move down drill
  digitalWrite(dirPin,HIGH);
  double count = 0;
  for(int i; i<toolDistance; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    count = count+1;
  }

  delay(3000);

  //move back up to same position
  digitalWrite(dirPin,LOW);
  while(count>0)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    count = count-1;
  }

  //return to original angle
  stepper2.move(-stepsMove);
}

 

/////////////////////////////////////////////////////////////////////////////////////////

void tool4(void)

{
toolDegrees = -90;
  int stepsMove = MOTOR_STEPS2/360*toolDegrees; //converts from degrees to steps

  stepper2.move(stepsMove);
  delay(500);
  
  //move down drill
  digitalWrite(dirPin,HIGH);
  double count = 0;
  for(int i; i<toolDistance; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    count = count+1;
  }

  delay(3000);

  //move back up to same position
  digitalWrite(dirPin,LOW);
  while(count>0)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);

    count = count-1;
  }

  //return to original angle
  stepper2.move(-stepsMove);
}

 

 

void valve1(void)

{
  digitalWrite(VALVE1, LOW);

  while(sref == 9 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(VALVE1,HIGH);

  
}
