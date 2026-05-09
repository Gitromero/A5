#include "main.h"
/* Function Prototypes -----------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void TIM2_IRQHandler(void);
int main(void)
{
		HAL_Init();
	    SystemClock_Config();
	    MX_GPIO_Init();
}
static void MX_GPIO_Init(void)
{
   // 1) Enable clock to GPIOA (LED pin bank)  [Lecture 2]
   RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
   GPIOA->MODER &= ~(GPIO_MODER_MODE8);
   GPIOA->MODER |=  (GPIO_MODER_MODE8_0);
   // Optional: push-pull, low speed, no pull (defaults are OK for LED)
   GPIOA->OTYPER &= ~(1UL << 8);
   GPIOA->OSPEEDR &= ~(0x3UL << (8 * 2));
   GPIOA->PUPDR   &= ~(0x3UL << (8 * 2));
   // 3) Enable clock to TIM2  [Lecture 7: general purpose timer]
   RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
   // 4) Configure TIM2 for 5 kHz, 25% duty using 4 MHz clock
   TIM2->PSC = 0;          // Prescaler = 0 -> timer clock = 4 MHz
   TIM2->ARR = 15999;        // Period = 800 ticks -> 200 us -> 5 kHz
   TIM2->CCR1 = 4000;       // Compare at 200 ticks -> 50 us -> 25% duty
   // Enable update interrupt (UIF) and CC1 compare interrupt (CC1IF)
   TIM2->DIER |= TIM_DIER_UIE | TIM_DIER_CC1IE;
   // 5) Enable TIM2 interrupt in NVIC  [Lecture 6: enabling interrupts]
   NVIC_EnableIRQ(TIM2_IRQn);
   // 6) Start TIM2 in upcount mode
   TIM2->CR1 |= TIM_CR1_CEN;
}
// 7) TIM2 Interrupt Service Routine
void TIM2_IRQHandler(void)
{
   // If update event occurred (counter reached ARR and rolled over)
   if (TIM2->SR & TIM_SR_UIF)
   {
       // Turn LED ON at start of each period
       GPIOA->BSRR = (1UL << 8);      // set PA5 high
       TIM2->SR &= ~TIM_SR_UIF;       // clear Update Interrupt Flag
   }
   // If compare match on CCR1 occurred
   if (TIM2->SR & TIM_SR_CC1IF)
   {
       // Turn LED OFF at 25% of the period
       GPIOA->BRR = (1UL << 8);       // reset PA5 low
       TIM2->SR &= ~TIM_SR_CC1IF;     // clear CC1 Interrupt Flag
   }
}
/* System Clock Configuration -----------------------------------------------*/
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 7;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
}
