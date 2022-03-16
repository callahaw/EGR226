/*
*This library contains all the functions for the menus.
 */

#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include "LCD_Library.h"
#include "SysTick_Library.h"
#include "MenuLib.h"

#define ASTERIK 19
#define POUND 21
#define START 657

//Global variable declaration
int period = 60000 / 8;
int lightsPeriod = 60000 / 8;
int keyPressed = (-1);

enum menuStates navigation = Main;
/*
 * This function navigate through the entire program.
 */
void navigateMenu(){
    switch(navigation){
    case Main:
        mainMenu();
        break;
    case Door:
        setServo();
        break;
    case Motor:
        setMotor();
        break;
    case Lights:
        updateColor();
        updateBrightness();
        break;
    }
}
/**************************Main Menu***************************************
 * The main menu function first prompts the user to input a 1,2, or 3.
 * This function will continously read from the keypad until valid entry
 * If the user enters:
 * 1)The door menu will appear
 * 2)The Motor menu will appear
 * 3)The Lights menu will appear
 ***************************************************************************/
void mainMenu(){
    lcdClear();
    lcdSetText("Select an option",0,0);
    lcdSetText("1: Door Menu",0,1);
    lcdSetText("2: Motor Menu",0,2);
    lcdSetText("3: Lights Menu",0,3);
    SysTick_delay_ms(5);
    while(navigation == Main){
        //Checks ADC Reading continously
        ADC14->CTL0 |= 1;                                               //start conversion
        while(!ADC14->IFGR0);                                           //Wait for conversion to complete
        TIMER_A0->CCR[4] = ADC14->MEM[0]/16384.0*3000;                  //Changes the brightness of the LCD

        readKeypad();
        if(keyPressed == 1){
            navigation = Door;
        }
        if(keyPressed == 2){
            navigation = Door;
        }
        if(keyPressed == 3){
            navigation = Lights;
        }
    }
    keyPressed = -1;
    SysTick_delay_ms(5);
}

/****
 * This function prompts the user to input information for the motor
 * The user may input a number 0-9 and this corresponds to the dutycycle
 * for the motor. There is also error checking for asteriks and pound signs
 * Returns to main menu following proper input.
 ****/
void setMotor(){
    //LCD Display update for motor menu
    lcdClear();
    lcdSetText("Motor speed:",0,0);
    lcdSetText("Enter 0 - 9:",0,1);

    while(keyPressed == (-1)){
        readKeypad();

        //Changes the PWM Signal based on the reading from the keypad, includes error checking
        if(keyPressed == 1)
            pwmMotor(period, (period * 10)/100);                            //sets up new PWM
        if(keyPressed == 2)
            pwmMotor(period, (period * 20)/100);                            //sets up new PWM
        if(keyPressed == 3)
            pwmMotor(period, (period * 30)/100);                            //sets up new PWM
        if(keyPressed == 4)
            pwmMotor(period, (period * 40)/100);                            //sets up new PWM
        if(keyPressed == 5)
            pwmMotor(period, (period * 50)/100);                            //sets up new PWM
        if(keyPressed == 6)
            pwmMotor(period, (period * 60)/100);                            //sets up new PWM
        if(keyPressed == 7)
            pwmMotor(period, (period * 70)/100);                            //sets up new PWM
        if(keyPressed == 8)
            pwmMotor(period, (period * 80)/100);                            //sets up new PWM
        if(keyPressed == 9)
            pwmMotor(period, (period * 90)/100);                            //sets up new PWM
        if(keyPressed == 0)
            pwmMotor(period, (period * 0)/100);                             //sets up new PWM
        if(keyPressed == ASTERIK || keyPressed == POUND)                    //Error checking, can't put in asterik or pound
            keyPressed = -1;
    }

    keyPressed = -1;                                                        //resets keypressed
    navigation = Main;                                                      //return to main menu
    SysTick_delay_ms(5);
}
/*****
 * This function initilizes and changes the PWM Signal at P7.5
 * The function loads a period of 60000 cycles which is equivalent
 * to 50 Hz in a 3MHz system
 *****/
void pwmMotor(uint16_t period, uint16_t periodON){
    P7->SEL0            |=  BIT5;                                                       //GPIO for PWM
    P7->SEL1            &=  ~BIT5;
    P7->DIR             =   BIT5;                                                       //OUTPUt

    TIMER_A1->CTL       =   TIMER_A_CTL_SSEL__SMCLK |                                   // SMCLK
                            TIMER_A_CTL_ID_3        |                                   // divide by 8
                            TIMER_A_CTL_MC_1;                                           // Up mode

    //LOADS Timer
    TIMER_A1->CCR[0]    = period;
    TIMER_A1->CCTL[3]   = 0b11100000;                                                   //reset/set
    TIMER_A1->CCR[3]    = periodON;
}

/****
 * This function prompts the user to input information for the motor
 * The user may input a number 0-9 and this corresponds to the dutycycle
 * for the motor. There is also error checking for asteriks and pound signs
 * Returns to main menu following proper input.
 ****/
void setServo(){
    //LCD Display update for motor menu
    lcdClear();
    lcdSetText("Door Menu:",0,0);
    lcdSetText("1: Open Door",0,1);
    lcdSetText("2: Close Door",0,2);

    while(navigation == Door){
        readKeypad();
        //Changes the PWM Signal based on the reading from the keypad, includes error checking
        if(keyPressed == 1){
            pwmServo(period, (period * 1)/100);                             //sets up new PWM
            P6->OUT     |=  BIT5;                                           //Change Green LED on
            P6->OUT     &=  ~BIT4;                                          //Change Red LED off
            navigation = Main;
        }
        if(keyPressed == 2){
            pwmServo(period, (period * 2.5)/100);                            //sets up new PWM
            P6->OUT     |=  BIT4;                                            //change Red LED on
            P6->OUT     &=  ~BIT5;                                           //change green LED off
            navigation = Main;
        }
    }

    keyPressed = -1;                                                        //resets keypressed                                                     //return to main menu
    SysTick_delay_ms(5);
}
/*****
 * This function Initializes and changes the PWM Signal at P7.4
 * The function loads a period of 60000 cycles which is equivalent
 * to 50 Hz in a 3MHz system
 *****/
void pwmServo(uint16_t period, uint16_t periodON){
    P7->SEL0            |=  BIT4;                                                       //GPIO for PWM
    P7->SEL1            &=  ~BIT4;
    P7->DIR             =   BIT4;                                                       //OUTPUt

    TIMER_A1->CTL       =   TIMER_A_CTL_SSEL__SMCLK |                                   // SMCLK
                            TIMER_A_CTL_ID_3        |                                   // divide by 8
                            TIMER_A_CTL_MC_1;                                           // Up mode

    //LOADS Timer
    TIMER_A1->CCR[0]    = period;
    TIMER_A1->CCTL[4]   = 0b11100000;                                                   //reset/set
    TIMER_A1->CCR[4]    = periodON;
}
/********************
 * This function will prompt the user to choose which color
 * they want to be displayed. The function contiously checks the
 * keyPad until a valid entry 1-3 is input. The function also intializes
 * PWM Signals for the LEDs
 ********************/
void updateColor(){
                //Initializes all three of the PWM signals for each LED
                P2->SEL0            |=  (BIT4|BIT5|BIT6);                                                       //GPIO for PWM
                P2->SEL1            &=  ~(BIT4|BIT5|BIT6);
                P2->DIR             =   (BIT4|BIT5|BIT6);                                                       //OUTPUt

                TIMER_A0->CTL       =   TIMER_A_CTL_SSEL__SMCLK |                                   // SMCLK
                                        TIMER_A_CTL_ID_3        |                                   // divide by 8
                                        TIMER_A_CTL_MC_1;                                           // Up mode

                //LOADS Timer
                TIMER_A0->CCR[0]    = period;
                TIMER_A0->CCTL[4]   = 0b11100000;                                                   //reset/set
                TIMER_A0->CCTL[4]   = 0b11100000;                                                   //reset/set
                TIMER_A0->CCTL[4]   = 0b11100000;                                                   //reset/set

    lcdClear();
    lcdSetText("Light Menu:",0,0);
    lcdSetText("1: Green",0,1);
    lcdSetText("2: Blue",0,2);
    lcdSetText("3: Red",0,3);
    while(keyPressed == (-1)){
        readKeypad();
        //Green LED
        if(keyPressed == 1){
            P2->OUT   &= ~BIT6;
            P2->OUT   &= ~BIT5;
            P2->OUT   |= BIT4;
        }
        //Blue LED
        if(keyPressed == 2){
            P2->OUT   &= ~BIT6;
            P2->OUT   &= ~BIT4;
            P2->OUT   |= BIT5;
        }
        //Red LED
        if(keyPressed == 3){
            P2->OUT   &= ~BIT5;
            P2->OUT   &= ~BIT4;
            P2->OUT   |= BIT6;
        }
        else
            keyPressed = (-1);                                                          //resets keypressed if anything else is pressed
        }
    }
/**********
 * This function adjusts the brightness of the LED.
 * First, the program will check which light is on.
 * Next, the program will continously read the keypad until
 * a valid entry is input.The program will return to the main
 * menu following proper input
************* */
void updateBrightness(){
    lcdClear();
    lcdSetText("Light Menu:",0,0);
    lcdSetText("Enter Brightness",0,1);
    lcdSetText("0-100:",0,2);
        if(keyPressed == 1){
        keyPressed = -1;
        while(navigation == Lights){
            readKeypad();
            if(keyPressed == 1){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 10)/100;
                navigation = Main;
                }
            if(keyPressed == 2){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 20)/100;
                navigation = Main;
                }
            if(keyPressed == 3){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 30)/100;
                navigation = Main;
                }
            if(keyPressed == 4){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 40)/100;
                navigation = Main;
                }
            if(keyPressed == 5){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 50)/100;
                navigation = Main;
                }
            if(keyPressed == 6){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 60)/100;
                navigation = Main;
                }
            if(keyPressed == 7){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 70)/100;
                navigation = Main;
                }
            if(keyPressed == 8){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 80)/100;
                navigation = Main;
                }
            if(keyPressed == 9){
                TIMER_A0->CCR[1]  =  (lightsPeriod * 90)/100;
                navigation = Main;
                }

            }
        }
        if(keyPressed == 2){
        keyPressed = -1;
        while(navigation == Lights){
            readKeypad();
            if(keyPressed == 1){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 10)/100;
                navigation = Main;
                }
            if(keyPressed == 2){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 20)/100;
                navigation = Main;
                }
            if(keyPressed == 3){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 30)/100;
                navigation = Main;
                }
            if(keyPressed == 4){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 40)/100;
                navigation = Main;
                }
            if(keyPressed == 5){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 50)/100;
                navigation = Main;
                }
            if(keyPressed == 6){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 60)/100;
                navigation = Main;
                }
            if(keyPressed == 7){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 70)/100;
                navigation = Main;
                }
            if(keyPressed == 8){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 80)/100;
                navigation = Main;
                }
            if(keyPressed == 9){
                TIMER_A0->CCR[2]  =  (lightsPeriod * 90)/100;
                navigation = Main;
                }

            }
        }
        if(keyPressed == 3){
        keyPressed = -1;
        while(navigation == Lights){
            readKeypad();
            if(keyPressed == 1){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 10)/100;
                navigation = Main;
                }
            if(keyPressed == 2){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 20)/100;
                navigation = Main;
                }
            if(keyPressed == 3){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 30)/100;
                navigation = Main;
                }
            if(keyPressed == 4){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 40)/100;
                navigation = Main;
                }
            if(keyPressed == 5){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 50)/100;
                navigation = Main;
                }
            if(keyPressed == 6){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 60)/100;
                navigation = Main;
                }
            if(keyPressed == 7){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 70)/100;
                navigation = Main;
                }
            if(keyPressed == 8){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 80)/100;
                navigation = Main;
                }
            if(keyPressed == 9){
                TIMER_A0->CCR[3]  =  (lightsPeriod * 90)/100;
                navigation = Main;
                }
            }
        }
}

/**********
 * This function is used to debounce keypad entries. It first detects
 * if a key is pressed, waits a short bit of time, and checks the key
 * again
 *********/
int button_Debounce(uint16_t delayTime, int bit){
    if(!(P4->IN & bit)){                                   //checks if the button is pressed
            SysTick_delay_ms(delayTime);                       //delay the time
                if(!(P4->IN & bit)){                        //Checks if button is pressed again
                        return 1;                           //returns one if button was pressed
                }
    }
    return 0;                                               //returns 0 if button is not pressed
}
/*******
 * This function read the keypad. If a key is pressed, it sets
 * the keyPressed global variable to the correct number.
 *******/
void readKeypad()
{
      int bit = 0b00000001;                                             //used for cycling through P4.0-4.4
      uint16_t debounceDelay = 3;                                       //debounce delay

      //COLUMN 1 KEYS
      P4->OUT   &= ~BIT4;                                               //Set default state to a 0
      P4->DIR   |=  BIT4;                                               //Change to an output

      while(button_Debounce(debounceDelay   ,   bit))                   //Checks if key 1 is press, stays here until button is let go
         keyPressed     =   1;                                          //changes key press
      bit<<=1;                                                          //shifts bit to left 1

      while(button_Debounce(debounceDelay   ,   bit))                   //Checks p4.1
          keyPressed    =   4;
      bit<<=1;

      while(button_Debounce(debounceDelay   ,   bit))                   //checks p4.2
          keyPressed    =   7;
      bit<<=1;

      while(button_Debounce(debounceDelay   ,   bit))                   //checks p4.3
         keyPressed     =   ASTERIK;
      P4->DIR   &= ~BIT4;                                               //Change to an input
      P4->OUT   |=  BIT4;                                               //Set default state to a 1
      bit = 0b00000001;

      //COLUMN 2 KEYS
      P4->OUT   &= ~BIT5;                   //Set default state to a 0
      P4->DIR   |=  BIT5;                   //Change to an output (that is 0 due to previous line)

      while(button_Debounce(debounceDelay   ,   bit))
          keyPressed    =   2;
      bit<<=1;

      while(button_Debounce(debounceDelay   ,   bit))
          keyPressed    =   5;
      bit<<=1;

      while(button_Debounce(debounceDelay   ,   bit))
          keyPressed    =   8;
      bit<<=1;

      while(button_Debounce(debounceDelay   ,   bit))
          keyPressed    =   0;

      P4->DIR   &= ~BIT5;                   //Change to an input
      P4->OUT   |=  BIT5;                   //Set default state to a 1 (pull up)
      bit = 0b00000001;

      //COLUMN 3 KEYS
      P4->OUT   &= ~BIT6;                   //Set default state to a 0
      P4->DIR   |=  BIT6;                   //Change to an output (that is 0 due to previous line)

      while(button_Debounce(debounceDelay   ,   bit))
          keyPressed    =   3;
      bit<<=1;

      while(button_Debounce(debounceDelay   ,   bit))
          keyPressed    =   6;
      bit<<=1;

      while(button_Debounce(debounceDelay   ,   bit))
          keyPressed    =   9;
      bit<<=1;

      while(button_Debounce(debounceDelay   ,   bit))
          keyPressed    =   POUND;
      P4->DIR   &= ~BIT6;                   //Change to an input
      P4->OUT   |=  BIT6;                   //Set default state to a 1 (pull up)
      bit = 0b00000001;
}
