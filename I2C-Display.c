/* I2C-Display.c :
 * To use concurently with lcd.h and lcd.c from "wiringPi" LPGLv3 lib 
 * on Raspberry Pi.
 * V.1.1.0
 */
 
/*=======================================================================\
|      - Copyright (c) - 2015 October - F6HQZ - Francois BERGERET -      |
|                                                                        |
| This I2C-Display.c open source code file can run only with the         |
| necessary and excellent wiringPi tools suite for Raspberry Pi from the |
| "Gordons Projects" web sites from Gordon Henderson :                   |
| https://projects.drogon.net/raspberry-pi/wiringpi/                     |
| http://wiringpi.com/                                                   |
|                                                                        |
| My library permits an immediate and easy use of an Hitachi 44780 LCD   |
| display or compatible, drived by a PCF8574 8-bits I2C I/O expender.    |
|                                                                        |
| Thanks to friends who have supported me for this project and all guys  |
| who have shared their own codes with the Open Source community.        |
|                                                                        |
| Always exploring new technologies, curious about any new idea or       |
| solution, while respecting and thanking the work of alumni who have    |
| gone before us.                                                        |
|                                                                        |
| Enjoy !                                                                |
|                                                                        |
| Feedback for bugs or ameliorations to f6hqz-m@hamwlan.net, thank you ! |
\=======================================================================*/
 
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation; either version 3 of the 
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program; 
 * if not, see <http://www.gnu.org/licenses/>.
 */
 
/*
 * Few of following commented lines are to print some variables values 
 * for debugging, evaluation, curiosity and self training only.        
 * Enable them at your need and personal risk only.                    
 */
 
#include <stdio.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8574.h>
#include <mcp23017.h>

#include "lcd.c"

#include "binary-values.h"

// define I2C LCD or I/O extension module address on the I2C bus
#define LCD_I2C_ADDRESS	0x3f // or what you see with "gpio i2cdetect" or "i2cdetect -y 1" programm on Linux console

// define first I2C encoder pins to LCD features :
#define AF_BASE	100 	// arbitrary first pin allocation. Must be > 64
#define AF_RS	(AF_BASE + 0)
#define AF_RW	(AF_BASE + 1)
#define AF_E	(AF_BASE + 2)
#define AF_BL	(AF_BASE + 3)
#define AF_D1	(AF_BASE + 4)
#define AF_D2	(AF_BASE + 5)
#define AF_D3	(AF_BASE + 6)
#define AF_D4	(AF_BASE + 7)
#define LCD_DELAY	1	// needs some time between two sending to a LCD to avoid to turn it mad

unsigned int backlightTempo = 3000 ; // the backlight cut off after a while (ms), or never if 0
unsigned long int backlightTimer ; // display backlight enlighting duration timer
unsigned char backlightStatus ; // backlight ON or OFF status

int setUpIO ;
int lcdDisplay1 ;
int errno ;

void displayInit (void) ;
void displayShow (char *line1, char *line2) ;
 
void displayInit (void)
{
/*	// I2C bus to LCD init // not needed if you have included the <pcf8574.h> wiringPi library
	setUpIO = wiringPiI2CSetup(LCD_I2C_ADDRESS) ;
	if (setUpIO < 0) { printf("setUpIO: Unable to intialise I2C: %s\n", strerror (errno)) ;	}

	int x = wiringPiI2CRead(LCD_I2C_ADDRESS) ;	// strictly a personal curiosity
	printf("LCD I2C response: %d \n", x) ;
*/
	pcf8574Setup(AF_BASE, LCD_I2C_ADDRESS) ; // new I2C GPIO extension pins arbitrary allocation
	
	pinMode(AF_BL, OUTPUT) ;
	digitalWrite(AF_BL, ON) ; delay(30) ;	// Backlight ON at starting, prooving that "light is on"
		
	pinMode(AF_RW, OUTPUT) ;
	digitalWrite(AF_RW, OFF) ; delay(30) ; // LCD displays nothing if not pushed down "OFF"
	
	lcdDisplay1 = lcdInit(2,16,4,  AF_RS,AF_E,  AF_D1,AF_D2,AF_D3,AF_D4,  0,0,0,0) ;
	if (lcdDisplay1 < 0) { printf("lcdDisplay1: Unable to initialize LCD: %s\n", strerror (errno)) ; }	

	lcdClear(lcdDisplay1) ; delay(LCD_DELAY) ;
	
/*	optionnal functions :
//	lcdDisplay(lcdDisplay1, ON) ;	// turn ON/OFF the display (ON default)
//	lcdCursor(lcdDisplay1, ON) ;	// turn ON/OFF the cursor (OFF default)
//	lcdCursorBlink(lcdDisplay1, ON) ;	// turn ON/OFF the cursor blink (OFF default)
//	lcdCharDef(lcdDisplay1, unsigned char data[8]) ; // re-define one of the 8 user-definable 5x8 characters
//	lcdPutchar(lcdDisplay1, unsigned char data) ; // display only one ASCII character
//	lcdPrintf(lcdDisplay1, const char *message, ...) ; // same thing as printf formatting commands
*/
	// optionnal "Hello" message at application starting :
	lcdPosition(lcdDisplay1,0,0) ; delay(LCD_DELAY) ;
	lcdPuts(lcdDisplay1, "I2C-Midi Control") ; delay(LCD_DELAY) ; // 16 characters long
 	lcdPosition(lcdDisplay1,0,1) ; delay(LCD_DELAY) ;
	lcdPuts(lcdDisplay1, "F6HQZ F.BERGERET") ; delay(LCD_DELAY) ; // 16 characters long
	printf("Initialisation du Display OK \n") ;
}

void displayShow (char *line1, char *line2)
{
	if (backlightStatus == 0)
	{
		digitalWrite(AF_BL, ON) ; delay(LCD_DELAY) ; // put Backlight ON
		backlightStatus = ON ;
	}
		
	backlightTimer = millis() ; // reset backlight timer now
	
	if (line1 != "" && line2 != "")
	{
		lcdClear(lcdDisplay1) ; delay(LCD_DELAY) ;
		
		lcdPosition(lcdDisplay1,0,0) ; delay(LCD_DELAY) ;
		//printf("LCD ligne 1: %s \n", line1) ;	
		//	lcdPuts(lcdDisplay1, line1) ; delay(LCD_DELAY) ;
		lcdPrintf(lcdDisplay1, line1) ; delay(LCD_DELAY) ;
		
		lcdPosition(lcdDisplay1,0,1) ; delay(LCD_DELAY) ;
		//printf("LCD ligne 2: %s \n", line2) ;
		//	lcdPuts(lcdDisplay1, (const char *)line2) ; delay(LCD_DELAY) ;
		lcdPrintf(lcdDisplay1, line2) ; delay(LCD_DELAY) ;
	}
}
