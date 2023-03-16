
#include "stm32f4xx.h"

#include "CK_SYSTEM.h"
#include "CK_TIME_HAL.h"
#include "CK_GPIO.h"


// This code generates pwm from GPIOA2.
// PWM is both connected to an external led and also to GPIOC0
// to trigger at rising edge of the input pwm.

int main(void){

	CK_SYSTEM_SetSystemClock(SYSTEM_CLK_180MHz);

	HAL_Init();

	CK_GPIOx_ClockEnable(GPIOG);
	CK_GPIOx_Init(GPIOG, 14, CK_GPIO_OUTPUT, CK_GPIO_NOAF, CK_GPIO_PUSHPULL, CK_GPIO_VERYHIGH, CK_GPIO_NOPUPD);


    // PA2 is TIM2 Channel 3
	uint32_t frequency = 1; // Default is set to 10Hz.

	// TIM2 Enable
    RCC->APB1ENR |= 1u << 0;

    CK_GPIOx_ClockEnable(GPIOA);

    CK_GPIOx_Init(GPIOA, 2, CK_GPIO_AF, CK_GPIO_AF1, CK_GPIO_PUSHPULL, CK_GPIO_VERYHIGH, CK_GPIO_NOPUPD);

    uint32_t frequency_pclk2 = HAL_RCC_GetPCLK2Freq();

    // Main clock prescalar
    TIM2->PSC = 7;

    // Freq. is fix for each ch on same timer
    TIM2->ARR = ( (frequency_pclk2 / (TIM2->PSC+1) ) / frequency ) - 1;

    int percentage = 50;  // Duty cycle 0 to 100
    TIM2->CCR3 = (TIM2->ARR * percentage) / 100 ;

    //TIM2 CH3
    TIM2->CCMR2 |= (6u << 4) | (1u << 3); // CH3 PWM Mode 1 upcounting, CH3 Preload Enable Bit 3,
    TIM2->CCER  |= (1u << 8);             //CH3 Enable

    // CR1 default Edge Alligned, Upcounting
    TIM2->CR1 |= (1u << 7); // Auto-reload preload enable, Update request source

    TIM2->EGR |= 1u << 0; // Update generation enabled

    //TIM2->CR1 |= (1u << 2); // Auto-reload preload enable, Update request source
    //TIM2->DIER |= 1u << 0; // Update interrupt enable
    //NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= 1u << 0; // Counter enable



	// PC0 is will be used as external interrupt
	RCC->APB2ENR |= 1u << 14; // SYSCFG Clock Enable

	CK_GPIOx_ClockEnable(GPIOC);
	CK_GPIOx_Init(GPIOC, 0, CK_GPIO_INPUT, CK_GPIO_NOAF,CK_GPIO_PUSHPULL,CK_GPIO_VERYHIGH,CK_GPIO_PULLDOWN);

	SYSCFG->EXTICR[0] |= (2u << 0); // External interrupt 0 is to GPIOC Pin0

	EXTI->IMR  |= (1u<<0);	// Line 0 Interrupt Mask Request
	EXTI->EMR  |= (1u<<0);	// Line 0 Event Mask Request
	EXTI->RTSR |= (1u<<0);	// Rising edge detection for each line
	//EXTI->FTSR |= (1u<<0);	// Falling edge detection for each line

	NVIC_EnableIRQ(EXTI0_IRQn);


	while(1){



	}

}

/*void TIM2_IRQHandler(){

	TIM2->SR = 0;

	CK_GPIOx_TogglePin(GPIOG, 14);

}*/

void EXTI0_IRQHandler(void){

	CK_GPIOx_TogglePin(GPIOG, 14);

	EXTI->PR |= 1u << 0; // Clear Interrupt
}










