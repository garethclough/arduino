
int buttonPin = 9;
int subtractButtonPin = 10;
int storedVal = 0;
int storedSubtractVal = 0;
int counter = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  delay(100);
}

void loop() {
  int val = digitalRead(buttonPin);
  if (val != storedVal) {
    if (val == 0 && storedVal == 1) {
      counter++;
      Serial.print("Count: ");
      Serial.println(counter);
    }
  }
  storedVal = val;

  int subtractVal = digitalRead(subtractButtonPin);
  if (subtractVal != storedSubtractVal) {
    if (subtractVal == 0 && storedSubtractVal == 1) {
      counter--;
      Serial.print("Count: ");
      Serial.println(counter);
    }
  }
  storedSubtractVal = subtractVal;

  delay(100);
}
