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
    int enteredNumber = entradaSerial.toInt();
    if (enteredNumber < 0 || enteredNumber > 255) {
      Serial.println("Enter a number between 0 and 255");
    } else {
      Serial.println("Entered:");
      Serial.println(enteredNumber);
      EEPROM.write(0, (byte)enteredNumber);     
    }
    entradaCompleta = false;
    entradaSerial = "";
  }
}


void loop()
{
  byte c = EEPROM.read(0);
  Serial.println("Read:");
  Serial.println(c);
  delay(1000);
}