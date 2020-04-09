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
const int HIGHSPEED = 60;
const int LOWSPEED = 20;
const int stepsPerRevolution = 200;
Stepper myStepper(stepsPerRevolution, STEP1A, STEP1B, STEP2A, STEP2B);

// operational variables
bool dispensingNut = false;
bool nutSensed = false;
bool startUp = true;
unsigned long startTime = millis();
const int waitMs = 10 * 1000; // sec *1000
int nextTrys = 0;
int dir = 1; // clockwise

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
      if (!digitalRead(IRDET)) { // if empty
        nextNut();
        if (!nutSensed) { // did something come out?
          nextTrys++;
        } else { // sure did, exit
          nextTrys = 0;
          //          startTime = millis();
        }
        if (nextTrys == 10) {
          nextTrys = 0;
          startUp = true; // nuts empty
        }
      } else { // if full
        nextTrys = 0;
        //        startTime = millis();
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
  } else {
    startTime = millis();
  }
}

void nextNut() {
  nutSensed = false;
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
  delay(200); // nut falling
  dispensingNut = false;
  dir = -dir;
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
