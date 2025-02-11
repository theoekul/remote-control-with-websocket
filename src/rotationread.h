#ifndef ROTREAD_H_
#define ROTREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ROTR_01 8
#define ROTR_02 3
#define ROTR_03 46
#define ROTR_04 9
#define ROTR_05 10
#define ROTR_06 11
#define ROTR_07 12
#define ROTR_08 13

#define LED_1 7
#define LED_2 10
#define LED_3 11
#define LED_4 12
#define LED_5 13
#define LED_6 14
#define LED_7 15
#define LED_8 16
#define LED_9 17



void initRotRead();
void initreadRotarySwitch();
void initupdateLEDsBasedOnPosition();
#ifdef __cplusplus
}
#endif

#endif /* ROTSWITCH_H_ */