#include <Stepper.h>

// GPIO
const int BUTTON = 0; // active LOW, use INPUT_PULLUP
const int MAGNET = 2; // active LOW, INPUT_PULLUP
const int IRDET = 5; // active LOW, should remove pull-up on PCB
const int LED1 = 8;
const int LED2 = 13;

// motor
const int STEP1A = 6;
const int STEP1B = 7;
const int STEP2A = 11;
const int STEP2B = 12;
const int HIGHSPEED = 80;
const int LOWSPEED = 30;
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, STEP1A, STEP1B, STEP2A, STEP2B);

// operational variables
bool dir = true; // clockwise

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(HIGHSPEED);
  pinMode(BUTTON, OUTPUT);
  pinMode(MAGNET, INPUT_PULLUP);
  pinMode(IRDET, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP2A, LOW);
  digitalWrite(STEP1B, LOW);
  digitalWrite(STEP2B, LOW);

  attachInterrupt(digitalPinToInterrupt(MAGNET), magnetDisplay, CHANGE);
}

void loop() {
  nextNut();
  digitalWrite(LED1, HIGH);
  delay(1000);
  digitalWrite(LED1, LOW);
  delay(1000);
}

void blinkFast(int pin, int n) {
  for (int ii = 0; ii < n; ii++) {
    digitalWrite(pin, HIGH);
    delay(50);
    digitalWrite(pin, LOW);
    delay(200);
  }
}

void magnetDisplay() {
  digitalWrite(LED2, digitalRead(MAGNET));
}

void nextNut() {
  if (dir) {
    dir = false;
    myStepper.step(80);
    myStepper.setSpeed(LOWSPEED);
    myStepper.step(20);
    myStepper.setSpeed(HIGHSPEED);
  } else {
    dir = true;
    myStepper.step(-80);
    myStepper.setSpeed(LOWSPEED);
    myStepper.step(-20);
    myStepper.setSpeed(HIGHSPEED);
  }
  stepperOff();
}

void stepperOff() {
  Serial.println("Stpeper off");
  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP2A, LOW);
  digitalWrite(STEP1B, LOW);
  digitalWrite(STEP2B, LOW);
}
