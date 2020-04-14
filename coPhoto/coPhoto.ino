const int light = A1;
const int ind = 2;
const int LED = 13;

void setup() {
  pinMode(light, INPUT);
  pinMode(ind, OUTPUT);
  pinMode(LED, OUTPUT);
//  Serial.begin(9600);
}

void loop() {
//  Serial.println(analogRead(light));
//  delay(100);
  if (analogRead(light) < 200) {
    digitalWrite(ind,LOW);
    digitalWrite(LED,LOW);
  } else {
    digitalWrite(ind,HIGH);
    digitalWrite(LED,HIGH);
  }
}
