#ifndef SRC_DAC_H_
#define SRC_DAC_H_
#include "stm32l476xx.h"


#define DAC_CS_PIN 4         // PA4 for example
#define DAC_CS_LOW()  (GPIOA->BSRR = (1U << (DAC_CS_PIN + 16))) // reset pin (low)
#define DAC_CS_HIGH() (GPIOA->BSRR = (1U << DAC_CS_PIN))        // set pin (high)
#define DAC_MAX_VALUE 4095   // 12-bit DAC
#define VREF_MV      3300   // Maximum voltage in millivolts


void DAC_Init();
void DAC_Write(uint16_t voltage);
uint16_t MVtoDAC(uint16_t voltage);
uint16_t sendValue(uint16_t voltage);


#endif /* SRC_DAC_H_ */