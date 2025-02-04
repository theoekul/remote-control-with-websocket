#include <Arduino.h>
#include "swr_led.h"

void initSWRDisplay() {
    pinMode(SWRLED_01,      OUTPUT);
    pinMode(SWRLED_02,      OUTPUT);
    //pinMode(SWRLED_03,      OUTPUT);
    pinMode(SWRLED_04,      OUTPUT);
    pinMode(SWRLED_05,      OUTPUT);
    pinMode(SWRLED_06,      OUTPUT);
    pinMode(SWRLED_07,      OUTPUT);
    pinMode(SWRLED_08,      OUTPUT);
}