/* digital-pot.c :
 * Permits to drive a gang of I2C digital pots behind a Raspberry Pi.
 * To be used with rotaryencoder.c and rotaryencoder.h library.
 * Permits to test the "rotaryencoder" and "I2C-Display" libraries.
 * V.1.0.0
 */

/*=======================================================================\
|      - Copyright (c) - 2015 October- F6HQZ - Francois BERGERET -       |
|                                                                        |
| This "digital-pots.c" files can run only with the                      |
| necessary and excellent wiringPi tools suite for Raspberry Pi from the |
| "Gordons Projects" web sites from Gordon Henderson :                   |
| https://projects.drogon.net/raspberry-pi/wiringpi/                     |
| http://wiringpi.com/                                                   |
|                                                                        |
| My library permits an easy use of few rotary encoders with push switch |
| in ther axe and use them as "objects" stored in structures. Like this, |
| they are easy to read or modify values and specs from anywhere in your |
| own software which must use them.                                      |
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
#include <math.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "digital-pot.h"

struct digipot *setupdigipot(char *digipot_bus_type, int digipot_address, 
	char digipot_channels, char *digipot_reference,	int digipot_ohms, int wiper_positions, 
	char *digipot_label0, char *digipot_curve0, char *digipot_0_position0,
	char *digipot_label1, char *digipot_curve1, char *digipot_0_position1, 
	char *digipot_label2, char *digipot_curve2, char *digipot_0_position2,
	char *digipot_label3, char *digipot_curve3, char *digipot_0_position3,
	char *digipot_label4, char *digipot_curve4, char *digipot_0_position4,
	char *digipot_label5, char *digipot_curve5, char *digipot_0_position5,
	char *digipot_label6, char *digipot_curve6, char *digipot_0_position6,
	char *digipot_label7, char *digipot_curve7, char *digipot_0_position7) ; 

// don't change these init values :
int numberofdigipots = 0 ; // as writed, number of digipots, will be modified by the code later

int updateOneDigipot(char *digipot_label, int wiper_value) ; // store all values in Raspi RAM
double digipotRead(char *digipot_label) ; // Read from digipot
int digipotWrite(char *digipot_label) ; // Write to digipot
int checkOneDigipot(char *digipot_label) ; // for chip tests only

//======================================================================

int updateOneDigipot(char *digipot_label, int wiper_value)
{
//	printf("!!! digipot: %s - value: %d \n", digipot_label, wiper_value) ;
	struct digipot *digipot = digipots ;	
	for (; digipot < digipots + numberofdigipots ; digipot++)
	{
		int loop = 0 ;
		int slaveAddressByte ;
		int instructionByte ;
		float ratio ;
		float dB ;
				
		for (; loop < digipot->digipot_channels ; loop++)
		{
			if (digipot_label == digipot->digipot_label[loop])
			{
				int tap = -(wiper_value - digipot->wiper_positions) ;
				
				if (digipot->digipot_reference == "AD5263") // chip from Analog Device
				{
					slaveAddressByte = digipot->digipot_address << 1 | 0b0 ; // prepare the first byte including the internal sub digipot address, only for displaying and tests, because it's sent automaticaly by wiringpi itself, don't care !	
					instructionByte = loop << 5 ; // this is the "int reg", the second I2C byte sent by wiringpi
					//wiringPiI2CWriteReg8(digipot->digipot_setUpIO, instructionByte, wiper_value) ; // send the complete I2C frame to the chip
					//digipot->digipot_value[loop] = wiper_value ; // store the wiper value in memory
				}	
				
				// convert digipot tap position to attenuation in dB
				if (digipot->digipot_0_position[loop] == "RIGHT")
				{
					ratio = (float) (digipot->wiper_positions - tap) / (digipot->wiper_positions -1) ;
					digipot->digipot_value[loop] = wiper_value ; // store the wiper value in memory
					wiringPiI2CWriteReg8(digipot->digipot_setUpIO, instructionByte, wiper_value) ; // send the complete I2C frame to the chip
				}
				else if (digipot->digipot_0_position[loop] == "CENTER")
				{
					if (wiper_value >= 0)
					{
						ratio = (float) digipot->wiper_positions / (digipot->wiper_positions - wiper_value) ;
						digipot->digipot_value[loop] = wiper_value + (digipot->wiper_positions / 2) -1 ; // store the wiper value in memory
						wiringPiI2CWriteReg8(digipot->digipot_setUpIO, instructionByte, wiper_value + (digipot->wiper_positions / 2) - 1) ; // send the complete I2C frame to the chip
					}
					else
					{
						ratio = (float) (wiper_value + digipot->wiper_positions) / (digipot->wiper_positions) ;
						digipot->digipot_value[loop] = wiper_value + (digipot->wiper_positions / 2) - 1 ; // store the wiper value in memory
						wiringPiI2CWriteReg8(digipot->digipot_setUpIO, instructionByte, wiper_value + (digipot->wiper_positions / 2) - 1) ; // send the complete I2C frame to the chip
					}	
				}
				else
				{
					printf("\n!!! ZERO position value not recognized !!!\n") ;
					printf("ELSE-digipot->digipot_0_position[loop]:%s",digipot->digipot_0_position[loop]) ;
				}
				dB = (20 * log10(ratio)) ;
				digipot->digipot_att[loop] = dB ; // store the digipot attenuation in dB
				//printf("\n === Loop:%d - Label:%s - Positions:%d - RotaryEncoderValue:%d - Ratio:%f - dB:%f - DigipotValue:%d \n",loop,digipot->digipot_label[loop],digipot->wiper_positions,wiper_value,ratio,dB,digipot->digipot_value[loop]) ;
					
				//printf("\n>>> Digipot Read response : x:%d - tap:%0.0f - att:%2.2f(dB) \n", x, tap, dB) ;
				
				break ; // digipot found, stop searching
			}
		}
	}
					
	return wiper_value ;
}

//======================================================================

double digipotRead(char *digipot_label)
{
	double x = -1 ;
	struct digipot *digipot = digipots ;	
	for (; digipot < digipots + numberofdigipots ; digipot++)
	{
		int loop = 0 ;
		int found = 0 ;
		for (; loop < digipot->digipot_channels ; loop++)
		{
			int loop = 0 ; 
			if (digipot_label == digipot->digipot_label[loop])
			{
//				printf("*** digipot_label: %s - digipot->digipot_label: %d - loop: %d \n", digipot_label, digipot->digipot_label, loop) ;
				int slaveAddressByte = digipot->digipot_address << 1 | 0b0 ; // prepare the first byte including the internal sub digipot address, only for displaying and tests, because it's sent automaticaly by wiringpi itself, don't care !
				int instructionByte = loop << 5 ; // this is the "int reg", the second I2C byte sent by wiringpi
				wiringPiI2CWriteReg8(digipot->digipot_setUpIO, instructionByte, digipot->digipot_value[loop]) ; // send the complete I2C frame to the chip, rewrite the current wipper value, because the READ instruction get the last writed digipot
				
				int x = -1 ;
				x = wiringPiI2CRead(digipot->digipot_setUpIO) ;	
				
				if (x > -1)
				{	// convert tap position to attenuation in dB
					double tap = -(x - digipot->wiper_positions) ;
					double ratio ;
					if (digipot->digipot_0_position[loop] == "RIGHT")
					{
						ratio = ((digipot->wiper_positions - tap) / (digipot->wiper_positions -1)) ;
					}
					else if (digipot->digipot_0_position[loop] == "CENTER")
					{
						ratio = ((digipot->wiper_positions - tap) / ((digipot->wiper_positions/2) -1)) ;
					}
					else
					{
						printf("\n!!! ZERO position value not recognized !!!\n") ;
						printf("ELSE-digipot->digipot_0_position[loop]:%s",digipot->digipot_0_position[loop]) ;
					}
					double dB = (20 * log10(ratio)) ;
					digipot->digipot_att[loop] = dB ; // store the digipot attenuation in dB
//					printf("\n>>> Digipot Read response : x:%d - tap:%-0.0f - att:%0.2f(dB) \n", x, tap, dB) ;
					
/*					printf(">>> Digipot Read addr: Ox%x = %d - setUpIO: 0x%x = %d - slaveAddressByte: 0x%x = %d - instructionByte: 0x%x = %d - dataByte/att: 0x%x = %3.2f(dB) \n", 
							digipot->digipot_address, digipot->digipot_address, digipot->digipot_setUpIO, digipot->digipot_setUpIO, slaveAddressByte, slaveAddressByte, instructionByte, instructionByte, x, dB) ;
*/
				}
				else
				{
					printf("Digipot Read error") ;
				}
				found = 1 ;
//				printf("found digipot") ;
				break ;
			}
//			printf("boucle 1 \n") ;
			if (found == 1) 
			{
				break ;
			}
		}
//		printf("boucle 2 \n") ;
		if (found == 1) 
		{
			break ;
		}
	}
//	printf("sortie de boucle 2 \n") ;
	return x ;
}

//======================================================================

int checkOneDigipot(char *digipot_label) // for chip tests only
{
	int wiper_value = -1 ;
	
	struct digipot *digipot = digipots ;	
	for (; digipot < digipots + numberofdigipots ; digipot++)
	{
		int found = 0 ;
		int loop = 0 ;
		for (; loop < digipot->digipot_channels ; loop++)
		{
			if (digipot_label == digipot->digipot_label[loop])
			{
				if (digipot->digipot_reference == "AD5263") // chip from Analog Device
				{
					int slaveAddressByte = digipot->digipot_address << 1 | 0b0 ; // prepare the first byte including the internal sub digipot address, only for displaying and tests, because it's sent automaticaly by wiringpi itself, don't care !	
					int instructionByte = loop << 5 ; // this is the "int reg", the second I2C byte sent by wiringpi
					printf("\n * checking values of DIGIPOT:[%d]: \"%s\" \n", loop, digipot->digipot_label[loop]) ;
					
					wiper_value = 0 ;
					for (; wiper_value < 256 ; wiper_value++)
					{
						wiringPiI2CWriteReg8(digipot->digipot_setUpIO, instructionByte, wiper_value) ; // send the complete I2C frame to the chip
						if (wiper_value == 0) { delay(1500) ; } else { delay(10) ; } // let the time to any mechanical galvanometer to reset for low values measurement
					}
					printf("-- done --\n\n") ;
				}
				
				found = 1 ;
				break ;	
			}
		}
		if (found == 1) { break ; }
	}
	return wiper_value ;
}

//======================================================================

struct digipot *setupdigipot(char *digipot_bus_type, int digipot_address, 
	char digipot_channels, char *digipot_reference,	int digipot_ohms, int wiper_positions, 
	char *digipot_label0, char *digipot_curve0, char *digipot_0_position0,
	char *digipot_label1, char *digipot_curve1, char *digipot_0_position1, 
	char *digipot_label2, char *digipot_curve2, char *digipot_0_position2,
	char *digipot_label3, char *digipot_curve3, char *digipot_0_position3,
	char *digipot_label4, char *digipot_curve4, char *digipot_0_position4,
	char *digipot_label5, char *digipot_curve5, char *digipot_0_position5,
	char *digipot_label6, char *digipot_curve6, char *digipot_0_position6,
	char *digipot_label7, char *digipot_curve7, char *digipot_0_position7)
{
	if (numberofdigipots > MAX_DIGIPOTS)
	{
		printf("Maximum number of digipots exceded: %i\n", MAX_DIGIPOTS) ;
		return NULL ;
	}
	
	struct digipot *newdigipot = digipots + numberofdigipots++ ;
	newdigipot->digipot_bus_type = digipot_bus_type ;    // 0 for I2C or 1 for SPI
	newdigipot->digipot_address = digipot_address ;      // address of the chip on the I2C or SPI bus
	newdigipot->digipot_reference = digipot_reference ;  // digipot provider reference
	newdigipot->digipot_ohms = digipot_ohms ;            // RAB potentiometer resistor value
	newdigipot->wiper_positions = wiper_positions ;      // 128 256 512 1024 positions from 0 to max value (A to B pot connectors)
	newdigipot->digipot_channels = digipot_channels ;	 // number of independant digipots in the same IC : single, dual, quad, octo...
	
	int loop = 0 ;
	for (; loop < digipot_channels ; loop++)
	{
		printf("loop:%d",loop) ;
		switch(loop)
		{
			case 0:
				newdigipot->digipot_label[0] = digipot_label0 ;           // 8 labels for names for each independant pots in the same IC
				newdigipot->digipot_curve[0] = digipot_curve0 ;           // linear ,log, antilog, whatever described in the library
				newdigipot->digipot_0_position[0] = digipot_0_position0 ; // ZERO dB position
				break ;
			case 1:
				newdigipot->digipot_label[1] = digipot_label1 ; 
				newdigipot->digipot_curve[1] = digipot_curve1 ;
				newdigipot->digipot_0_position[1] = digipot_0_position1 ;
				break ;
			case 2:
				newdigipot->digipot_label[2] = digipot_label2 ; 
				newdigipot->digipot_curve[2] = digipot_curve2 ;
				newdigipot->digipot_0_position[2] = digipot_0_position2 ;
				break ;
			case 3:        
				newdigipot->digipot_label[3] = digipot_label3 ;
				newdigipot->digipot_curve[3] = digipot_curve3 ;
				newdigipot->digipot_0_position[3] = digipot_0_position3 ;
				break ;
			case 4:
				newdigipot->digipot_label[4] = digipot_label4 ;
				newdigipot->digipot_curve[4] = digipot_curve4 ;
				newdigipot->digipot_0_position[4] = digipot_0_position4 ;
				break ;
			case 5:
				newdigipot->digipot_label[5] = digipot_label5 ;
				newdigipot->digipot_curve[5] = digipot_curve5 ;
				newdigipot->digipot_0_position[5] = digipot_0_position5 ; 
				break ;
			case 6:
				newdigipot->digipot_label[6] = digipot_label6 ;
				newdigipot->digipot_curve[6] = digipot_curve6 ;
				newdigipot->digipot_0_position[6] = digipot_0_position6 ;
				break ;
			case 7:
				newdigipot->digipot_label[7] = digipot_label7 ;
				newdigipot->digipot_curve[7] = digipot_curve7 ; 
				newdigipot->digipot_0_position[7] = digipot_0_position7 ; 
				break ;
			default:
				printf("more than 8 channel !") ;
				break ;   
		}     
	}

	loop = 0 ;
	for (; loop < digipot_channels ; loop++)
	{
		newdigipot->wiper_memo[loop] = 0 ; // record the last position before to shutdown, to restore value at restarting
		newdigipot->digipot_value[loop] = 0 ; // current digipot register value for each channel
		newdigipot->digipot_att[MAX_POT_CHIP] = 0 ; // in dB, current attenuation, calculated from the current digipot_value
//		printf("newdigipot loop:%d - \"%s\" - memo:%d - value:%d  \n", loop, newdigipot->digipot_label[loop], newdigipot->wiper_memo[loop], newdigipot->digipot_value[loop]) ;
	}
	
	setUpIO = wiringPiI2CSetup(digipot_address) ; // I2C init, target to selected chip
	if (setUpIO < 0) { printf("setUpIO: Unable to intialise I2C: %s\n", strerror(errno)) ;	}
//	printf(">>> setUpIO: %d \n",setUpIO) ;
	newdigipot->digipot_setUpIO = setUpIO ;
	 
	return newdigipot ;
}

