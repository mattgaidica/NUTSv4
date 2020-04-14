#include <Stepper.h>

// GPIO
const int BUTTON = 0; // active LOW, use INPUT_PULLUP
const int MAGNET = 2; // active LOW, INPUT_PULLUP
const int photo = 5; // active LOW, should remove pull-up on PCB
const int LED1 = 8;
const int LED2 = 13;

// motor
const int STEP1A = 6;
const int STEP1B = 7;
const int STEP2A = 11;
const int STEP2B = 12;
const int HIGHSPEED = 60;
const int LOWSPEED = 20;
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, STEP1A, STEP1B, STEP2A, STEP2B);

// operational variables
bool dispensingNut = false;
bool attendedTo = true; // dispense at startup
bool startUp = true;
const int waitMs = 60 * 1000; // sec *1000
unsigned long startTime = millis();
int dir = 1; // clockwise

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(HIGHSPEED);
  pinMode(BUTTON, OUTPUT);
  pinMode(MAGNET, INPUT_PULLUP);
  pinMode(photo, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);

  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP2A, LOW);
  digitalWrite(STEP1B, LOW);
  digitalWrite(STEP2B, LOW);

  attachInterrupt(digitalPinToInterrupt(MAGNET), magnetDisplay, CHANGE);

  Serial.println("NUTSv4");
}

void loop() {
  if (startUp) {
    digitalWrite(BUTTON, HIGH);
    delay(100);
    digitalWrite(BUTTON, LOW);
    delay(900);
    pinMode(BUTTON, INPUT_PULLUP);
    delay(50); // debounce
    if (!digitalRead(BUTTON)) {
      startUp = false;
      pinMode(BUTTON, OUTPUT);
      blinkFast(BUTTON, 10);
      attachInterrupt(digitalPinToInterrupt(photo), attention, CHANGE);
      attention(); // init
      startTime = millis() + waitMs; // forces nextNut on startup
    }
    pinMode(BUTTON, OUTPUT);
  } else {
    unsigned long currentTime = millis();
    if ((currentTime - startTime) > waitMs && attendedTo) {
      nextNut(); // ASSUME IT DISPENSES
      startTime = millis();
    }

    digitalWrite(LED1, HIGH);
    delay(500);
    digitalWrite(LED1, LOW);
    delay(500);
  }
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

void attention() {
  digitalWrite(BUTTON, !digitalRead(photo));
  if (attendedTo) {
    startTime = millis();
  } else {
    attendedTo = true;
  }
}

void nextNut() {
  dispensingNut = true;
  // kick out of sensor area at least 1/4 rotation
  myStepper.step(dir * stepsPerRevolution / 4);

  int trys = 0;
  int nStep = 2;
  while (digitalRead(MAGNET)) {
    myStepper.step(dir * nStep);
    trys++;
    if (trys % (stepsPerRevolution / nStep) == 0) {
      break; // failed to spin 1 revolution
    }
  }
  // go extra, shake
  myStepper.setSpeed(LOWSPEED);
  myStepper.step(dir * 10);
  myStepper.setSpeed(HIGHSPEED);
  stepperOff();
  dispensingNut = false;
  dir = -dir;
  attendedTo = false;
}

void unjam() {
  myStepper.step(-dir * 20);
}

void sensedMagnet() {
  detachInterrupt(digitalPinToInterrupt(MAGNET));
}

void stepperOff() {
  Serial.println("Stpeper off");
  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP1A, LOW);
  digitalWrite(STEP2A, LOW);
  digitalWrite(STEP1B, LOW);
  digitalWrite(STEP2B, LOW);
}
