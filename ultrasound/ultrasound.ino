#include <EEPROM.h>
#include <avr/wdt.h>

#define US1_TRIG_PIN 9
#define US1_ECHO_PIN 10
#define LED_PIN 8
#define US2_TRIG_PIN 7
#define US2_ECHO_PIN 6
#define BUTTON_PIN 2
#define ONBOARD_LED_PIN 13
#define ECHO_PULSE_TIMEOUT_MICROSECONDS 20000
#define ULTRASOUND_CYCLE_DELAY_MS 200
#define MAIN_CYCLE_DELAY_MS 20

String messageString = "";
bool messageComplete = false;
int minDistance = 5; // in cm 
bool warningLight = false;
bool switchedOn = false;
int loopCounter = 0;


void setup() {
  Serial.begin(9600);

  // clear garbage
  Serial.flush();
  while (Serial.available() > 0) {
      Serial.read();
  }

  pinMode(US1_TRIG_PIN, OUTPUT);
  pinMode(US1_ECHO_PIN, INPUT);
  pinMode(US2_TRIG_PIN, OUTPUT);
  pinMode(US2_ECHO_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(ONBOARD_LED_PIN, OUTPUT);

  // Configure the digital pin as an output
  pinMode(LED_PIN, OUTPUT);
  
  // Turn the LED off
  turnOffWarningLight();
  readEeprom();

  // Enable watchdog with a 2-second timeout
  wdt_enable(WDTO_2S);

  Serial.println("Watchdog enabled");  
}

void writeToEeprom(int newMinDistance) {
    String distanceString = String(newMinDistance) + '\n';

    for (unsigned int i = 0; i < distanceString.length(); ++i) {
        EEPROM.write(i, distanceString[i]);
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
    Serial.print("Failed to read min distance from EEPROM. Using default:");
    Serial.println(minDistance);
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
    if (newMinDistance > 0) {
      writeToEeprom(newMinDistance);
      Serial.print("New Min Distance: ");
      Serial.println(newMinDistance);
      minDistance = newMinDistance;
      turnOffWarningLight();

    } else {
      Serial.println("Invalid Distance: " + messageString);
    }
    messageComplete = false;
    messageString = "";
  }
}

void detectObjectLoop() {
  // Clear the trigger pin
  digitalWrite(US1_TRIG_PIN, LOW);
  digitalWrite(US2_TRIG_PIN, LOW);
  delayMicroseconds(2);

  // Send a 10-microsecond high pulse to trigger the sensor
  digitalWrite(US1_TRIG_PIN, HIGH);
  digitalWrite(US2_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(US1_TRIG_PIN, LOW);
  digitalWrite(US2_TRIG_PIN, LOW);

  // Read the echo pin pulse duration in microseconds
  long duration1 = pulseIn(US1_ECHO_PIN, HIGH, ECHO_PULSE_TIMEOUT_MICROSECONDS);
  long duration2 = pulseIn(US2_ECHO_PIN, HIGH, ECHO_PULSE_TIMEOUT_MICROSECONDS);

  // Calculate distance in cm (Speed of sound is ~0.0343 cm/µs, divided by 2 for round trip)
  float distance1 = getDistanceFromDuration(duration1);
  Serial.print("Distance 1: ");
  Serial.print(distance1);
  Serial.println(" cm");

  float distance2 = getDistanceFromDuration(duration2);
  Serial.print("Distance 2: ");
  Serial.print(distance2);
  Serial.println(" cm");


  float averageDistance = (distance1 + distance2) / 2;

  Serial.print("Distance: ");
  Serial.print(averageDistance);
  Serial.println(" cm");

  bool showWarning = (distance1 < minDistance && distance1 != 0) || (distance2 < minDistance && distance2 != 0);
  if (showWarning && !warningLight) {
    turnOnWarningLight();
  } else if (!showWarning && warningLight) {
    turnOffWarningLight();
  }
}

void turnOnWarningLight() {
    digitalWrite(LED_PIN, HIGH);
    warningLight = true;
    Serial.println("Warning Light On");
}

void turnOffWarningLight() {
    warningLight = false;
    Serial.println("Warning Light Off");
    digitalWrite(LED_PIN, LOW);
}

void loop() {
  wdt_reset();
  int buttonState = digitalRead(BUTTON_PIN);

  // Acts as a toggle button - LOW corresponds to button down
  if (buttonState == LOW) {
    Serial.print("Button pressed ");
    switchedOn = !switchedOn;
    Serial.println(switchedOn ? "On" : "Off");
    digitalWrite(ONBOARD_LED_PIN, switchedOn ? HIGH : LOW);
    delay(100);
    loopCounter = 0;

    // This is to test the watchdog - if you hold the button down for 2 seconds the watchdog is triggered
    while (buttonState == LOW) {
      // wdt_reset(); // This is commmented out to test the watchdog
      buttonState = digitalRead(BUTTON_PIN);
    }
    if (!switchedOn) {
      // Turn the LED off
      turnOffWarningLight();
    }
  }

  // Dont run the ultrasound loop every cycle to improve button sensitivity
  int mainCyclesPerUltrasoundCycle = round(ULTRASOUND_CYCLE_DELAY_MS / MAIN_CYCLE_DELAY_MS);
  if (switchedOn && loopCounter % mainCyclesPerUltrasoundCycle == 0) {
    detectObjectLoop();
  }
  loopCounter++;
  delay(MAIN_CYCLE_DELAY_MS);
}
