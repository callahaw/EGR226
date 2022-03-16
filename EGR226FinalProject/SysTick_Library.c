/**************************************************************************************
*                                SysTick_Library.c
*                           Trevor Ekin / Nabeeh Kandalaft
*                         EGR226      Date: March, 6, 2019
*
*  This is a library for the SysTick Timer Peripheral on the MSP432.
*
*  All functions are briefly described in their comment blocks.  The /// notation makes
*  it so the function description block is visible when you hovering over a function call
*  in any file (this feature is called Intellisense).
*
*
* **************************************************************************************/
#include "SysTick_Library.h"
#include "msp.h"

/// ****| SysTickInit_NoInterrupts | ****************//*
/// * Brief: Initialize the SysTick peripheral for use
/// *          without interrupts (busy-waits)
/// * param:
/// *      N/A
/// * return:
/// *      N/A
/// ****************************************************/
void SysTickInit_NoInterrupts(void){
    SysTick->CTRL  &= ~BIT0;                             //clears enable to stop the counter
    SysTick->LOAD   = 0x00FFFFFF;                        //sets the period... note: (3006600/1000 - 1) = 1ms
    SysTick->VAL    = 0;                                 //clears the value
    SysTick->CTRL   = (STCSR_CLKSRC | STCSR_EN);         //enable SysTick with core clock, no interrupts -> this is the ENABLE and CLKSOURSE bits: Systic->CTRL |= 0x05;
}

/// ****| SysTickInit_WithInterrupts | **************//*
/// * Brief: Initialize the SysTick peripheral for use
/// *          with interrupts (interrupt delays)
/// * param:
/// *      (uint32_t) ms_delay:  number of milliseconds
/// *                              to delay
/// * return:
/// *      N/A
/// ****************************************************/
void SysTickInit_WithInterrupts(uint32_t delay_ms){
    SysTick->CTRL  &= ~BIT0;                                             //clears enable to stop the counter
    SysTick->LOAD   = delay_ms * 3000;                                   //sets the period
    SysTick->VAL    = 0;                                                 //clears the value
    SysTick->CTRL   = (STCSR_CLKSRC | STCSR_INT_EN | STCSR_EN);          //enable SysTick with core clock, interrupts on -> this is the ENABLE, TICKINT, and CLKSOURSE bits: Systic->CTRL |= 0x07;
}

/// ****| SysTick_delay_ms | *************************//*
/// * Brief: Use the SysTick timer to delay a specified
/// *          number of milliseconds
/// * param:
/// *      (uint32_t) ms_delay:  number of milliseconds
/// *                              to delay
/// * return:
/// *      N/A
/// ****************************************************/
void SysTick_delay_ms(uint32_t ms_delay){
    //Delays time_ms number of milliseconds
    //Assume 3MHz clock -> 3000 cycles per millisecond
    SysTick->LOAD  = 3000 * (uint32_t)ms_delay;
    SysTick->VAL   = 0;                               // starts counting from 0
    SysTick->CTRL |= (STCSR_CLKSRC | STCSR_EN);       // ENABLE, CLKSOURSE bits  .... Systic->CTRL |= 0x05;
    while(!(SysTick->CTRL & ((uint32_t)1)<<16));      // Continue while bit 16 is high or use   ....while( (SysTick->CTRL & BIT16) == 0);
    SysTick->CTRL &= ~(STCSR_CLKSRC | STCSR_EN);      // Disable the Systic timer               .... Systic->CTRL =0 ;
}

/// ****| SysTick_delay_us | ************************//*
/// * Brief: Use the SysTick timer to delay a specified
/// *          number of microseconds
/// * param:
/// *      (uint32_t) us_delay:  number of microseconds
/// *                              to delay
/// * return:
/// *      N/A
/// ****************************************************/
void SysTick_delay_us(uint32_t us_delay){
    //Delays time_ms number of milliseconds
    //Assume 3MHz clock -> 3 cycles per microsecond
    SysTick->LOAD  = us_delay*3 - 1;                 //counts up to delay
    SysTick->VAL   = 0;                              //starts counting from 0
    SysTick->CTRL |= (STCSR_CLKSRC | STCSR_EN);      // ENABLE, CLKSOURSE bits  .... Systic->CTRL |= 0x05;
    while(!(SysTick->CTRL & ((uint32_t)1)<<16));     // Continue while bit 16 is high   .... while( (SysTick->CTRL & BIT16) == 0);
    SysTick->CTRL &= ~(STCSR_CLKSRC | STCSR_EN);     // Disable the Systic timer        .... Systic->CTRL =0 ;
}

/// ****| SysTick_Handler | **************************//*
/// * Brief: SysTick Handler (rewrite for desired use)
/// *
/// * param:
/// *      N/A
/// * return:
/// *      N/A
/// ****************************************************/
void SysTick_Handler(void) {
    _flag = 1;
}
