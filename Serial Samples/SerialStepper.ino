# define dirPin 4
# define stepPin 3
#define led 2
#define relay 8
# define stepsPerRevolution 800
long state = 0;
int s= 1;

void setup() {
  // Declare pins as output:
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(relay,LOW);
  Serial.begin(115200);
}

  
void loop() {
  // Set the spinning direction clockwise:

  delay(2000);
  state = Serial.parseInt();
  
  
  if(state==1){  
    digitalWrite(dirPin, HIGH);
    // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < 6*stepsPerRevolution; i++) {
    // These four lines result in 1 step:
    
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(600);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(600);
    }
  }
  
  else if(state==-1){
    digitalWrite(dirPin, LOW);
    // Spin the stepper motor 1 revolution slowly:
  for (int i = 0; i < 6*stepsPerRevolution; i++) {
    // These four lines result in 1 step:
    
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(600);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(600);
    }
  }

  else if(state==2){
    digitalWrite(relay,HIGH);
    }
  else if(state==3){
    digitalWrite(relay,LOW);
    }
  
 
}
