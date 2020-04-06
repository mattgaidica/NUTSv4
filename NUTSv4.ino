#include <Stepper.h>
// GPIO
const int BUTTON = 0; // active LOW, use INPUT_PULLUP
const int MAGNET = 2;
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
  myStepper.setSpeed(45);
  pinMode(BUTTON, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  pinMode(STEPSTBY, OUTPUT);
  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP2A, LOW);
  digitalWrite(STEP1B, LOW);
  digitalWrite(STEP2B, LOW);

  attachInterrupt(digitalPinToInterrupt(MAGNET), magnetDisplay, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BUTTON), nextNut, LOW);

  Serial.println("NUTSv4");
}

void loop() {
  //  myStepper.step(30);
  nextNut();
  digitalWrite(LED1, HIGH);
  delay(1000);
  digitalWrite(LED1, LOW);
  delay(1000);
}

void magnetDisplay() {
  digitalWrite(LED2, digitalRead(MAGNET));
}

void nextNut() {
  // kick out of sensor area
  if (!digitalRead(MAGNET)) {
    myStepper.step(15);
  }
  int trys = 0;
  while (digitalRead(MAGNET)) {
    myStepper.step(5);
    trys++;
    if(trys % 10 == 0) {
      unjam();
    }
    if(trys > 100) {
      break; // something is wrong
    }
  }
  // go extra, get out of sensor range
  for (int ii=0; ii<10; ii++) {
    myStepper.step(1);
    delay(50);
  }
  stepperOff();
}

void unjam() {
  myStepper.step(-20);
}

void sensedMagnet() {
  detachInterrupt(digitalPinToInterrupt(MAGNET));
  Serial.println("MAGNET!");
}

void stepperOff() {
  Serial.println("Stpeper off");
  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP2A, LOW);
  digitalWrite(STEP1B, LOW);
  digitalWrite(STEP2B, LOW);
}
