
#ifndef SWRLED_H_
#define SWRLED_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SWRLED_01 48
//#define SWRLED_02 45
//#define SWRLED_03 0 Kommentert ut fordi denne inputen er reservert til knapp
#define SWRLED_04 15
#define SWRLED_05 7
#define SWRLED_06 6
#define SWRLED_07 38
#define SWRLED_08 39
#define SWRLED_09 40
#define SWRLED_10 41


void initSWRDisplay();
void setSWRLeds(uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* ROTSWITCH_H_ */
