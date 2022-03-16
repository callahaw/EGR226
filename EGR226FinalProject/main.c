/*****************
 * Wade Callahan
 * Prof. Ekin
 * EGR 226 Final Project
 * In this project, the MSP432 MCU will be used to navigate through
 * multiple menus and alter the state of a Servo motor, DC motor, and RGB LEDs.
 * The user may open and close a door using the servo. They may change the speed
 * of a motor. and they may change the color and brightness of an led. GPIO interupts
 * are used to toggle the LED as well as serve as an emergency stop for the motor.
 ****************/

#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "LCD_Library.h"
#include "SysTick_Library.h"
#include "MenuLib.h"

void port5IRQInit();

int main(void)
{
    // disable watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    SysTick_delay_ms(6000);


    lcdInit();
    LCDBrightnessInit();
    port5IRQInit();

    SysTickInit_NoInterrupts();

    NVIC_EnableIRQ(PORT5_IRQn);  // Enable ISR Port1
    __enable_interrupt();        // Turn on all ISRs for processing

    while(1){
        navigateMenu();
    }
}



/*
 * This function initializes the interupt for Port 5.
 * Button 1 is used for the LED and Button 2 is the emergency
 * stop for the motor
 */
void port5IRQInit(){
    //button 1
    P5->SEL0    &=  ~BIT4;
    P5->SEL1    &=  ~BIT4;
    P5->DIR     &=  ~BIT4;                          //direction is input (0)
    P5->REN     |=  BIT4;                           //Enable resistor
    P5->OUT     |=  BIT4;                           //set default state to a 1
    //button 2
    P5->SEL0    &=  ~BIT5;
    P5->SEL1    &=  ~BIT5;
    P5->DIR     &=  ~BIT5;                          //direction is input (0)
    P5->REN     |=  BIT5;                           //enable resistor
    P5->OUT     |=  BIT5;                           //set default state to a 1

    //Interupt setup
    P2->IES     |=  BIT5|BIT4;
    P2->IE      |=  BIT5|BIT4;
    P2->IFG = 0;                                //clears IFG

}
/*
 * This interupt handler toggles an LED. It also serves
 * as an emergency stop for the motor
 */
void PORT5_IRQHandler()
{
    if(P5->IFG & BIT4){
        SysTick_delay_ms(5);
        P3->OUT   ^= BIT6;
        P5->IFG &= ~BIT4;
    }
    if(P5->IFG & BIT5){
        SysTick_delay_ms(5);
        TIMER_A1->CCR[3]  =  0;
        P5->IFG &= ~BIT4;
    }
    P5->IFG = 0;
}

