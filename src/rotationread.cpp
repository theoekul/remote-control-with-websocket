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
void initreadRotarySwitch() { 
    String position = "Ingen posisjon";
    delay(50); // debounce delay
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
void initupdateLEDsBasedOnPosition() { // denne funksjonen er for å skru av og på LED basert på posisjonen til rotary switchen
    if (digitalRead(ROTR_01) == HIGH) { 
        digitalWrite(LED_5, HIGH); 
    } else if (digitalRead(ROTR_05) == HIGH) {
        digitalWrite(LED_1, HIGH);
    } else if (digitalRead(ROTR_02) == HIGH) {
        digitalWrite(LED_4, HIGH);
    } else if (digitalRead(ROTR_06) == HIGH) {
        digitalWrite(LED_2, HIGH);
    } else if (digitalRead(ROTR_03) == HIGH) {
        digitalWrite(LED_3, HIGH);
    } else if (digitalRead(ROTR_07) == HIGH) {
        digitalWrite(LED_7, HIGH);
    } else if (digitalRead(ROTR_04) == HIGH) {
        digitalWrite(LED_6, HIGH);
    } else if (digitalRead(ROTR_08) == HIGH) {
        digitalWrite(LED_8, HIGH);
    } else {
        digitalWrite(LED_1, LOW);
        digitalWrite(LED_2, LOW);
        digitalWrite(LED_3, LOW);
        digitalWrite(LED_4, LOW);
        digitalWrite(LED_5, LOW);
        digitalWrite(LED_6, LOW);
        digitalWrite(LED_7, LOW);
        digitalWrite(LED_8, LOW);
    }
}


