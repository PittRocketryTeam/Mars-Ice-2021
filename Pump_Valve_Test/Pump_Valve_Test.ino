
#define PUMP  10
#define VALVE1  11

void setup() {
  // put your setup code here, to run once:
pinMode(PUMP, OUTPUT);
pinMode(VALVE1, OUTPUT);
}

void loop() {
     digitalWrite(VALVE1,LOW); //Pin 11
     digitalWrite(PUMP, LOW); //Pin 10

}
