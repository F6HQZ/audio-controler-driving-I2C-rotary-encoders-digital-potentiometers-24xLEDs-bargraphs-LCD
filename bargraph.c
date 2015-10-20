/* bargraph.c :
 * 
 * Permits to drive a gang of I2C digital LEDs bargraphs behind a Raspberry Pi.
 * To be used with digital-pot.c digital-pot.h I2C-Display.c rotaryencoder.c 
 * and rotaryencoder.h libraries.
 * 
 * V.1.0.0
 */

/*=======================================================================\
|      - Copyright (c) - 2015 October- F6HQZ - Francois BERGERET -       |
|                                                                        |
| This "bargraph.h" and "bargraph.c" files can run only with the         |
| necessary and excellent wiringPi tools suite for Raspberry Pi from the |
| "Gordons Projects" web sites from Gordon Henderson :                   |
| https://projects.drogon.net/raspberry-pi/wiringpi/                     |
| http://wiringpi.com/                                                   |
|                                                                        |
| My libraries permit an easy use of few rotary encoders with push switch|
| in their axe, I2C LCD displays, I2C LEDs bargraphes and I2C digital    |
| potentiometers, and use them as "objects" stored in structures.        |
| Like this, they are all easy to read or modify values and specs from   |
| anywhere in your own software which must use them.                     |
|                                                                        |
| Thanks to friends who have supported me for this project and all guys  |
| who have shared their own codes with the community.                    |
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
 
#include <stdio.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "bargraph.h"

struct bargraph *setupbargraph(char *bargraph_label, int bargraph_address, 
	char *bargraph_ref, int bargraph_steps, unsigned char bargraph_bicolor,
	unsigned char bargraph_reversed) ; 
	
int numberofbargraphs = 0 ; // as writed, number of bargraphs, will be modified by the code later
	
void bargraphBlackOut(int setUpIO) ;

int bargraphInit(void) ;
int bargraphWrite(char *bargraph_label, long int value) ;

//======================================================================

int bargraphWrite(char *bargraph_label, long int value)
{ 	
//	printf("\n !!! bargraph: %s - value: %d \n", bargraph_label, value) ;
	int x = 0 ;
	
	struct bargraph *bargraph = bargraphs ;	
	for (; bargraph < bargraphs + numberofbargraphs ; bargraph++)
	{
		if (bargraph_label == bargraph->bargraph_label)
		{
			if (bargraph->bargraph_ref == "adafruit1721")
			{ 	// found one
//				printf(" >>> found setUpIO:%d \n",bargraph->bargraph_setUpIO) ;
				int slaveAddressByte = bargraph->bargraph_address << 1 | 0b0 ; // prepare the first byte including the internal sub digipot address, only for displaying and tests, because it's sent automaticaly by wiringpi itself, don't care !	
				int instructionByte ;
				
				bargraphBlackOut(bargraph->bargraph_setUpIO) ;
				
				// select the correct matrix green row :
				x = value / 4 ; // 4 consecutive same color leds blocs
//				printf("### value:%d - x:%d - x/3:%d - even/odd:%d - x%3:%d - x%6:%d - x%9:%d \n", value, x, x/3, (x/3)%2, x%3, x%6, x%9) ;				
				printf("§§§ value:%d - page# x/3:%d - (x/3)/2:%d - ((x/3)/2)-1:%d - (8*((x/3)/2)):%d - (16*(((x/3)/2)-1)):%d \n",value,x/3,(x/3)/2,((x/3)/2)-1,(8*((x/3)/2)),(16*(((x/3)/2)-1)) ) ;

				if((x/3)%2==0) 
				{ // even - (pair) - first 3 x 8 bits blocs (LSB)
					switch (x%3) // LED row
					{
						case 0 :
							{ value = 1 << (value - 0 -(8*((x/3)/2)) -(16*((x/3)/2))) ; instructionByte = 0x01 ; break ; }
						case 1 : 
							{ value = 1 << (value - 4 -(8*((x/3)/2)) -(16*((x/3)/2))) ; instructionByte = 0x03 ; break ; }
						case 2 : 
							{ value = 1 << (value - 8 -(8*((x/3)/2)) -(16*((x/3)/2))) ; instructionByte = 0x05 ; break ; }
					}
				}
				else
				{ // odd - (impair) - last 3 x 8 bits blocs (MSB)
					switch (x%3) // LED row
					{
						case 0 :
							{ value = 1 << (value -  8 -(8*((x/3)/2)) -(16*(((x/3)/2)))) ; instructionByte = 0x01 ; break ; }
						case 1 : 
							{ value = 1 << (value - 12 -(8*((x/3)/2)) -(16*(((x/3)/2)))) ; instructionByte = 0x03 ; break ; }
						case 2 : 
							{ value = 1 << (value - 16 -(8*((x/3)/2)) -(16*(((x/3)/2)))) ; instructionByte = 0x05 ; break ; }
					}
				}
				
//				printf(" === odd/even:%d - value: %d - setUpIO:%d - instructionByte: 0x%x - value: (%d) 0x%x ===\n\n", (x/3)%2, value, bargraph->bargraph_setUpIO, instructionByte, value, value) ;
				
				x = wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ;
			}
			
			break ;
		}
	}
	return x ;
}

void bargraphBlackOut(int setUpIO)
{	
	int loop = 0 ;
	int value = 0x00 ; 
	
	for(; loop < 6; loop++)
	{
		wiringPiI2CWriteReg8(setUpIO, loop, value) ;
		delay(2) ;
		// printf("+++ setUpIO: %d - loop: %d - value: %d +++ \n", setUpIO,  loop, value) ;
	}
}

//======================================================================

int bargraphInit(void)
{
	printf("\n bargraphInit start...") ;
	
	struct bargraph *bargraph = bargraphs ;	
	int x ;
	int slaveAdressByte ;
	int instructionByte ;
		
	for (; bargraph < bargraph + numberofbargraphs ; bargraph++)
	{
		if (bargraph->bargraph_ref == "adafruit1721")
		{
			printf(" - found one...") ;
			slaveAdressByte = bargraph->bargraph_address << 1 | 0b0 ; // prepare the first byte including the internal sub digipot address, only for displaying and tests, because it's sent automaticaly by wiringpi itself, don't care !

			instructionByte = HT16K33_SYS_SETUP_REGISTER_OFF ; // 0x20 - this is the "int reg", the second I2C byte sent by wiringpi
			x = wiringPiI2CWrite(bargraph->bargraph_setUpIO, instructionByte) ; // send the complete I2C frame to the chip
			delay(2) ;
			instructionByte = HT16K33_SYS_SETUP_REGISTER_ON ; // 0x21 - this is the "int reg", the second I2C byte sent by wiringpi
			x = wiringPiI2CWrite(bargraph->bargraph_setUpIO, instructionByte) ; // send the complete I2C frame to the chip
			delay(2) ;
			instructionByte = HT16K33_ROW_INT_SET_REGISTER ; // 0xa0 - this is the "int reg", the second I2C byte sent by wiringpi
			x = wiringPiI2CWrite(bargraph->bargraph_setUpIO, instructionByte) ; // send the complete I2C frame to the chip
			delay(2) ;
			instructionByte = HT16K33_REGISTER_DISPLAY_OFF ; // 0x80 - this is the "int reg", the second I2C byte sent by wiringpi
			x = wiringPiI2CWrite(bargraph->bargraph_setUpIO, instructionByte) ; // send the complete I2C frame to the chip
			delay(2) ;
			instructionByte = HT16K33_REGISTER_DISPLAY_ON ; // 0x81 - this is the "int reg", the second I2C byte sent by wiringpi
			x = wiringPiI2CWrite(bargraph->bargraph_setUpIO, instructionByte) ; // send the complete I2C frame to the chip
			delay(2) ;
			instructionByte = HT16K33_BRIGHTNESS ; // 0xe0 to 0xef - this is the "int reg", the second I2C byte sent by wiringpi
			x = wiringPiI2CWrite(bargraph->bargraph_setUpIO, instructionByte) ; // send the complete I2C frame to the chip
			delay(2) ;
			bargraphBlackOut(bargraph->bargraph_setUpIO) ;
			delay(2) ;
			
			// bargraph dance for test, starting with GREEN LEDs :
			printf(" - LAS VEGAS...") ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x01) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x02) ;
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x04) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x08) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x00) ;
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x01) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x02) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x04) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x08) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x00) ; 
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x01) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x02) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x04) ;
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x08) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x00) ; 
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x10) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x20) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x40) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x80) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x01, 0x00) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x10) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x20) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x40) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x80) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x03, 0x00) ; 
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x10) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x20) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x40) ;
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x80) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x05, 0x00) ; 
			delay(2) ;
			
			// RED LEDs now :
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x01) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x02) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x04) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x08) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x00) ; 
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x01) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x02) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x04) ;
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x08) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x00) ; 
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x01) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x02) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x04) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x08) ;
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x00) ; 
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x10) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x20) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x40) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x80) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x00, 0x00) ; 
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x10) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x20) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x40) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x80) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x02, 0x00) ; 
			delay(2) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x10) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x20) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x40) ;
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x80) ; 
			delay(30) ;
			wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, 0x04, 0x00) ; 
			delay(2) ;
				
			printf(" - bargraphInit terminated...") ;
			
			break ;
		}	
	}
	return x ;
}

//======================================================================

struct bargraph *setupbargraph(char *bargraph_label, int bargraph_address, 
	char *bargraph_ref, int bargraph_steps, unsigned char bargraph_bicolor,
	unsigned char bargraph_reversed)
{
	if (numberofbargraphs > MAX_BARGRAPHS)
	{
		printf("Max number of bargraphs exceded: %i\n", MAX_BARGRAPHS) ;
		return NULL ;
	}
	
	struct bargraph *newbargraph = bargraphs + numberofbargraphs++ ;
	newbargraph->bargraph_label = bargraph_label ;       // name or label as "Volume" or "Balance" or "Treble", etc...
	newbargraph->bargraph_address = bargraph_address ;   // address of the chip on the I2C or SPI bus (check with "gpio i2cdetect" Linux console instruction)
	newbargraph->bargraph_ref = bargraph_ref ;           // model, provider/manufacturer reference
	newbargraph->bargraph_steps = bargraph_steps ;       // number of LEDs steps without counting colors, only what is visible when lights turned OFF
	newbargraph->bargraph_bicolor = bargraph_bicolor ;   // 1 or 2. Number of different colors/LEDs (ex: GREEN and RED LEDs = 2, ORANGE is ONLY this 2 colors ON simultaneously.)
	newbargraph->bargraph_reversed = bargraph_reversed ; // 0 = no - 1 =:mmmmmmmmmmmmmmmmmmmmmmml yes - left to right or right to left, depending hardware integration in a front pannel
		
	setUpIO = wiringPiI2CSetup(bargraph_address) ; // I2C init, return the standard Linux file number to handle the I2C chip
	if (setUpIO < 0) { printf("setUpIO: Unable to intialise I2C: %s\n", strerror(errno)) ;	}
//	printf(">>> bargraph_setUpIO: %d \n",setUpIO) ;
	newbargraph->bargraph_setUpIO = setUpIO ;
	 
	return newbargraph ;
}

