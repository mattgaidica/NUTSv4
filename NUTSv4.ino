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
const int HIGHSPEED = 45;
const int LOWSPEED = 10;
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, STEP1A, STEP1B, STEP2A, STEP2B);

// operational variables
bool dispensingNut = false;
bool nutSensed = false;
bool startUp = true;
unsigned long startTime = millis();
const int waitMs = 5000;
int nextTrys = 0;

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
      attachInterrupt(digitalPinToInterrupt(IRDET), IRbreak, CHANGE);
      IRbreak(); // init
      startTime = millis();
    }
    pinMode(BUTTON, OUTPUT);
  } else {
    unsigned long currentTime = millis();
    if ((currentTime - startTime) > waitMs) {
      if (!digitalRead(IRDET)) { // fill the port
        nextNut();
        if (!nutSensed) { // did something come out?
          nextTrys++;
        } else { // sure did, exit
          nextTrys = 0;
          startTime = millis();
        }
        if (nextTrys == 10) {
          nextTrys = 0;
          startUp = true; // nuts empty
        }
      } else {
        nextTrys = 0;
        startTime = millis();
      }
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

//bool nutPresent() {
//  return nutSensed || digitalRead(IRDET);
//}

void IRbreak() {
  digitalWrite(BUTTON, digitalRead(IRDET));
  if (dispensingNut) {
//    if (digitalRead(IRDET)) {
      nutSensed = true;
//    }
  }
}

void nextNut() {
  nutSensed = false;
  dispensingNut = true;
  // kick out of sensor area
  if (!digitalRead(MAGNET)) {
    myStepper.step(15);
  }
  int trys = 0;
  while (digitalRead(MAGNET)) {
    myStepper.step(5);
    trys++;
    if (trys % 10 == 0) {
      unjam();
    }
    if (trys > 100) {
      break; // something is wrong
    }
  }
  // go extra, get out of sensor range
  myStepper.setSpeed(LOWSPEED);
  myStepper.step(10);
  myStepper.setSpeed(HIGHSPEED);
  stepperOff();
  delay(200);
  dispensingNut = false;
}

void unjam() {
  myStepper.step(-20);
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
