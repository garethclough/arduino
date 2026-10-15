#include <EEPROM.h>

#define US1_TRIG_PIN 9
#define US1_ECHO_PIN 10
#define LED_PIN 8
#define US2_TRIG_PIN 7
#define US2_ECHO_PIN 6

String messageString = "";
bool messageComplete = false;
int minDistance = 5; // in cm 

void setup() {
  Serial.begin(9600);
  pinMode(US1_TRIG_PIN, OUTPUT);
  pinMode(US1_ECHO_PIN, INPUT);
  pinMode(US2_TRIG_PIN, OUTPUT);
  pinMode(US2_ECHO_PIN, INPUT);

  // Configure the digital pin as an output
  pinMode(LED_PIN, OUTPUT);
  
  // Turn the LED off
  digitalWrite(LED_PIN, LOW);
  readEeprom();
}

void writeToEeprom(String newMinDistanceString) {
    for (int i = 0; i < newMinDistanceString.length(); ++i) {
      char c = newMinDistanceString[i];
      EEPROM.write(i, c);
    }
}

String readEeprom() {
  bool foundEndOfString = false;
  int index = 0;
  String message = "";
  while (foundEndOfString == false && index < 255) {
    char c = EEPROM.read(index);
    message += c;
    index++;
    if (c == '\n') {
      foundEndOfString = true;
    }
  }
  if (foundEndOfString) {
    int newMinDistance = message.toInt();
    if (newMinDistance) {
      minDistance = newMinDistance;
      Serial.print("Read min distance from EEPROM:");
      Serial.println(newMinDistance);
    } else {
      Serial.print("Invalid distance from EEPROM. Using default: ");
      Serial.println(minDistance);
    }
    

  } else {
    Serial.println("Failed to read min distance from eprom");
  }
}

float getDistanceFromDuration(float duration) {
  // Calculate distance in cm (Speed of sound is ~0.0343 cm/µs, divided by 2 for round trip)
  return(duration * 0.0343 / 2);
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    messageString += inChar;
    // use end of line to determine end of message
    if (inChar == '\n') {
      messageComplete = true;
    }
  }
  if (messageComplete) {
    int newMinDistance = messageString.toInt();
    if (newMinDistance) {
      writeToEeprom(messageString);
      Serial.println("New Min Distance: " + messageString);
      minDistance = newMinDistance;
    } else {
      Serial.println("Invalid Distance: " + messageString);
    }
    messageComplete = false;
    messageString = "";
  }
}

void loop() {
  // Clear the trigger pin
  digitalWrite(US1_TRIG_PIN, LOW);
  digitalWrite(US2_TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond high pulse to trigger the sensor
  digitalWrite(US1_TRIG_PIN, HIGH);
  digitalWrite(US2_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_TRIG_PIN, LOW);
  digitalWrite(US1_TRIG_PIN, LOW);

  // Read the echo pin pulse duration in microseconds
  long duration1 = pulseIn(US1_ECHO_PIN, HIGH);
  long duration2 = pulseIn(US2_ECHO_PIN, HIGH);

  // Calculate distance in cm (Speed of sound is ~0.0343 cm/µs, divided by 2 for round trip)
  float distance1 = getDistanceFromDuration(duration1);
  float distance2 = getDistanceFromDuration(duration2);

  float averageDistance = (distance1 + distance2) / 2;

  Serial.print("Distance: ");
  Serial.print(averageDistance);
  Serial.println(" cm");

  if (averageDistance < minDistance) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }

  delay(500);
}
