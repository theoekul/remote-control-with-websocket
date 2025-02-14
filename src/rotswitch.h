#ifndef ROTSWITCH_H_
#define ROTSWITCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ROTSW_01 8
#define ROTSW_02 3
#define ROTSW_03 46
#define ROTSW_04 9
#define ROTSW_05 10
#define ROTSW_06 11
#define ROTSW_07 12
#define ROTSW_08 13

void initRotarySwitch();
uint8_t readRotarySwitch();

#ifdef __cplusplus
}
#endif

#endif /* ROTSWITCH_H_ */