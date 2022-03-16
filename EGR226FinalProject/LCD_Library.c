/**************************************************************************************
 *                                LCD_Library.c
 *                                Trevor Ekin (Adapted from Dr. Nabeeh Kandalaft)
 *                         EGR226      Date: February-21-2019
 *
 *  This is a library for the 4x16 LCD.
 *
 *  All functions are briefly described in their comment blocks.  The /// notation makes
 *  it so the function description block is visible when you hover over a function call
 *  in any file (this feature is called Intellisense).
 *
 *  All pins are set with default values (see below) but they can be easily changed with
 *  in LCD_Library.h to any pin configuration (follow instructions in header file).
 *  Likely, these pins willnot work for you unless you've soldered on the additional
 *  header at the end opposing USB connection on your MSP.
 *
 *****************************  Pins   **************************************************
 *                           MSP432 PINS (Default, see LCD_Library.h to change these)
 *                      P10.2 -> LCD D4
 *                      P10.3 -> LCD D5
 *                      P10.4 -> LCD D6
 *                      P10.5 -> LCD D7
 *                      P10.1 -> LCD E
 *                      P7.6 -> LCD RS
 *                      P10.0 -> LCD RW
 * **************************************************************************************/

/***************************************************************************************
*Revision Log:
*   (v2.0) 6/15/2020: Reset Byte macros to make reset sequence a more understandable.
* **************************************************************************************/

#include <stdint.h>
#include "LCD_Library.h"
#include <stdio.h>
#include "SysTick_Library.h"

/// ****| lcdInit | *********************************//*
/// * Brief: Initialize the LCD with chosen connection
/// *          pins.  Send configuration sequence.
/// * param:
/// *      N/A
/// * return:
/// *      N/A
/// ****************************************************/
void lcdInit() {
    // Initialize all communication pins as GPIOs by clearing SEL0 and SEL1 (see LCD_Library.h for PSEL# macro definitions)
    EN_GPIO;
    RS_GPIO;
    RW_GPIO;
    D4_GPIO;
    D5_GPIO;
    D6_GPIO;
    D7_GPIO;

    // Initialize all communication pins as outputs (see LCD_Library.h for PDIR macro definitions)
    RS_DIR |= RS;
    RW_DIR |= RW;
    EN_DIR |= EN;
    D4_DIR |= D4;
    D5_DIR |= D5;
    D6_DIR |= D6;
    D7_DIR |= D7;

    // Initialize all communication pins as low (see LCD_Library.h for Bit Toggling macro definitions)
    RS_LOW;
    RW_LOW;
    EN_LOW;
    D4_LOW;
    D5_LOW;
    D6_LOW;
    D7_LOW;

    SysTick_delay_ms(100);  // Allow ample time for pin configuration to settle.

    //-------------------------------------------------------------------------------------
    // RESET COMMANDS TO INITIALIZE DEVICE: Send 4 nibbles {3, 3, 3, 2} to restart device and set it for 4-wire mode
    //RESET_BYTE_1    0x33    -- Reset sequence, part 1 and 2
    //RESET_BYTE_2    0x32    -- Reset Sequence, part 3 and 4
    //-------------------------------------------------------------------------------------
    lcdWriteCmd(RESET_BYTE_1);      SysTick_delay_ms(100);  //reset sequence, part 1
    lcdWriteCmd(RESET_BYTE_2);      SysTick_delay_ms(100);  //reset sequence, part 2


    //-------------------------------------------------------------------------------------
    // FUNCTION SET COMMAND: 0b001[DL] [N][F] * * -> DL = Data length, N = display lines, F = character font
    //FSET_000        0x20    -- 4-bit data, 1 line, 5x8 dot font
    //FSET_001        0x24    -- 4-bit data, 1 line, 5x10 dot font
    //FSET_010        0x28    -- 4-bit data, 2 lines, 5x8 dot font
    //FSET_011        0x2C    -- 4-bit data, 2 lines, 5x10 dot font
    //FSET_100        0x30    -- 8-bit data, 1 line, 5x8 dot font     (standard)
    //FSET_101        0x34    -- 8-bit data, 1 line, 5x10 dot font
    //FSET_110        0x38    -- 8-bit data, 2 lines, 5x8 dot font
    //FSET_111        0x3C    -- 8-bit data, 2 lines, 5x10 dot font
    //-------------------------------------------------------------------------------------
    lcdWriteCmd(FSET_010);      SysTick_delay_ms(100);  //reset sequence, part 3

    // HOME command is 0x02
    lcdWriteCmd(HOME);          SysTick_delay_ms(100);  //send cursor home

    //-------------------------------------------------------------------------------------
    // ENTRY MODE COMMAND: 0b0000 01[I][S] -> I = Increment, S = Shift
    //ENTRYMODE_00    0x04    // no auto increment, no display shift  (DEFAULT)
    //ENTRYMODE_01    0x05    // no auto increment, display shift
    //ENTRYMODE_10    0x06    // auto increment, no display shift
    //ENTRYMODE_11    0x07    // auto increment, display shift
    //-------------------------------------------------------------------------------------
    lcdWriteCmd(ENTRYMODE_10);  SysTick_delay_ms(100);  //set up for auto incrementing

    lcdWriteCmd(CLEAR);         SysTick_delay_ms(100);  //clear screen (again)

    //-------------------------------------------------------------------------------------
    // DISPLAY CONTORL COMMAND: 0b0000 1[D][C][B] -> D = Display, C = Cursor, B = Blinking
    //DISPLAY_000     0x08    -- display off, cursor off, blinking off    (DEFAULT)
    //DISPLAY_001     0x09    -- display off, cursor off, blinking on     (not useful)
    //DISPLAY_010     0x0A    -- display off, cursor on, blinking off     (not useful)
    //DISPLAY_011     0x0B    -- display off, cursor on, blinking on      (not useful)
    //DISPLAY_100     0x0C    -- display on, cursor off, blinking off
    //DISPLAY_101     0x0D    -- display on, cursor off, blinking on
    //DISPLAY_110     0x0E    -- display on, cursor on, blinking off
    //DISPLAY_111     0x0F    -- display on, cursor on, blinking on
    //-------------------------------------------------------------------------------------
    lcdWriteCmd(DISPLAY_111);   SysTick_delay_ms(100);  //turn on display with blinking cursor

    // CLEAR command is 0x01
    lcdWriteCmd(CLEAR);         SysTick_delay_ms(100);  //clear screen

    // reset CGRAM offset address
    _offset = 0;
}

/// ****| lcdTriggerEN | ****************************//*
/// * Brief: Pulse the enable pin to notify the LCD to
/// *          latch the current data inputs.
/// * param:
/// *      (unsigned char) data: 8-bit data to send
/// * return:
/// *      N/A
/// ****************************************************/
void lcdTriggerEN() {
    EN_HIGH;
    SysTick_delay_us(50);
    EN_LOW;
    SysTick_delay_us(50);
}

/// ****| lcdWriteData | ****************************//*
/// * Brief: Send data one nibble at a time to LCD via
/// *          SetNibble macro (see LCD_Library.h)
/// * param:
/// *      (unsigned char) data: 8-bit data to send
/// * return:
/// *      N/A
/// ****************************************************/
void lcdWriteData(unsigned char data) {
    RS_HIGH;
    RW_LOW;
    SysTick_delay_us(50);
    SetNibble(data >> 4);   // Upper nibble
    SysTick_delay_us(50);
    lcdTriggerEN();
    SetNibble(data);        // Lower nibble
    SysTick_delay_us(50);
    lcdTriggerEN();
    SysTick_delay_us(50);
    SetNibble(0x00);        // clear output
}

/// ****| lcdWriteCmd | *****************************//*
/// * Brief: Send command one nibble at a time to LCD
/// *          via SetNibble macro (see LCD_Library.h)
/// * param:
/// *      (unsigned char) cmd: 8-bit command to send
/// * return:
/// *      N/A
/// ****************************************************/
void lcdWriteCmd(unsigned char cmd) {
    RS_LOW;
    RW_LOW;
    SysTick_delay_us(50);
    SetNibble(cmd >> 4);     // Upper nibble
    SysTick_delay_us(50);
    lcdTriggerEN();
    SetNibble(cmd);          // Lower nibble
    SysTick_delay_us(50);
    lcdTriggerEN();
    SysTick_delay_us(50);
    SetNibble(0x00);        // clear output
}

/// ****| lcdSetText | ******************************//*
/// * Brief: Display character string on the LCD at the
/// *          chosen coordinates.
/// * param:
/// *      (char*) text: character string to display
/// *      (int) x:      x-coordinate
/// *      (int) y:      y-coordinate
/// * return:
/// *      N/A
/// ****************************************************/
void lcdSetText(char* text, int x, int y) {
    int i;
    if (x < 16) {
        x |= 0x80;      // Set LCD for first line write
        switch (y){
        case 0:
            x |= 0x00;  // Set LCD for first line write
            break;
        case 1:
            x |= 0x40;  // Set LCD for Second line write
            break;
        case 2:
            x |= 0x10;  // Set LCD for Third line write
            break;
        case 3:
            x |= 0x50;  // Set LCD for Fourth line write
            break;
        case 5:
            x |= 0x20;  // Set LCD for second line write reverse
            break;
        }
        lcdWriteCmd(x);
    }
    i = 0;
    while (text[i] != '\0') {
        lcdWriteData(text[i]);
        i++;
    }
}

/// ****| lcdSetChar | ******************************//*
/// * Brief: Display character on the LCD at the
/// *          chosen coordinates.
/// * param:
/// *      (char) c:     character to display (can be
/// *                       custom character if c = offset
/// *                       of custom character)
/// *      (int) x:      x-coordinate
/// *      (int) y:      y-coordinate
/// * return:
/// *      N/A
/// ****************************************************/
void lcdSetChar(char c, int x, int y) {
    if (x < 16) {
        x |= 0x80;      // Set LCD for first line write
        switch (y){
        case 0:
            x |= 0x00;  // Set LCD for first line write
            break;
        case 1:
            x |= 0x40;  // Set LCD for Second line write
            break;
        case 2:
            x |= 0x10;  // Set LCD for Third line write
            break;
        case 3:
            x |= 0x50;  // Set LCD for Fourth line write
            break;
        case 5:
            x |= 0x20;  // Set LCD for second line write reverse
            break;
        }
        lcdWriteCmd(x);
    }

    lcdWriteData(c);
}

/// ****| lcdSetInt | *******************************//*
/// * Brief: Convert integer into character string to be
/// *          displayed on LCD at chosen coordinates.
/// * param:
/// *      (int) val: value to convert to display
/// *      (int) x:   x-coordinate
/// *      (int) y:   y-coordinate
/// * return:
/// *      N/A
/// ****************************************************/
void lcdSetInt(int val, int x, int y){
    char number_string[16];
    sprintf   (number_string, "%d\0", val); // Convert the integer to character string
    lcdSetText(number_string, x, y);
}

/// ****| lcdCreateCustomChar | **********************//*
/// * Brief: Creates a custom character in CGRAM based on
/// *          character structure passed.
/// * param:
/// *      (custom_char_t)* cc: custom character struct
/// *                             to place in CGRAM
/// * return:
/// *      (uint8_t) _offset:   offset index of new
/// *                               custom char
/// ****************************************************/
uint8_t lcdCreateCustomChar(custom_char_t* cc) {
    lcdWriteCmd(CGRAM+(8*_offset));     // characters placed in intervals of 8 bytes
    lcdWriteData(cc->line0);            // send byte 0 of new character
    lcdWriteData(cc->line1);            // send byte 1 of new character
    lcdWriteData(cc->line2);            // send byte 2 of new character
    lcdWriteData(cc->line3);            // send byte 3 of new character
    lcdWriteData(cc->line4);            // send byte 4 of new character
    lcdWriteData(cc->line5);            // send byte 5 of new character
    lcdWriteData(cc->line6);            // send byte 6 of new character
    lcdWriteData(cc->line7);            // send byte 7 of new character
    return _offset++;                   // return then increment offset value for next character
}


/// ****| lcdClear | ********************************//*
/// * Brief: Clear all visible characters from the
/// *          screen.
/// * param:
/// *      N/A
/// * return:
/// *      N/A
/// ****************************************************/
void lcdClear() {
    lcdWriteCmd(CLEAR);
    SysTick_delay_ms(10);
}
/*
 * This function initializes the TIMERA for the PWM signal on
 * the LCD. It also initiilizes the ADC converter for the pwm signal.
 * After this function call, the brightness will be adjusted inside the
 * main menu.
 *
 */
void LCDBrightnessInit(){
        P8->SEL0 |=  BIT5;  // Analog, SEL0==SEL1==1
        P8->SEL1 |=  BIT5;
        P8->DIR  &= ~BIT5;

        ADC14->CTL0         =       0b11000101111001100111011110010000;
        ADC14->CTL1         =       0b110000;                  // Bits 5 and 4 = 11 to enable 14 bit conversion

        ADC14->MCTL[0]      =       20;                         //A20
        ADC14->MCTL[7]      =       0|BIT7;
        ADC14->CTL0         |=      BIT1;                       // Enable Conversion

        P2->DIR |= BIT7;
        P2->SEL0 |= BIT7;
        P2->SEL1 &= ~BIT7;

        TIMER_A0->CTL       = 0b1000010100;  //SMCLK, UP mode
        TIMER_A0->CCR[0]    = 3000-1;  //1kHz
        TIMER_A0->CCTL[4]   = 0b11100000; //reset/set
        TIMER_A0->CCR[4]    = 0;
}
