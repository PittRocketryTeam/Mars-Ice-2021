//check actuator direction; if these are flipped, they'll need to be flipped in the code
#define act1 34
#define act2 36
void setup() {
  // put your setup code here, to run once:
  pinMode(act1,OUTPUT);
  pinMode(act2,OUTPUT);
}

void loop() {
  //THis SHOULD be extension
  digitalWrite(act1,HIGH);
  digitalWrite(act2,LOW);

  delay(3000)//actuator will stop automatically because of internal limit switches

  //this SHOULD be contraction
  digitalWrite(act1,LOW);
  digitalWrite(act2,HIGH);
}
