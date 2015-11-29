/* bargraph.c :
 * 
 * Permits to drive a gang of I2C digital LEDs bargraphs behind a Raspberry Pi.
 * To be used with digital-pot.c digital-pot.h I2C-Display.c rotaryencoder.c 
 * and rotaryencoder.h libraries.
 * 
 * V.1.0.1
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
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h> // need to add "-l" during compiling phase, to link the "llibm.a" library

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "bargraph.h"

# define FS 255

struct bargraph *setupbargraph(char *bargraph_label, int bargraph_address, 
	char *bargraph_ref, int bargraph_steps, unsigned char bargraph_bicolor,
	unsigned char bargraph_reversed) ; 
	
int numberofbargraphs = 0 ; // as writed, number of bargraphs, will be modified by the code later
extern char VuMeterWakeUp ;
	
void bargraphBlackOut(int setUpIO) ;
int bargraphInit(void) ;
int bargraphWrite(char *bargraph_label, float low_Limit, float high_Limit, int bargraph_mode, long int value) ;

//======================================================================

int bargraphWrite(char *bargraph_label, float low_Limit, float high_Limit, int bargraph_mode, long int value)
{ 	
	//printf("\n !!! bargraph: %s - value: %d \n", bargraph_label, value) ;
	int x = 0 ;
	float coef = high_Limit - low_Limit ;
	//printf("> 0/ lo:%f - hi:%f - coef:%f - value:%d \n",low_Limit,high_Limit,coef,value) ;
	
	struct bargraph *bargraph = bargraphs ;	
	for (; bargraph < bargraphs + numberofbargraphs ; bargraph++)
	{
		if (bargraph_label == bargraph->bargraph_label)
		{	// found one
			switch (bargraph_mode)
			{	// different displaying mode cases, different effects
			
				//======================================================
				
				case 0 : // green LEDs background bar for the full scale, but a unique red LED turning several times 
				         // for each of the smallest steps, for rotary encoder rotation indication guide
				{
					printf("\n*** bargraph case 0 ***\n") ;
					if (bargraph->bargraph_ref == "adafruit1721")
					{ 	
						//printf("> 1/ lo:%f - hi:%f - coef:%f - value:%d \n", low_Limit, high_Limit, coef, value) ;
						if ((high_Limit - low_Limit) <= 600) // number of max steps for this bargraph
							{ coef = 599 / coef ; value = value * coef ; }
						else
							{ coef = coef / 599 ; value = value / coef ; }
						//printf("> 2/ lo:%f - hi:%f - coef:%f - value:%f \n", low_Limit, high_Limit, coef, value) ;
						//printf(" >>> found setUpIO:%d \n",bargraph->bargraph_setUpIO) ;
						int slaveAddressByte = bargraph->bargraph_address << 1 | 0b0 ; // prepare the first byte including the internal sub digipot address, only for displaying and tests, because it's sent automaticaly by wiringpi itself, don't care !	
						int instructionByte ;
						
						bargraphBlackOut(bargraph->bargraph_setUpIO) ; // cut all the LEDs light first
						
						// green LEDs in background, one green LED more step each time the RED LED cursor do a complete range course (24 green LEDs for 24 pages) :
						long int backgroundLEDs = ((value/4)/3)/2 ; // 4 LEDs blocs, 3 rows by color matrix, 2 separated 4 bits blocs on the same row
						
						if      (backgroundLEDs == 0)
						{ 	backgroundLEDs = 0b00000000 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 1)
						{ 	backgroundLEDs = 0b00000001 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 2)
						{ 	backgroundLEDs = 0b00000011 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 3)
						{ 	backgroundLEDs = 0b00000111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 4)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }				
		
						else if (backgroundLEDs == 5)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00000001 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 6)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00000011 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 7)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00000111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 8)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
					
						else if (backgroundLEDs == 9)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00000001 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 10)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00000011 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 11)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00000111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 12)
						{ 	backgroundLEDs = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
		
						else if (backgroundLEDs == 13)
						{ 	backgroundLEDs = 0b00011111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 14)
						{ 	backgroundLEDs = 0b00111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 15)
						{ 	backgroundLEDs = 0b01111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 16)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
		
						else if (backgroundLEDs == 17)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00011111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 18)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; 
							backgroundLEDs = 0b00111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 19)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b01111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 20)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
										
						else if (backgroundLEDs == 21)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b00011111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 22)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b00111111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 23)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b01111111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
						else if (backgroundLEDs == 24)
						{ 	backgroundLEDs = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ;
							backgroundLEDs = 0b11111111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, backgroundLEDs) ; delay(I2C_DELAY) ; }
																
						//printf("instructionByte: 0x%x - backgroundLEDs:%d 0x%x \n",instructionByte,backgroundLEDs,backgroundLEDs) ;
															
						// select the correct matrix red LEDs row, only one LED lighting, running along the bargraph for more precision :
						x = value / 4 ; // 4 consecutive same color leds blocs
						//printf("### value:%d - x:%d - x/3:%d - even/odd:%d - x%3:%d - x%6:%d - x%9:%d \n", value, x, x/3, (x/3)%2, x%3, x%6, x%9) ;				
						//printf("§§§ value:%d - page# x/3:%d - (x/3)/2:%d - ((x/3)/2)-1:%d - (8*((x/3)/2)):%d - (16*(((x/3)/2)-1)):%d \n",value,x/3,(x/3)/2,((x/3)/2)-1,(8*((x/3)/2)),(16*(((x/3)/2)-1)) ) ;
		
						if((x/3)%2==0) 
						{ // even - (pair) - first 3 x 8 bits blocs (LSB)
							switch (x%3) // LED row
							{
								case 0 :
									{ value = 1 << (value - 0 -(8*((x/3)/2)) -(16*((x/3)/2))) ; instructionByte = 0x00 ; break ; }
								case 1 : 
									{ value = 1 << (value - 4 -(8*((x/3)/2)) -(16*((x/3)/2))) ; instructionByte = 0x02 ; break ; }
								case 2 : 
									{ value = 1 << (value - 8 -(8*((x/3)/2)) -(16*((x/3)/2))) ; instructionByte = 0x04 ; break ; }
							}
						}
						else
						{ // odd - (impair) - last 3 x 8 bits blocs (MSB)
							switch (x%3) // LED row
							{
								case 0 :
									{ value = 1 << (value -  8 -(8*((x/3)/2)) -(16*(((x/3)/2)))) ; instructionByte = 0x00 ; break ; }
								case 1 : 
									{ value = 1 << (value - 12 -(8*((x/3)/2)) -(16*(((x/3)/2)))) ; instructionByte = 0x02 ; break ; }
								case 2 : 
									{ value = 1 << (value - 16 -(8*((x/3)/2)) -(16*(((x/3)/2)))) ; instructionByte = 0x04 ; break ; }
							}
						}
						
						//printf(" === odd/even:%d - value: %d - setUpIO:%d - instructionByte: 0x%x - value: (%d) 0x%x ===\n\n", (x/3)%2, value, bargraph->bargraph_setUpIO, instructionByte, value, value) ;
						
						x = wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ;
					}
					break ;
				}
				
				//======================================================
				
				case 1 : // like a standard analog audio VU-Meter but linear like a Voltmeter (not log), green bar + amber small bar + a few red leds at right, with a needle integration balistic emulation (temporized decay)
				{
					//printf("\n*** bargraph case 1 ***\n") ;
					if (bargraph->bargraph_ref == "adafruit1721")
					{ 	// found one
						int bargraph_length = 0 ; // how many LEDs ON, 0 to 24
						int bargraph_value = 0 ; // value which will be realy sent to bargraph to display through I2C
						int attackTime = 300 ; // msec from min to max
						int releaseTime = 300 ; // msec from max to min
						int suplBits = 0 ; // used to add few LSB to bargraph_Value
						int bargraphRedBlackout ;

						if ((high_Limit - low_Limit) <= 24) // number of max steps for this bargraph
							{ coef = 24 / coef ; bargraph_length = value * coef ; }
						else
							{ coef = coef / 24 ; bargraph_length = value / coef ; }	
						
						if ((bargraph_length != bargraph->bargraph_value) || (VuMeterWakeUp == 1))
						{	// write to bargraph if different value only
	
							if (bargraph_length > bargraph->bargraph_value)
							{
								if (bargraph->bargraph_value < 24)
								{
									delay(attackTime / bargraph->bargraph_steps) ;
									++bargraph->bargraph_value ; // slowly increases the enlighted bargraph length for galvanometer balistic emulation
								}
								bargraph_length = bargraph->bargraph_value ;						}
							else 
							{
								if ((bargraph->bargraph_value > 0) && (bargraph_length < bargraph->bargraph_value)) 
								{ 
									delay(releaseTime / bargraph->bargraph_steps) ;
									--bargraph->bargraph_value ; // slowly decreases the enlighted bargraph length for galvanometer balistic emulation
								}  
								bargraph_length = bargraph->bargraph_value ;
							}
	
							int slaveAddressByte = bargraph->bargraph_address << 1 | 0b0 ; // prepare the first byte including the internal sub digipot address, only for displaying and tests, because it's sent automaticaly by wiringpi itself, don't care !	
							int instructionByte ;
							
							// 3 LEDs segments with different colors green, orange and red at final :
							if (bargraph_length == 0)
							{
								suplBits = 0 ;
							}
							else if (bargraph_length == 1 | bargraph_length == 5 | bargraph_length ==  9 | bargraph_length == 13 | bargraph_length == 17 | bargraph_length == 21)
							{
								suplBits = 0b1 ;
							}
							else if (bargraph_length == 2 | bargraph_length == 6 | bargraph_length == 10 | bargraph_length == 14 | bargraph_length == 18 | bargraph_length == 22)
							{
								suplBits = 0b11 ;
							}
							else if (bargraph_length == 3 | bargraph_length == 7 | bargraph_length == 11 | bargraph_length == 15 | bargraph_length == 19 | bargraph_length == 23)
							{
								suplBits = 0b111 ;
							}
							else if (bargraph_length == 4 | bargraph_length == 8 | bargraph_length == 12 | bargraph_length == 16 | bargraph_length == 20 | bargraph_length == 24)
							{
								suplBits = 0b1111 ;
							}
//							printf("\n === value:%d - ratio:%f - dBu:%f - bargraph_length:%d - suplBits:%d ===", value, ratio, dB, bargraph_length, suplBits) ;
							// no RED LEDs (all of them OFF)
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x00 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x02 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
	
							//GREEN LEDs now, depending the signal value
							if (bargraph_length < 5)
							{
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}			
							else if (bargraph_length > 4 && bargraph_length < 9)
							{
								bargraph_value = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 8 && bargraph_length < 13)
							{
								bargraph_value = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 12 && bargraph_length < 17)
							{							
								bargraph_value = ((0b00001111 << (bargraph_length -12)) | suplBits) ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 16 && bargraph_length < 21)
							{	
								bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00001111 << (bargraph_length -16) | suplBits) ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = ((0b00000000 | suplBits) << 4) ; instructionByte = 0x02 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 20 && bargraph_length < 25)
							{
								bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b11110000 ; instructionByte = 0x02 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = ((0b00000000 | suplBits) << 4) ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							if (value == 255)
							{ 	// switch amber LEDs to RED color for eyes alerting
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ; // the 4 amber LEDs change to RED only ar max value (255)
							}				
							VuMeterWakeUp = 0 ;
						}
					}
					break ;
				}
				
				//======================================================
				
				case 2 : // like a standard analog logarythmic audio VU-Meter, green bar + amber small bar + a small red bar at right before clipping indication by turning orange into red at full scale, with a small decay timing
				{
					//printf("\n*** bargraph case 2 ***\n") ;
					if (bargraph->bargraph_ref == "adafruit1721")
					{ 	// found one
//						value = 255 ;  // temporary value for test
						int values = 256 ;
						int zerodB = 255 ;
						int ledsQty = bargraph->bargraph_steps ; // 24 apparent steps
						int ledStep_dB = 2 ; // each LED bar one more step = 2 dB x 24 LEDs = 48 dB of dynamic = 8 bits !
						int bargraph_length = 0 ; // how many LEDs ON, 0 to 24
						int bargraph_value = 0 ; // value which will be realy sent to bargraph to display through I2C
						int suplBits = 0 ; // used to add few LSB to bargraph_Value
						int instructionByte ; 
						int bargraphRedBlackout ;
						int attackTime = 300 ; // msec from min to max
						int releaseTime = 300 ; // msec from max to min
						
						// for manual tests only
//						value = 10 ;
						
						// log variation	
						float ratio = (float) value / zerodB ;  // Input value / Full Scale
						float dB = 20 * log10(ratio) ;
						bargraph_length = 25 + (dB / ledStep_dB) ;
						
						// for manual tests only
//						bargraph_length = 1 ;
//						value = 255 ;						
						
						// few manual cosmetic corrections for first steps
						switch(value)
						{
							case 1 : bargraph_length = 1 ; break ; // the smallerst value to display on first LED  
							case 2 : bargraph_length = 2 ; break ; 
							case 3 : bargraph_length = 3 ; break ; 
							case 4 : bargraph_length = 4 ; break ; 
							case 5 : bargraph_length = 5 ; break ; 
							case 6 : bargraph_length = 6 ; break ; 
							case 7 : bargraph_length = 7 ; break ; 
							case 8 : bargraph_length = 8 ; break ; 
							case 9 : bargraph_length = 9 ; break ; 
							default: break ;
						}

						if ((bargraph_length != bargraph->bargraph_value) || (VuMeterWakeUp == 1))
						{	// write to bargraph if different value only
						
							if (bargraph_length > bargraph->bargraph_value)
							{
								if (bargraph->bargraph_value < 24)
								{
									delay(attackTime / bargraph->bargraph_steps) ;
									++bargraph->bargraph_value ; // slowly increases the enlighted bargraph length for galvanometer balistic emulation
								}
								bargraph_length = bargraph->bargraph_value ;						}
							else 
							{
								if ((bargraph->bargraph_value > 0) && (bargraph_length < bargraph->bargraph_value)) 
								{ 
									delay(releaseTime / bargraph->bargraph_steps) ;
									--bargraph->bargraph_value ; // slowly decreases the enlighted bargraph length for galvanometer balistic emulation
								}  
								bargraph_length = bargraph->bargraph_value ;
							}
	
							if (bargraph_length == 0)
							{
								suplBits = 0 ;
							}
							else if (bargraph_length == 1 | bargraph_length == 5 | bargraph_length ==  9 | bargraph_length == 13 | bargraph_length == 17 | bargraph_length == 21)
							{
								suplBits = 0b1 ;
							}
							else if (bargraph_length == 2 | bargraph_length == 6 | bargraph_length == 10 | bargraph_length == 14 | bargraph_length == 18 | bargraph_length == 22)
							{
								suplBits = 0b11 ;
							}
							else if (bargraph_length == 3 | bargraph_length == 7 | bargraph_length == 11 | bargraph_length == 15 | bargraph_length == 19 | bargraph_length == 23)
							{
								suplBits = 0b111 ;
							}
							else if (bargraph_length == 4 | bargraph_length == 8 | bargraph_length == 12 | bargraph_length == 16 | bargraph_length == 20 | bargraph_length == 24)
							{
								suplBits = 0b1111 ;
							}
	
	//						printf("\n === value:%d - ratio:%f - dBu:%f - bargraph_length:%d - suplBits:%d ===", value, ratio, dB, bargraph_length, suplBits) ;
	
							// no RED LEDs (all of them OFF)
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x00 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x02 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
	
							//GREEN LEDs now, depending the signal value
							if (bargraph_length < 5)
							{
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}			
							else if (bargraph_length > 4 && bargraph_length < 9)
							{
								bargraph_value = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 8 && bargraph_length < 13)
							{
								bargraph_value = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 12 && bargraph_length < 17)
							{							
								bargraph_value = ((0b00001111 << (bargraph_length -12)) | suplBits) ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 16 && bargraph_length < 21)
							{	
								bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00001111 << (bargraph_length -16) | suplBits) ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = ((0b00000000 | suplBits) << 4) ; instructionByte = 0x02 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 20 && bargraph_length < 25)
							{
								bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b11110000 ; instructionByte = 0x02 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = ((0b00000000 | suplBits) << 4) ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							if (value == 255)
							{ 	// switch amber LEDs to RED color for eyes alerting
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ; // the 4 amber LEDs change to RED only ar max value (255)
							}				
							VuMeterWakeUp = 0 ;
						}
					}
					break ;
				}
				
				//======================================================
				
				case 3 : // like a peak program digital VU-Meter (PPM), green bar + only one red led at right for max value just before clipping indication to never raise, with a small decay timing
				{
					//printf("\n*** bargraph case 3 ***\n") ;
					if (bargraph->bargraph_ref == "adafruit1721")
					{ 	// found one
						int values = 256 ;
						int zerodB = 255 ;
						int ledsQty = bargraph->bargraph_steps ; // 24 apparent steps
						int ledStep_dB = 2 ; // each LED bar one more step = 2 dB x 24 LEDs = 48 dB of dynamic = 8 bits !
						int bargraph_length = 0 ; // how many GREEN LEDs ON, 0 to 24
						int bargraph_value = 0 ; // value which will be realy sent to bargraph to display through I2C
						int suplBits = 0 ; // used to add few LSB to bargraph_Value
						int instructionByte ; 
						int bargraphRedBlackout ;
						int attackTime = 10 ; // msec from min to max
						int releaseTime = 1500 ; // msec from max to min
						
						// log variation	
						float ratio = (float) value / zerodB ;  // Input value / Full Scale
						float dB = 20 * log10(ratio) ;
						bargraph_length = 25 + (dB / ledStep_dB) ;
						
						// few manual cosmetic corrections for first steps
						switch(value)
						{
							case 1 : bargraph_length = 1 ; break ; // the smallerst value to display on first LED  
							case 2 : bargraph_length = 2 ; break ; 
							case 3 : bargraph_length = 3 ; break ; 
							case 4 : bargraph_length = 4 ; break ; 
							case 5 : bargraph_length = 5 ; break ; 
							case 6 : bargraph_length = 6 ; break ; 
							case 7 : bargraph_length = 7 ; break ; 
							case 8 : bargraph_length = 8 ; break ; 
							case 9 : bargraph_length = 9 ; break ; 
							default: break ;
						}
						
						if ((bargraph_length != bargraph->bargraph_value) || (VuMeterWakeUp == 1))
						{	// write to bargraph if different value only

							if (bargraph_length > bargraph->bargraph_value)
							{
								if (bargraph->bargraph_value < 24)
								{
									delay(attackTime / bargraph->bargraph_steps) ;
									++bargraph->bargraph_value ; // slowly increases the enlighted bargraph length for galvanometer balistic emulation
								}
								bargraph_length = bargraph->bargraph_value ;						}
							else 
							{
								if ((bargraph->bargraph_value > 0) && (bargraph_length < bargraph->bargraph_value)) 
								{ 
									delay(releaseTime / bargraph->bargraph_steps) ;
									--bargraph->bargraph_value ; // slowly decreases the enlighted bargraph length for galvanometer balistic emulation
								}  
								bargraph_length = bargraph->bargraph_value ;
							}
							
							// display enlight LEDs depending bargraph_length from 0 to 24, then enlight a last RED LED if value = 255
							if (bargraph_length == 0)
							{
								suplBits = 0 ;
							}
							else if (bargraph_length == 1 | bargraph_length == 5 | bargraph_length ==  9 | bargraph_length == 13 | bargraph_length == 17 | bargraph_length == 21)
							{
								suplBits = 0b1 ;
							}
							else if (bargraph_length == 2 | bargraph_length == 6 | bargraph_length == 10 | bargraph_length == 14 | bargraph_length == 18 | bargraph_length == 22)
							{
								suplBits = 0b11 ;
							}
							else if (bargraph_length == 3 | bargraph_length == 7 | bargraph_length == 11 | bargraph_length == 15 | bargraph_length == 19 | bargraph_length == 23)
							{
								suplBits = 0b111 ;
							}
							else if (bargraph_length == 4 | bargraph_length == 8 | bargraph_length == 12 | bargraph_length == 16 | bargraph_length == 20 | bargraph_length == 24)
							{
								suplBits = 0b1111 ;
							}
							
	//						printf("\n === value:%d - ratio:%f - dBu:%f - bargraph_length:%d - suplBits:%d ===", value, ratio, dB, bargraph_length, suplBits) ;
	
							// no RED LEDs (all of them OFF)
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x00 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x02 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
	
							//GREEN LEDs now, depending the signal value
							if (bargraph_length < 5)
							{
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}			
							else if (bargraph_length > 4 && bargraph_length < 9)
							{
								bargraph_value = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 8 && bargraph_length < 13)
							{
								bargraph_value = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 12 && bargraph_length < 17)
							{							
								bargraph_value = ((0b00001111 << (bargraph_length -12)) | suplBits) ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 16 && bargraph_length < 21)
							{	
								bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00001111 << (bargraph_length -16) | suplBits) ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 20 && bargraph_length < 25)
							{
								bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = ((0b00001111 << (bargraph_length -20)) | suplBits) ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							if (value == 255)
							{ 	
	//							bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
	//							bargraph_value = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b01111111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b10000000 ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ; // the last RED LED point if 255
							}
							VuMeterWakeUp = 0 ;
						}	
					}
					break ;
				}
				
				//======================================================
				
				case 4 : // green LEDs background bar for the full scale, no red led at all,
				         // for "LOG" rotary encoder rotation indication guide
				{
					printf("\n*** bargraph case 4 ***\n") ;
					if (bargraph->bargraph_ref == "adafruit1721")
					{ 	// found one
						int values = 256 ;
						int zerodB = 255 ;
						int ledsQty = bargraph->bargraph_steps ; // 24 apparent steps
						int ledStep_dB = 2 ; // each LED bar one more step = 2 dB x 24 LEDs = 48 dB of dynamic = 8 bits !
						int bargraph_length = 0 ; // how many GREEN LEDs ON, 0 to 24
						int bargraph_value = 0 ; // value which will be realy sent to bargraph to display through I2C
						int suplBits = 0 ; // used to add few LSB to bargraph_Value
						int instructionByte ; 
						int bargraphRedBlackout ;
						int attackTime = 0 ; // msec from min to max
						int releaseTime = 0 ; // msec from max to min
						
						// log variation	
						float ratio = (float) value / zerodB ;  // Input value / Full Scale
						float dB = 20 * log10(ratio) ;
						bargraph_length = 25 + (dB / ledStep_dB) ;
						
						// few manual cosmetic corrections for first steps
						switch(value)
						{
							case 1 : bargraph_length = 1 ; break ; // the smallerst value to display on first LED  
							case 2 : bargraph_length = 2 ; break ; 
							case 3 : bargraph_length = 3 ; break ; 
							case 4 : bargraph_length = 4 ; break ; 
							case 5 : bargraph_length = 5 ; break ; 
							case 6 : bargraph_length = 6 ; break ; 
							case 7 : bargraph_length = 7 ; break ; 
							case 8 : bargraph_length = 8 ; break ; 
							case 9 : bargraph_length = 9 ; break ; 
							default: break ;
						}
						
//						if ((bargraph_length != bargraph->bargraph_value) || (VuMeterWakeUp == 1))
//						{	// write to bargraph if different value only
/*
							if (bargraph_length > bargraph->bargraph_value)
							{
								if (bargraph->bargraph_value < 24)
								{
									delay(attackTime / bargraph->bargraph_steps) ;
									++bargraph->bargraph_value ; // slowly increases the enlighted bargraph length for galvanometer balistic emulation
								}
								bargraph_length = bargraph->bargraph_value ;						}
							else 
							{
								if ((bargraph->bargraph_value > 0) && (bargraph_length < bargraph->bargraph_value)) 
								{ 
									delay(releaseTime / bargraph->bargraph_steps) ;
									--bargraph->bargraph_value ; // slowly decreases the enlighted bargraph length for galvanometer balistic emulation
								}  
								bargraph_length = bargraph->bargraph_value ;
							}
*/							
							// display enlight LEDs depending bargraph_length from 0 to 24, then enlight a last RED LED if value = 255
							if (bargraph_length == 0)
							{
								suplBits = 0 ;
							}
							else if (bargraph_length == 1 | bargraph_length == 5 | bargraph_length ==  9 | bargraph_length == 13 | bargraph_length == 17 | bargraph_length == 21)
							{
								suplBits = 0b1 ;
							}
							else if (bargraph_length == 2 | bargraph_length == 6 | bargraph_length == 10 | bargraph_length == 14 | bargraph_length == 18 | bargraph_length == 22)
							{
								suplBits = 0b11 ;
							}
							else if (bargraph_length == 3 | bargraph_length == 7 | bargraph_length == 11 | bargraph_length == 15 | bargraph_length == 19 | bargraph_length == 23)
							{
								suplBits = 0b111 ;
							}
							else if (bargraph_length == 4 | bargraph_length == 8 | bargraph_length == 12 | bargraph_length == 16 | bargraph_length == 20 | bargraph_length == 24)
							{
								suplBits = 0b1111 ;
							}
							
	//						printf("\n === value:%d - ratio:%f - dBu:%f - bargraph_length:%d - suplBits:%d ===", value, ratio, dB, bargraph_length, suplBits) ;
	
							// no RED LEDs (all of them OFF)
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x00 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x02 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
							bargraphRedBlackout = 0b00000000 ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraphRedBlackout) ; delay(I2C_DELAY) ;
	
							//GREEN LEDs now, depending the signal value
							if (bargraph_length < 5)
							{
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}			
							else if (bargraph_length > 4 && bargraph_length < 9)
							{
								bargraph_value = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00000000 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 8 && bargraph_length < 13)
							{
								bargraph_value = 0b00001111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00000000 << bargraph_length) | suplBits ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 12 && bargraph_length < 17)
							{							
								bargraph_value = ((0b00001111 << (bargraph_length -12)) | suplBits) ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 16 && bargraph_length < 21)
							{	
								bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = (0b00001111 << (bargraph_length -16) | suplBits) ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b00001111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
							else if (bargraph_length > 20 && bargraph_length < 25)
							{
								bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = ((0b00001111 << (bargraph_length -20)) | suplBits) ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
							}	
/*							if (value == 255)
							{ 	
	//							bargraph_value = 0b11111111 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
	//							bargraph_value = 0b11111111 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b01111111 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ;
								bargraph_value = 0b10000000 ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, bargraph_value) ; delay(I2C_DELAY) ; // the last RED LED point if 255
							}
*/
							VuMeterWakeUp = 0 ;
//						}	
					}
					break ;
				}
						
				printf("\n not an expected case for bargraph management ! \n") ;
				break ;
			}
		}
	}
	return x ;
}

//======================================================================

void bargraphBlackOut(int setUpIO)
{	
	int loop = 0 ;
	int value = 0x00 ; 
	
	for(; loop < 6; loop++)
	{
		wiringPiI2CWriteReg8(setUpIO, loop, value) ;
		delay(I2C_DELAY) ;
	}
}

//======================================================================

int bargraphInit(void)
{
	printf("bargraphInit start...") ;
	
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
			printf(" - LAS VEGAS !...") ;
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
			
			// a long LEDs segment with green color, and red at final :
			bargraphBlackOut(bargraph->bargraph_setUpIO) ;
			delay(30) ;
			int loop = 0 ;
			unsigned int value = 0b00000000 ;
			for (;loop < 4;loop++)
			{
				value = (value << 1) + 1 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ; delay(I2C_DELAY) ;
				delay(30) ;
			}			
			value = 0b00000000 ;
			for (;loop < 8;loop++)
			{
				value = (value << 1) + 1 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ; delay(I2C_DELAY) ;
				delay(30) ;
			}	
			value = 0b00000000 ;
			for (;loop < 12;loop++)
			{
				value = (value << 1) + 1 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ; delay(I2C_DELAY) ;
				delay(30) ;
			}	
			value = 0b00001111 ;
			for (;loop < 16;loop++)
			{
				value = (value << 1) + 1 ; instructionByte = 0x01 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ; delay(I2C_DELAY) ;
				delay(30) ;
			}	
			value = 0b00001111 ;
			for (;loop < 20;loop++)
			{
				value = (value << 1) + 1 ; instructionByte = 0x03 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ; delay(I2C_DELAY) ;
				delay(30) ;
			}	
			value = 0b00001111 ;
			for (;loop < 24;loop++)
			{
				value = (value << 1) + 1 ; instructionByte = 0x05 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ; delay(I2C_DELAY) ;
				delay(30) ;
			}	
			loop = 24 ;
			if (loop == 24)
			{ 	
				value = 0b10000000 ; instructionByte = 0x04 ; wiringPiI2CWriteReg8(bargraph->bargraph_setUpIO, instructionByte, value) ; delay(I2C_DELAY) ; 
				delay(30) ;
			}
				
			printf(" - bargraphInit terminated...\n\n") ;
			
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
	newbargraph->bargraph_reversed = bargraph_reversed ; // 0 = no - 1 = yes - left to right or right to left, depending hardware integration in a front pannel
	newbargraph->bargraph_value = 0 ;                    // memory, for balistic use (decay tempo, etc...) 
		
	setUpIO = wiringPiI2CSetup(bargraph_address) ; // I2C init, return the standard Linux file number to handle the I2C chip
	if (setUpIO < 0) { printf("setUpIO: Unable to intialise I2C: %s\n", strerror(errno)) ;	}
//	printf(">>> bargraph_setUpIO: %d \n",setUpIO) ;
	newbargraph->bargraph_setUpIO = setUpIO ;
	 
	return newbargraph ;
}

