/*
 * MenuLib.h
 *
 *  Created on: Dec 14, 2020
 *      Author: calla
 */

#ifndef MENULIB_H_
#define MENULIB_H_

enum menuStates{
    Main,
    Door,
    Motor,
    Lights
};

void pwmMotor(uint16_t period, uint16_t periodON);
int button_Debounce(uint16_t delay, int bit);
void navigateMenu();
void readKeypad();
void mainMenu();
void setMotor();
void setServo();
void pwmServo(uint16_t period, uint16_t periodON);
void updateBrightness();
void updateColor();

#endif /* MENULIB_H_ */
