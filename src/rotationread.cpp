#include <Arduino.h>
#include "RotationRead.h"

void initRotRead() { //Denne setter rotary switchen til input
    pinMode(ROTR_01,      INPUT);
    pinMode(ROTR_02,      INPUT);
    pinMode(ROTR_03,      INPUT);
    pinMode(ROTR_04,      INPUT);
    pinMode(ROTR_05,      INPUT);
    pinMode(ROTR_06,      INPUT);
    pinMode(ROTR_07,      INPUT);
    pinMode(ROTR_08,      INPUT);
}
void checkRotarySwitch() { //Denne sjekker hvilken posisjon rotary switchen er i å skriver det ut i serial monitor
    if (digitalRead(ROTR_01) == HIGH) {
        Serial.println("Rotary Switch Position: 1");
    } else if (digitalRead(ROTR_02) == HIGH) {
        Serial.println("Rotary Switch Position: 2");
    } else if (digitalRead(ROTR_03) == HIGH) {
        Serial.println("Rotary Switch Position: 3");
    } else if (digitalRead(ROTR_04) == HIGH) {
        Serial.println("Rotary Switch Position: 4");
    } else if (digitalRead(ROTR_05) == HIGH) {
        Serial.println("Rotary Switch Position: 5");
    } else if (digitalRead(ROTR_06) == HIGH) {
        Serial.println("Rotary Switch Position: 6");
    } else if (digitalRead(ROTR_07) == HIGH) {
        Serial.println("Rotary Switch Position: 7");
    } else if (digitalRead(ROTR_08) == HIGH) {
        Serial.println("Rotary Switch Position: 8");
    } else {
        Serial.println("Rotary Switch Position: Unknown");
    }
}
