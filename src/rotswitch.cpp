
#include <Arduino.h>
#include "rotswitch.h"

void initRotarySwitch() {
    pinMode(ROTSW_01,      INPUT);
    pinMode(ROTSW_02,      INPUT);
    pinMode(ROTSW_03,      INPUT);
    pinMode(ROTSW_04,      INPUT);
    pinMode(ROTSW_05,      INPUT);
    pinMode(ROTSW_06,      INPUT);
    pinMode(ROTSW_07,      INPUT);
    pinMode(ROTSW_08,      INPUT);
}


uint8_t readRotarySwitch() {
    uint8_t position = 0;
    if (digitalRead(ROTSW_01) == HIGH) position = 1;
    else if (digitalRead(ROTSW_02) == HIGH) position = 2;
    else if (digitalRead(ROTSW_03) == HIGH) position = 3;
    else if (digitalRead(ROTSW_04) == HIGH) position = 4;
    else if (digitalRead(ROTSW_05) == HIGH) position = 5;
    else if (digitalRead(ROTSW_06) == HIGH) position = 6;
    else if (digitalRead(ROTSW_07) == HIGH) position = 7;
    else if (digitalRead(ROTSW_08) == HIGH) position = 8;
    else position = 0;
}