
#include <Arduino.h> //Denne inkluderer arduino biblioteket
#include "swr_led.h" //Denne inkluderer header fila til swr_led

void initSWRDisplay() { //Denne setter SWR displayet til output
    pinMode(SWRLED_01,      OUTPUT);
    //pinMode(SWRLED_02,      OUTPUT);
    //pinMode(SWRLED_03,      OUTPUT); //Denne inputen er reservert til knapp
    pinMode(SWRLED_04,      OUTPUT);
    pinMode(SWRLED_05,      OUTPUT);
    pinMode(SWRLED_06,      OUTPUT);
    pinMode(SWRLED_07,      OUTPUT);
    pinMode(SWRLED_08,      OUTPUT);
    pinMode(SWRLED_09,      OUTPUT);
    pinMode(SWRLED_10,      OUTPUT);    
}

//hvis over  resistans skal led 1 lyse
void setSWRLeds(uint8_t value){
    if (value > 0) { digitalWrite(SWRLED_01, LOW);} else { digitalWrite(SWRLED_01, HIGH);}
    //if (value > 1) { digitalWrite(SWRLED_02, LOW);} else { digitalWrite(SWRLED_02, HIGH);}
    //if (value > 2) { digitalWrite(SWRLED_03, LOW);} else { digitalWrite(SWRLED_03, HIGH);}
    if (value > 3) { digitalWrite(SWRLED_04, LOW);} else { digitalWrite(SWRLED_04, HIGH);}
    if (value > 4) { digitalWrite(SWRLED_05, LOW);} else { digitalWrite(SWRLED_05, HIGH);}
    if (value > 5) { digitalWrite(SWRLED_06, LOW);} else { digitalWrite(SWRLED_06, HIGH);}
    if (value > 6) { digitalWrite(SWRLED_07, LOW);} else { digitalWrite(SWRLED_07, HIGH);}
    if (value > 7) { digitalWrite(SWRLED_08, LOW);} else { digitalWrite(SWRLED_08, HIGH);}
    if (value > 8) { digitalWrite(SWRLED_09, LOW);} else { digitalWrite(SWRLED_09, HIGH);}
    if (value > 9) { digitalWrite(SWRLED_10, LOW);} else { digitalWrite(SWRLED_10, HIGH);}
}