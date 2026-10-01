#include <EEPROM.h>

int count = 0;
int value;
String entradaSerial = "";         // String para almacenar entrada
bool entradaCompleta = false;  // Indicar si el String está completo

void setup()
{
  Serial.begin(9600);
  delay(1000);
}

void serialEvent() {
  while (Serial.available()) {
    // Obtener bytes de entrada:
    char inChar = (char)Serial.read();
    // Agregar al String de entrada:
    entradaSerial += inChar;
    // Para saber si el string está completo, se detendrá al recibir
    // el caracter de retorno de línea ENTER \n
    if (inChar == '\n') {
      entradaCompleta = true;
    }
  }
  if (entradaCompleta) {
    Serial.println("Entered: " + entradaSerial);
    for (int i = 0; i < entradaSerial.length(); ++i) {
      char c = entradaSerial[i];
      EEPROM.write(i, c);
    }
    entradaCompleta = false;
    entradaSerial = "";
  }
}


void loop()
{
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
    Serial.println("Read:");
    Serial.println(message);
  } else {
    Serial.println("No Message Found");
  }
  delay(1000);

/*
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
  */
}