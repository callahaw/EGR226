/***************************************************************************************
*                                LCD_Library.h
*                                Trevor Ekin (Adapted from Dr. Nabeeh Kandalaft)
*                         EGR226      Date: February-21-2019
*
*  This is a library for the 4x16 LCD.
*
*  All functions are briefly described in their comment blocks.  The /// notation makes
*  it so the function description block is visible when you hovering over a function call
*  in any file (this feature is called Intellisense).
*
*  All pins are set with default values (see below) but they can be easily changed.  See
*  description below in "Pins" comment block
* **************************************************************************************/

/***************************************************************************************
*Revision Log:
*   (v2.0) 6/15/2020: Reset Byte macros to make reset sequence a more understandable.
* **************************************************************************************/

#ifndef LCD_LIBRARY_H_
#define LCD_LIBRARY_H_

//#include "driverlib.h"  // for use with driverlib
#include "msp.h"
#include <stdint.h>

/************  Pins   *******************************
*          MSP432 PINS (Default)
* P10.2 -> LCD D4
* P10.3 -> LCD D5
* P10.4 -> LCD D6
* P10.5 -> LCD D7
* P10.1 -> LCD E
* P7.6 -> LCD RS
* P10.0 -> LCD RW
* NOTICE:
*   This section is configurable and dynamic.  If you
*   would like to use different pins, make the swaps
*   here.  For example, if you would like D5 to be on
*   P2.4 instead, make following changes:
*       D5_SEL1 P2SEL1
*       D5_SEL0 P2SEL0
*       D5_DIR P2DIR
*       D5_OUT P2OUT
*       D5 BIT4
* *****************************************************/

// SEL1 registers (modify as needed)
#define EN_SEL1  P5SEL1
#define RS_SEL1  P2SEL1
#define RW_SEL1  P2SEL1
#define D4_SEL1  P2SEL1
#define D5_SEL1  P6SEL1
#define D6_SEL1  P6SEL1
#define D7_SEL1  P5SEL1

// SEL0 registers (modify as needed)
#define EN_SEL0  P5SEL0
#define RS_SEL0  P2SEL0
#define RW_SEL0  P2SEL0
#define D4_SEL0  P2SEL0
#define D5_SEL0  P6SEL0
#define D6_SEL0  P6SEL0
#define D7_SEL0  P5SEL0

// direction registers (modify as needed)
#define EN_DIR  P5DIR
#define RS_DIR  P2DIR
#define RW_DIR  P2DIR
#define D4_DIR  P2DIR
#define D5_DIR  P6DIR
#define D6_DIR  P6DIR
#define D7_DIR  P5DIR

// Port registers (modify as needed)
#define EN_OUT  P5OUT
#define RS_OUT  P2OUT
#define RW_OUT  P2OUT
#define D4_OUT  P2OUT
#define D5_OUT  P6OUT
#define D6_OUT  P6OUT
#define D7_OUT  P5OUT

// Pin BITs (modify as needed)
#define EN  BIT6
#define RS  BIT5
#define RW  BIT4
#define D4  BIT3
#define D5  BIT6
#define D6  BIT7
#define D7  BIT1

// Bit Toggling (DO NOT CHANGE)
#define EN_LOW (EN_OUT &= ~EN)
#define EN_HIGH (EN_OUT |= EN)
#define RS_LOW (RS_OUT &= ~RS)
#define RS_HIGH (RS_OUT |= RS)
#define RW_LOW (RW_OUT &= ~RW)
#define RW_HIGH (RW_OUT |= RW)
#define D4_LOW (D4_OUT &= ~D4)
#define D4_HIGH (D4_OUT |= D4)
#define D5_LOW (D5_OUT &= ~D5)
#define D5_HIGH (D5_OUT |= D5)
#define D6_LOW (D6_OUT &= ~D6)
#define D6_HIGH (D6_OUT |= D6)
#define D7_LOW (D7_OUT &= ~D7)
#define D7_HIGH (D7_OUT |= D7)

// Select Clearing (DO NOT CHANGE)
#define EN_GPIO (EN_SEL1 &= ~EN);(EN_SEL0 &= ~EN)
#define RS_GPIO (RS_SEL1 &= ~RS);(RS_SEL0 &= ~RS)
#define RW_GPIO (RW_SEL1 &= ~RW);(RW_SEL0 &= ~RW)
#define D4_GPIO (D4_SEL1 &= ~D4);(D4_SEL0 &= ~D4)
#define D5_GPIO (D5_SEL1 &= ~D5);(D5_SEL0 &= ~D5)
#define D6_GPIO (D6_SEL1 &= ~D6);(D6_SEL0 &= ~D6)
#define D7_GPIO (D7_SEL1 &= ~D7);(D7_SEL0 &= ~D7)

/// represent provided nibble on data lines use ternary statements and multi-line macro
/// *  note: ternary statements are like an "if / else" statement.
/// *      if/else example:
/// *
/// *      if(x & 0x01) {
/// *          D4_HIGH;
/// *      } else {
/// *          D4_LOW;
/// *      }
/// *
/// *      Ternary version with exact same result:
/// *      (x & 0x01) ? D4_HIGH : D4_LOW;
/// */
#define SetNibble(x) \
    ((x & 0x01) ? D4_HIGH : D4_LOW); \
    ((x & 0x02) ? D5_HIGH : D5_LOW); \
    ((x & 0x04) ? D6_HIGH : D6_LOW); \
    ((x & 0x08) ? D7_HIGH : D7_LOW);


/************************** Commands ***********************************
************************************************************************/
#define CLEAR       0x01
#define HOME        0x02

// RESET COMMANDS TO INITIALIZE DEVICE: Send 4 nibbles {3, 3, 3, 2} to restart device and set it for 4-wire mode
#define RESET_BYTE_1    0x33    // Reset sequence, part 1 and 2
#define RESET_BYTE_2    0x32    // Reset sequence, part 3 and 4


// ENTRY MODE COMMAND: 0b0000 01[I][S] -- I = Increment, S = Shift
#define ENTRYMODE_00    0x04    // no auto increment, no display shift  (DEFAULT)
#define ENTRYMODE_01    0x05    // no auto increment, display shift
#define ENTRYMODE_10    0x06    // auto increment, no display shift
#define ENTRYMODE_11    0x07    // auto increment, display shift

// DISPLAY CONTORL COMMAND: 0b0000 1[D][C][B] -- D = Display, C = Cursor, B = Blinking
#define DISPLAY_000     0x08    // display off, cursor off, blinking off    (DEFAULT)
#define DISPLAY_001     0x09    // display off, cursor off, blinking on     (not useful)
#define DISPLAY_010     0x0A    // display off, cursor on, blinking off     (not useful)
#define DISPLAY_011     0x0B    // display off, cursor on, blinking on      (not useful)
#define DISPLAY_100     0x0C    // display on, cursor off, blinking off
#define DISPLAY_101     0x0D    // display on, cursor off, blinking on
#define DISPLAY_110     0x0E    // display on, cursor on, blinking off
#define DISPLAY_111     0x0F    // display on, cursor on, blinking on

// CURSOR/DISPLAY SHIFT COMMAND: 0b0001 [DC][RL] * * -- DC = Display or Cursor, RL = Right or Left, * = don't care
#define SHIFT_00        0x10;   // cursor shift to the left
#define SHIFT_01        0x14;   // cursor shift to the right
#define SHIFT_10        0x18;   // display shift to the left
#define SHIFT_11        0x1C;   // display shift to the right

// FUNCTION SET COMMAND: 0b001[DL] [N][F] * * -- DL = Data length, N = display lines, F = character font
#define FSET_000        0x20    // 4-bit data, 1 line, 5x8 dot font
#define FSET_001        0x24    // 4-bit data, 1 line, 5x10 dot font
#define FSET_010        0x28    // 4-bit data, 2 lines, 5x8 dot font
#define FSET_011        0x2C    // 4-bit data, 2 lines, 5x10 dot font
#define FSET_100        0x30    // 8-bit data, 1 line, 5x8 dot font     (standard)
#define FSET_101        0x34    // 8-bit data, 1 line, 5x10 dot font
#define FSET_110        0x38    // 8-bit data, 2 lines, 5x8 dot font
#define FSET_111        0x3C    // 8-bit data, 2 lines, 5x10 dot font

// CGRAM ADDRESS
#define CGRAM           0x40    // start address for CGRAM data, custom graphics

/************************** Commands ***********************************
************************************************************************/

/********************* Structure Definitions ***************************
************************************************************************/
/// custom_char_t is a struct containing 8 bytes of data, representing the
///      8 rows of dots that create an LCD character. Each bit is either
///      on (1) or off (0) to display the image you desire.
/// You can make a custom character at (https://omerk.github.io/lcdchargen/)
typedef struct custom_char{
    uint8_t line0;
    uint8_t line1;
    uint8_t line2;
    uint8_t line3;
    uint8_t line4;
    uint8_t line5;
    uint8_t line6;
    uint8_t line7;
}custom_char_t;
/********************* Structure Definitions ***************************
************************************************************************/

/********************** Global Definitions *****************************
************************************************************************/
uint8_t _offset;                // offset in CGRAM for new custom characters
/********************** Global Definitions *****************************
************************************************************************/

/********************* Function Prototypes *****************************
************************************************************************/
void lcdInit ();
void lcdClear();
void lcdTriggerEN();
void lcdWriteData(unsigned char data);
void lcdWriteCmd (unsigned char  cmd);
void lcdSetText(char * text, int x, int y);
void lcdSetChar(char c, int x, int y);
void lcdSetInt (int val,     int x, int y);
void LCDBrightnessInit();
uint8_t lcdCreateCustomChar(custom_char_t* cc);
/********************* Function Prototypes *****************************
************************************************************************/



#endif /* LIQUID_CRYSTAL_H_ */
