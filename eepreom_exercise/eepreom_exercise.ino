#include <EEPROM.h>

int count = 0;
int value;

void setup()
{
  Serial.begin(9600);
  delay(1000);
}

void loop()
{

  count++;

  value = EEPROM.read(0);
  Serial.println("Read:");
  Serial.print(count);
  Serial.print('\t');
  Serial.print(value);
  Serial.println();

  EEPROM.write(0, (int)random(255));

  value = EEPROM.read(0);
   Serial.println("Write:");
  Serial.print(count);
  Serial.print('\t');
  Serial.print(value);
  Serial.println();


  delay(60000);
}