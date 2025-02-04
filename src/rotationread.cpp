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

void readRotarySwitch() { // denne funksjonen er for å lese av rotary switchen og skrive ut posisjonen til serial monitor
    int position = 0;
    if (digitalRead(ROTR_01) == HIGH) position = 1;
    else if (digitalRead(ROTR_02) == HIGH) position = 2;
    else if (digitalRead(ROTR_03) == HIGH) position = 3;
    else if (digitalRead(ROTR_04) == HIGH) position = 4;
    else if (digitalRead(ROTR_05) == HIGH) position = 5;
    else if (digitalRead(ROTR_06) == HIGH) position = 6;
    else if (digitalRead(ROTR_07) == HIGH) position = 7;
    else if (digitalRead(ROTR_08) == HIGH) position = 8;
    else if (digitalRead(ROTR_01) == LOW && digitalRead(ROTR_02) == LOW && digitalRead(ROTR_03) == LOW && digitalRead(ROTR_04) == LOW && digitalRead(ROTR_05) == LOW && digitalRead(ROTR_06) == LOW && digitalRead(ROTR_07) == LOW && digitalRead(ROTR_08) == LOW) {
        Serial.println("Rotary switch position: Unknown");
        return;
    }
    
    Serial.print("Rotary switch position: ");
    Serial.println(position);
    
    }



void setup() {
    Serial.begin(9600);
    initRotRead();
}

void loop() {
    readRotarySwitch();
    delay(500); // Adjust the delay as needed
}
