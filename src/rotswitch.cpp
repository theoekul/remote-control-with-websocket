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