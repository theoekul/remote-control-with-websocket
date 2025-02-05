#include <Arduino.h>
#include "rotationread.h"

void initRotRead() {
    pinMode(ROTR_01,  INPUT);
    pinMode(ROTR_02,  INPUT);
    pinMode(ROTR_03,  INPUT);
    pinMode(ROTR_04,  INPUT);
    pinMode(ROTR_05,  INPUT);
    pinMode(ROTR_06,  INPUT);
    pinMode(ROTR_07,  INPUT);
    pinMode(ROTR_08,  INPUT);
}
// denne funksjonen er for å lese av rotary switchen og skrive ut posisjonen til serial monitor
void readRotarySwitch() { 
    String position = "Ingen posisjon";
    if (digitalRead(ROTR_01) == HIGH) position = "Sør"; // 1
    else if (digitalRead(ROTR_02) == HIGH) position = "Sørvest"; // 2
    else if (digitalRead(ROTR_03) == HIGH) position = "Vest"; // 3
    else if (digitalRead(ROTR_04) == HIGH) position = "Nordvest"; // 4
    else if (digitalRead(ROTR_05) == HIGH) position = "Nord"; // 5
    else if (digitalRead(ROTR_06) == HIGH) position = "Nordøst"; // 6
    else if (digitalRead(ROTR_07) == HIGH) position = "Øst"; // 7
    else if (digitalRead(ROTR_08) == HIGH) position = "Sørøst"; // 8
    else Serial.println("No position detected");
    Serial.print("Rotary switch position: ");
    Serial.println(position);
}

  

