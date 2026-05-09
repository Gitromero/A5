#include "DAC.h"


void DAC_Init(void)
{
    /* ---------- Enable clocks ---------- */
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   // for CS (PA4)
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;   // Bpins

    /* ---------- GPIO CONFIG ---------- */
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL4);
    GPIOA->AFR[0] |= (5 << GPIO_AFRL_AFSEL4_Pos);
    /* PA4 = CS (GPIO output) NSS*/
    GPIOA->MODER &= ~(GPIO_MODER_MODE4);
    GPIOA->MODER |=  (0x2 << 8);   // output
//    DAC_CS_HIGH();
    // idle high



    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL3 | GPIO_AFRL_AFSEL5);
    GPIOB->AFR[0] |=  (5 << GPIO_AFRL_AFSEL3_Pos) |
                      (5 << GPIO_AFRL_AFSEL5_Pos);
    /* PB3 = SPI1_SCK, PB5 = SPI1_MOSI (AF5) */
    GPIOB->MODER &= ~(GPIO_MODER_MODE3 | GPIO_MODER_MODE5);
    GPIOB->MODER |=  (GPIO_MODER_MODE3_1 | GPIO_MODER_MODE5_1);



    /* Optional but good practice: high speed */
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3 | GPIO_OSPEEDR_OSPEED5);

    /* ---------- SPI1 CONFIG ---------- */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    SPI1->CR1 = 0;
    SPI1->CR2 = 0;

    SPI1->CR1 =
        SPI_CR1_MSTR |   // master mode
        SPI_CR1_SSM  |   // software slave management
        SPI_CR1_SSI;     // internal NSS = high (prevents MODF)

    /* SPI mode 0: CPOL=0, CPHA=0 (default) */
    /* MSB first (default) */
    /* Baud rate default = fPCLK/2 (can slow later if needed) */

    SPI1->CR2 =
        (0xF << SPI_CR2_DS_Pos | SPI_CR2_NSSP | SPI_CR2_SSOE);   // 16-bit data frame

    SPI1->CR1 |= SPI_CR1_SPE;      // enable SPI
}
/* Convert millivolts to 12-bit DAC code */
uint16_t MVtoDAC(uint16_t voltage_mv)
{
    if (voltage_mv > VREF_MV)
        voltage_mv = VREF_MV;

    return (uint16_t)(((uint32_t)voltage_mv * DAC_MAX_VALUE) / VREF_MV);
}

/* Build MCP4921 command word */
uint16_t sendValue(uint16_t voltage_mv)
{
    uint16_t dac = MVtoDAC(voltage_mv);

    /* 0b0111 = buffer off, gain=1x, active */
    return (0x7000 | dac);
}

void DAC_Write(uint16_t voltage_mv)
{
    uint16_t frame = sendValue(voltage_mv);

    while (!(SPI1->SR & SPI_SR_TXE));
    *(__IO uint16_t *)&SPI1->DR = frame;

//    while (SPI1->SR & SPI_SR_BSY);


    // Flush RX FIFO
    while (SPI1->SR & SPI_SR_RXNE) {
        volatile uint16_t dummy = SPI1->DR;
        (void)dummy;

    }

}