#include <Arduino.h> //Denne inkluderer arduino biblioteket
#include "swr_led.h" //Denne inkluderer header fila til swr_led

void initSWRDisplay() { //Denne setter SWR displayet til output
    pinMode(SWRLED_01,      OUTPUT);
    pinMode(SWRLED_02,      OUTPUT);
    pinMode(SWRLED_03,      OUTPUT); //Denne inputen er reservert til knapp
    pinMode(SWRLED_04,      OUTPUT);
    pinMode(SWRLED_05,      OUTPUT);
    pinMode(SWRLED_06,      OUTPUT);
    pinMode(SWRLED_07,      OUTPUT);
    pinMode(SWRLED_08,      OUTPUT);
}