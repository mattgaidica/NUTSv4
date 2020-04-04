#include <Stepper.h>
// GPIO
const int BUTTON = 0; // active LOW, use INPUT_PULLUP
const int LED1 = 8;
const int LED2 = 13;
const int IRDET = 5; // active LOW, could remove pull-up on PCB
// motor
const int STEP1A = 6;
const int STEP1B = 7;
const int STEP2A = 11;
const int STEP2B = 12;
const int STEPSTBY = 10; // active LOW !! ADD THIS, remove pull-up on PCB?
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, STEP1A, STEP1B, STEP2A, STEP2B);

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED2, INPUT);
  digitalWrite(LED1,LOW);
  digitalWrite(LED1,LOW);

  pinMode(STEPSTBY, OUTPUT);
  digitalWrite(STEP1A,LOW);
  digitalWrite(STEP2A,LOW);
  digitalWrite(STEP1B,LOW);
  digitalWrite(STEP2B,LOW);
  Serial.println("NUTSv4");
}

void loop() {
  myStepper.step(10);

  digitalWrite(LED1,HIGH);
  delay(200);
  digitalWrite(LED1,LOW);
  delay(200);
}

void stepperOff() {
  Serial.println("Stpeper off");
  digitalWrite(STEP1A,LOW);
  digitalWrite(STEP1A,LOW);
  digitalWrite(STEP2A,LOW);
  digitalWrite(STEP1B,LOW);
  digitalWrite(STEP2B,LOW);
}
