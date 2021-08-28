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
#include "EmonLib.h"

//AMMETER VARIABLES
EnergyMonitor emon1; // Create an instance for ammeter
double Irms;

 

//SERIAL COMMUNICATION VARIABLES
int sref=0; //sref is with computer

 

//DIGITAL CORE DEFINITIONS
HX711 forceSensor; //initializes force sensor object
#define dataPin 51
#define clockPin 53

float force = 0,dist = 0;

/////////////////////////////////////////////////////////////////////////////////////////
void setup() {
    //This code initializes force sensor
    forceSensor.begin(dataPin, clockPin);
    forceSensor.set_scale(420.0983); // loadcell factor 5 KG; CALIBRATION IS STILL REQUIRED
    forceSensor.tare(); //zeroes load cell

    //Begin serial communication, for use with MATLAB
    Serial.begin(9600);


    //Connect to secondary arduino
    Serial1.begin(9600);

    emon1.current(1, 111.1); //for ammeter

}//end setup

 
 
/////////////////////////////////////////////////////////////////////////////////////////
void loop()
{  
    if(Serial.available())
      sref = Serial.parseInt(); //which state are we in? 0 is read for no MATLAB inputs

    if(sref==1) 
      {
      Serial1.println("1 ");//Drilling down

      while((sref==1||sref==0))
      {
        force = forceSensor.get_units(1);
        dist = Serial1.parseInt();
        Serial.print(dist);
        Serial.print(" ");
        Serial.println(force);

        if(Serial.available())
          sref = Serial.parseInt();
      }

      Serial1.println("10 "); //stop
      }
      
    if(sref==2) //Pull out then stop
    {
      Serial1.println("2 ");//Dretract
      while(sref==2||sref==0)
      {
        Serial.println("current reading");  
        if(Serial.available())
          sref = Serial.parseInt();
      }

      Serial1.println("10 ");
    }

    if(sref==3) ; 

    if(sref==4) ;

    if(sref==5) ;

    if(sref==6) ;

    if(sref==7) ; //Heating Element

    if(sref==8) ;

    if(sref==9) ;
} //end main loop function

 

/////////////////////////////////////////////////////////////////////////////////////////
/*
void drillDown(void)
{
  //digitalWrite(DRILL,LOW);//turns on drill relays
  Serial2.println("1");
  
  
  while((sref==1||sref==0)&& digitalRead(botLimit)==LOW) //ADD A CALCULATED DISTANCE MAX
  {
    
      distance = Serial2.parse()
      Irms = emon1.calcIrms(1);  // Calculate Irms only
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

  while((sref==2||sref==0)&&digitalRead(topLimit)==0) //reverses count to end at starting position (if not stopped externally)
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
*/
