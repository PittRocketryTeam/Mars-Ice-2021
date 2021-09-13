/*
* Pitt SOAR
* This is the comprehensive code for controlling the drill via MATLAB and Arduino
* Updated 08/26/2021
*/

 

/*THINGS THAT STILL NEED DONE:
Uncomment actuator code in each tool change section when ready
*/

#include<HX711.h> //Load cell library
#include<BasicStepperDriver.h> //Stepper driver library (only for tool changer)
#include "EmonLib.h"

//AMMETER VARIABLES
EnergyMonitor emon1; // Create an instance for ammeter
double Irms;

 
//SERIAL COMMUNICATION VARIABLES
int sref=0;

 
//DIGITAL CORE DEFINITIONS
HX711 forceSensor; //initializes force sensor object
#define dataPin 21
#define clockPin 53
float force = 0,dist = 0;

 
//MOTOR CONTROL DEFINITIONS
//Motor 1 is vertical motion, motor 2 is tool changer
// Define Motor steps per revolution. Ours is set with the switches on the motor controller
#define MOTOR_STEPS1 800 
#define MOTOR_STEPS2 3200
#define RPM1 120 
#define RPM2 15 
#define MICROSTEPS 1
  /*Since microstepping is set externally, make sure this matches the selected mode
  If it doesn't, the motor will move at a different RPM than chosen
  1=full step, 2=half step etc. */
  
//calculation for length of delay used to control vertical speed (microseconds)
double stepDelay = 60.0/(long(RPM1)*long(MOTOR_STEPS1)*long(MICROSTEPS)*2)*1000000; 
long distance=0; //step count
int toolDegrees = 0; //angle for tool changer;
int toolDistance = 15000;
int toolDropDistance = 16000;
int tool1Degrees = 160;
int tool2Degrees = -149;
int steps2Move = MOTOR_STEPS2/360*tool2Degrees; //converts from degrees to steps
int steps1Move = MOTOR_STEPS2/360*tool1Degrees; //converts from degrees to steps

// All the wires needed for full functionality; motor 1 (vertical) and motor2 (tool change)
#define stepPin 2
#define dirPin 3
#define stepPin2 4
#define dirPin2 5
//limit switch digital pins
#define topLimit 14
#define botLimit 15
//actuator pins
#define act1 34
#define act2 36
// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper2(MOTOR_STEPS2, dirPin2, stepPin2);

 

//RELAY DEFINITIONS
#define DRILL  8
#define PROBE  9
#define PUMP  10

#define VALVE1  11
#define VALVE2  12
#define VALVE3  13
#define VALVE4  



/////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    //initialize stepper motors
    pinMode(stepPin,OUTPUT);
    pinMode(dirPin,OUTPUT);
    pinMode(act1,OUTPUT);
    pinMode(act2,OUTPUT);

    //initializes limit switches
    pinMode(topLimit,INPUT);
    pinMode(botLimit,INPUT);

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
    forceSensor.set_scale(420.0983); // loadcell factor 5 KG
    forceSensor.tare(); //zeroes load cell

    emon1.current(1, 111.1); //for ammeter


    attachInterrupt(digitalPinToInterrupt(dataPin),digitalCore,LOW);

    
    //Begin serial communication, for use with MATLAB
    Serial.begin(9600);

}//end setup

 
 
/////////////////////////////////////////////////////////////////////////////////////////
void loop()
{  
    if(Serial.available())
      sref = Serial.parseInt(); //which state are we in? 0 is read for no MATLAB inputs

    if(sref==1) drillDown(); //Drilling down

    if(sref==2) retract(); //Pull out then stop

    if(sref==3) beginToolChange();

    if(sref==4) tool1(); 

    if(sref==5) tool2();

    //6 and 7 are reserved for pulsing to realign the drill

    if(sref==8) heater(); //Heating Element

    if(sref==9) pump();

    if(sref==10) valve1();

    if(sref==11) valve2();

    if(sref==12) valve3();

    if(sref==13) valve4();
} //end main loop function

 

/////////////////////////////////////////////////////////////////////////////////////////
void drillDown(void)
{
  digitalWrite(DRILL,LOW);//turns on drill relays
  delay(1000); //wait 1sec before starting MOVE

  digitalWrite(dirPin, HIGH);//High for descent

  distance = 0;

  //continue until signal change or limit switch
  while((sref==1||sref==0)&& digitalRead(botLimit)==LOW) 
  {
    //one step
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(stepDelay);

    //digital core counter
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

    //check for changes from GUI
    if(Serial.available())
      sref = Serial.parseInt();
  }
}

 

/////////////////////////////////////////////////////////////////////////////////////////
void retract(void)
{
  digitalWrite(dirPin, LOW);

  while((sref==2||sref==0)&&digitalRead(topLimit)==0) //reverses unless stopped or limit switch is hit
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

  } 
  digitalWrite(DRILL,HIGH); //turns off drill AFTER full retraction
}

 

/////////////////////////////////////////////////////////////////////////////////////////
void heater(void)
{
  digitalWrite(PROBE,LOW); 

  while(sref == 8 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(PROBE,HIGH);
}

 

void pump(void)
{
  digitalWrite(PUMP, LOW);

  while(sref == 9 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(PUMP,HIGH);
}

/////////////////////////////////////////////////////////////////////////////////////////
beginToolChange(void)
{
  //needs to be 2 so retract will work properly
  sref=2;
  //always start in same up position
  retract(); 
  
  //allow repositioning
  sref = 6;
  //5 is a pulse, 6 is a wait to check if it's gucci
  while(sref == 5 || sref ==6)
  {
    if(sref == 5)
    {
      //pulse drill
      digitalWrite(DRILL,LOW);
      delay(500);
      digitalWrite(DRILL,HIGH);
      sref == 6;
    }
    
    if(Serial.available())
      sref = Serial.parseInt();
  } 
}

/////////////////////////////////////////////////////////////////////////////////////////
void rotCW(void)
{
  while(sref == 5 || sref ==6)
  {
    if(sref == 5)
    {
      //pulse drill
      digitalWrite(DRILL,LOW);
      delay(500);
      digitalWrite(DRILL,HIGH);
      sref == 6;
    }
    
    if(Serial.available())
      sref = Serial.parseInt();
  } 
}

/////////////////////////////////////////////////////////////////////////////////////////
void rotCCW(void)
{
  
}


/////////////////////////////////////////////////////////////////////////////////////////
void tool1(void)

//STEPS
//retract to starting position check
//spin drill until good check
//drop down check
//rotate tools
//actuator release
//raise
//rotate to other tool
//lower
//actuator clamp
//slight raise
//rotate tools to center
//retract completely


{
  //move down drill
  digitalWrite(dirPin,HIGH);
  for(int i; i<toolDropDistance; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }

  
  //rotate with original tool
  stepper2.move(steps1Move);
  delay(500);

  //release
  digitalWrite(act1,HIGH);
  digitalWrite(act2,LOW);
  delay(1000);
  digitalWrite(act1,HIGH);
  digitalWrite(act2,HIGH);

  //raise
  digitalWrite(dirPin,LOW);
  for(int i; i<toolDropDistance/2; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }

  //rotate to other tool
  stepper2.move(-steps1Move);
  delay(500);
  stepper2.move(steps2Move);
  delay(500);
  
  //lower drill
  digitalWrite(dirPin,HIGH);
  for(int i; i<toolDropDistance/2; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(500);

  //lock actuator
  digitalWrite(act1,LOW);
  digitalWrite(act2,HIGH);
  delay(1000);
  digitalWrite(act1,HIGH);
  digitalWrite(act2,HIGH);

  //slight raise
  digitalWrite(dirPin,LOW);
  for(int i; i<toolDropDistance/10; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  
  //rotate to center
  stepper2.move(-steps2Move);
  delay(500);

  //raise entirely
  sref = 2;
  retract();
  
}



/////////////////////////////////////////////////////////////////////////////////////////
void tool2(void)
{
  //move down drill
  digitalWrite(dirPin,HIGH);
  for(int i; i<toolDropDistance; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }

  
  //rotate with original tool
  stepper2.move(steps2Move);
  delay(500);

  //release
  digitalWrite(act1,HIGH);
  digitalWrite(act2,LOW);
  delay(1000);
  digitalWrite(act1,HIGH);
  digitalWrite(act2,HIGH);

  //raise
  digitalWrite(dirPin,LOW);
  for(int i; i<toolDropDistance/2; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }

  //rotate to other tool
  stepper2.move(-steps2Move);
  delay(500);
  stepper2.move(steps1Move);
  delay(500);
  
  //lower drill
  digitalWrite(dirPin,HIGH);
  for(int i; i<toolDropDistance/2; i++) //recalibrate it manually until we get a limit switch
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  delay(500);

  //lock actuator
  digitalWrite(act1,LOW);
  digitalWrite(act2,HIGH);
  delay(1000);
  digitalWrite(act1,HIGH);
  digitalWrite(act2,HIGH);

  //slight raise
  digitalWrite(dirPin,LOW);
  for(int i; i<toolDropDistance/10; i++)
  {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay);
  }
  
  //rotate to center
  stepper2.move(-steps1Move);
  delay(500);

  //raise entirely
  sref = 2;
  retract();
}


void valve1(void)

{
  digitalWrite(VALVE1, LOW);

  while(sref == 10 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(VALVE1,HIGH);

  
}

void valve2(void)

{
  digitalWrite(VALVE2, LOW);

  while(sref == 11 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(VALVE2,HIGH);

  
}

void valve3(void)

{
  digitalWrite(VALVE3, LOW);

  while(sref == 12 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(VALVE3,HIGH);

  
}

void valve4(void)

{
  digitalWrite(VALVE4, LOW);

  while(sref == 13 || sref == 0)
  {
    if(Serial.available())
      sref = Serial.parseInt();
  }
  digitalWrite(VALVE4,HIGH);
}

void digitalCore(void)
{
      Irms = emon1.calcIrms(5);  // Calculate Irms only
      force = forceSensor.get_units(1); //averages 1 readings for output
      Serial.print(distance);
      Serial.print(" ");
      Serial.print(force);
      Serial.print(" ");
      Serial.println(Irms);
}
