#include <Arduino.h>
#include "rotswitch.h"

void initRotarySwitch() {
    pinMode(ROTSW_01,      INPUT_PULLDOWN);
    pinMode(ROTSW_02,      INPUT_PULLDOWN);
    pinMode(ROTSW_03,      INPUT_PULLDOWN);
    pinMode(ROTSW_04,      INPUT_PULLDOWN);
    pinMode(ROTSW_05,      INPUT_PULLDOWN);
    pinMode(ROTSW_06,      INPUT_PULLDOWN);
    pinMode(ROTSW_07,      INPUT_PULLDOWN);
    pinMode(ROTSW_08,      INPUT_PULLDOWN);
}
void initRotarySwitch() {
    int position = 0;
    if (digitalRead(ROTSW_01) == HIGH) position = 1;
    else if (digitalRead(ROTSW_02) == HIGH) position = 2;
    else if (digitalRead(ROTSW_03) == HIGH) position = 3;
    else if (digitalRead(ROTSW_04) == HIGH) position = 4;
    else if (digitalRead(ROTSW_05) == HIGH) position = 5;
    else if (digitalRead(ROTSW_06) == HIGH) position = 6;
    else if (digitalRead(ROTSW_07) == HIGH) position = 7;
    else if (digitalRead(ROTSW_08) == HIGH) position = 8;

    Serial.print("Rotary switch position: ");
    Serial.println(position);
}

void setup() {
    Serial.begin(9600);
    initRotarySwitch();
}

void loop() {
    initRotarySwitch();
    delay(500); // Adjust the delay as needed
}