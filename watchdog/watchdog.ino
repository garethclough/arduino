#include <avr/wdt.h>

int counter = 1;

void setup() {
  Serial.begin(9600);

  // Enable watchdog with a 4-second timeout
  wdt_enable(WDTO_4S);

  Serial.println("Watchdog enabled");
}

void loop() {
  Serial.print("Hang for ");
  Serial.print(counter);
  Serial.println(" seconds");
  delay(counter * 1000);
  counter++;
  // Tell the watchdog the program is still running
  wdt_reset();
}