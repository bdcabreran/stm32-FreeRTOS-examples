#include <ledImplementation.h>
#include <stm32f1xx_hal.h>
#include "main.h"

/**
 * LED implementation for discrete LED's
 */

void GreenOn ( void ) {HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);}
void GreenOff ( void ) {HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);}
iLed GreenLed = { GreenOn, GreenOff };

void BlueOn ( void ) {HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);}
void BlueOff ( void ) {HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);}
iLed BlueLed = { BlueOn, BlueOff };

void RedOn ( void ) {HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);}
void RedOff ( void ) {HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);}
iLed RedLed = { RedOn, RedOff };

void YellowOn ( void ) {HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);}
void YellowOff ( void ) {HAL_GPIO_WritePin(LD4_GPIO_Port   , LD4_Pin, GPIO_PIN_SET);}
iLed YellowLed = { YellowOn, YellowOff };
