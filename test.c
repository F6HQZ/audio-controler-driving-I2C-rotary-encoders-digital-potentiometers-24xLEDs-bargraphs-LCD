/* test.c :
 * Permits to test the "rotaryencoder", "I2C-Display", "bargraph" and
 * "digital-pot" libraries.
 * No more interest, except curiosity or learning.
 * You must use your own to integrate that workshop into your audio project.
 * V.1.2.0
 */

/*=======================================================================\
|      - Copyright (c) - 2015 August - F6HQZ - Francois BERGERET -       |
|                                                                        |
| rotaryencoder.c and rotaryencoder.h files can run only with the        |
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
 
 /*
  * At starting, the LEDs will be enlighted (ON) few seconds for check,
  * then a full list of rotary encoders parameters/caracteristics will
  * be displayed, and axial push buttons included if any.
  * Then, two lines will be displayed reporting all the values status.
  * Each modification of any will display two new lines of status.
  * Not beautifull, but efficient to check the library, the components
  * and your wiring. ;-)  Each switch must be at ground during the "ON",
  * pullup resistors are programmed inside the GPIO, don't add them.
  * Enjoy !
  */
  
// to compile, from linux console, when in the source directory :
// "gcc -lwiringPi test.c -o test"
// then, input "./test" to execute the compiled program.  

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#include <wiringPi.h>

#include "binary-values.h"

#include "rotaryencoder.c"
#include "I2C-Display.c"
#include "digital-pot.c"
#include "bargraph.c"

// used GPIO for 2 UP/DOWN monitor flashing LEDs (arbitrary, you can change as desired)
#define	LED_DOWN	25
#define	LED_UP		29

// used PWM GPIO for LED dim demo with the first rotary encoder
#define PWM_LED 	1
// the first encoder is used to play with PWM_LED dim, "1" is mandatory: only one PWM output !
// steps are from 0 to 1024 for no light to max.

extern int speed ;

void pwmWrite (int pin, int value) ;

extern void displayInit (void) ;
extern void displayShow (char *line1, char *line2) ;
extern int bargraphInit (void) ;

int main (void)
{
	printf("\n * ROTARY ENCODER DEMO SOFTWARE FOR RASPBERRY PI * \n\n");

	wiringPiSetup () ;
	
	displayInit() ;

/*	LEDs (outputs)
	enlighted for 2 sec at starting to check them, 
	then when moving values up or down 
*/
	pinMode (25,OUTPUT) ;			// output to drive LED
	pinMode (29,OUTPUT) ;			// output to drive LED
	digitalWrite (25,ON) ;			// ON
	digitalWrite (29,ON) ;			// ON
	pinMode (PWM_LED, PWM_OUTPUT) ; // pin 1 is the only one PWM capable pin on RapsberryPi pcb
	pinMode (PWM_LED,1024) ; 		// Max bright value at starting
	delay (2000) ;					// mS
	digitalWrite (25,OFF) ;			// OFF
	digitalWrite (29,OFF) ;			// OFF
 
 /*
 *  Please, see variables meaning in the rotaryencoder.c and rotaryencoder.h files
 *  and adapt them to your case, suppress or add the neccessary encoders and witches
 *  depending your project, in this two following structure types (one "object" each line) :
 */
 
 	printf("\nrotary encoders declaration start \n") ;
	// rotary encoders declaration :
	struct encoder *encoder = 
	setupencoder ("GAIN","DIGIPOTGAIN",0,2,YES,NO,NO,0,255,50,500000,40000,25000,10000,10,75,200) ;  // pins 0 and 2
	setupencoder ("TONE","DIGIPOTTONE",3,4,YES,NO,NO,0,255,25,500000,30000,15000,6000,10,25,50) ;  // pins 3 and 4
	setupencoder ("VOLUME","BARGRAPH",5,6,YES,NO,NO,0,0xffff,0,500000,30000,15000,6000,50,500,5000) ;  // pins 5 and 6
	printf("rotary encoders declaration end \n") ;
	
	printf("buttons declaration start \n") ;
	// their axis buttons (or any buttons) are there :
	struct button *button = 
	setupbutton("GAIN",7,1) ;  // pin  7 and ON  at starting
	setupbutton("TONE",21,0) ;   // pin 21 and OFF at starting
	setupbutton("VOLUME",22,0) ;  // pin 22 and OFF at starting
	printf("buttons declaration end \n") ;
	
	printf("digipots declaration start \n") ;
	// digipots declaration :
	struct digipot *digipot = 
	setupdigipot("0",0x2c,4,"AD5293",20000,256,"DIGIPOTGAIN","DIGIPOTTONE","DIGIPOTVOLUME","DIGIPOTTREMOLO","","","","") ; // 0=I2C (1=SPI), addr, channels, ref, Ohms, positions, name#1, name#2, name#3, name#4, ...
//	setupdigipot("0",0x70,1,"HP16K33",20000,0xffff,"BARGRAPH","","","","","","","") ; // 0=I2C (1=SPI), addr, channels, ref, Ohms, positions, name#1, name#2, name#3, name#4, ...
	printf("digipots declaration end \n") ;
	
	printf("bargraphs declaration start \n") ;
	// bargraphs declaration :
	struct bargraph *bargraph = 
	setupbargraph("BARGRAPH",0x70,"adafruit1721",24,2,NO) ; // name, I2C address, ref, LEDs number, colors
	printf("bargraphs declaration end \n") ;
		
	extern numberofencoders ;
	extern numberofbuttons ;
	extern numberofdigipots ;
	extern numberofbargraphs ;
	
	long int memo_rotary[numberofencoders] ; // record the rotary encoder value for modification detection later
	long int memo_button[numberofbuttons] ;  // record the button value for modification detection later

	//display to HDMI screen the components list :
	printf("\nDIGIPOTS list :\n-----------------\n") ;
	for (; digipot < digipots + numberofdigipots ; digipot++)
	{ 
		int loop = 0 ;
		for (; loop < digipot->digipot_channels ; loop++)
			{
				printf("DIGIPOT:[%d]: \"%s\" \n", loop, digipot->digipot_label[loop]) ;
			}
		printf("BUS Type: %s \n address: %d \n chipset ref: %s \n R value: %d \n Wiper positions: %d \n channels: %d \n mem address: %d \n-----------------\n", 
			digipot->digipot_bus_type, digipot->digipot_address, digipot->digipot_reference, digipot->digipot_ohms, digipot->wiper_positions, digipot->digipot_channels, digipot) ; 
	}
	printf("\nROTARY ENCODERS list :\n-----------------\n") ;
	for (; encoder < encoders + numberofencoders ; encoder++)
	{
		printf("Label:\"%s\" \n driver_Entity:\"%s\" \n pin A: %d \n pin B: %d \n mem address: %d \n full sequence each step: %d \n reverse rotation: %d \n looping if limit reached: %d \n low_Limit value: %d \n high_Limit value: %d \n operator rotation pause duration detection (time between two steps in microsec): %d \n speed_Level_Threshold_2 (time between two steps in microsec): %d \n speed_Level_Multiplier_2: %d \n speed_Level_Threshold_3 (time between two steps in microsec): %d \n speed_Level_Multiplier_3: %d \n speed_Level_Threshold_4 (time between two steps in microsec): %d \n speed_Level_Multiplier_4: %d \n-----------------\n",
			encoder->label, encoder->drived_Entity, encoder->pin_a, encoder->pin_b, encoder, encoder->sequence, encoder->reverse, encoder->looping, encoder->low_Limit, encoder->high_Limit, encoder->pause, encoder->speed_Level_Threshold_2, encoder->speed_Level_Multiplier_2, encoder->speed_Level_Threshold_3, encoder->speed_Level_Multiplier_3, encoder->speed_Level_Threshold_4, encoder->speed_Level_Multiplier_4) ;
	}
	printf("\nBUTTONS list :\n-----------------\n") ;
	for (; button < buttons + numberofbuttons ; button++)
		{ printf("Label:\"%s\" \n pin: %d \n mem address: %d \n-----------------\n", button->label, button->pin, button) ; }

	printf("\n Two LEDs must be connected at #25 and #29 pins if you want to observe the rotation direction (normal or reverse). \n") ;
	printf(" The positive pin of the LED is to connect to the Raspi output pin, the negative pin of the LED to the minus (the \"ground\" or \"0V\"),\n but a serial resistor of about 1kOhms must be inserted to limit the current.") ;
	printf("\n The first rotary encoder is used to modify the PWM-LED dim which must be connected on pin #1, not another, which s the only one PWM capable. \n") ;
	printf("\n Some rotary encoders own a push button in their axis to trigger some extra feature, \n as to load or save something in memory, or change the feature to modify, etc... \n\n") ;

	int x = bargraphInit() ;
	if (x < 0) { printf("bargrapInit: Unable to intialise: %s\n", strerror(errno)) ; }

	while (1)
	{
		delay (10) ; // 10 ms default, decreases the loop speed (and the CPU load from about 25% to minus than 0.3%)
		digitalWrite (LED_DOWN, OFF) ;	// OFF
		digitalWrite (LED_UP, OFF) ; 	// OFF

		int step = 0 ;
		unsigned char print = 0 ;
		
		// check if it's time to cut off the backlight of the LCD
		if ( (backlightTempo != 0) && ((millis() - backlightTimer) > backlightTempo) && (backlightStatus == 1) )
		{
			digitalWrite(AF_BL, OFF) ; delay(LCD_DELAY) ;	// put Backlight OFF
			backlightStatus = OFF ;
		}
		
		// check if any rotary encoder modified value
		struct encoder *encoder = encoders ;
		for (; encoder < encoders + numberofencoders ; encoder++)
		{
			if (encoder->value != memo_rotary[step])
			{	
				printf("encoder->active_flag = %d \n", encoder->active_flag) ;
				print = 1 ;
				memo_rotary[step] = encoder->value ;
				updateOneDigipot(encoder->drived_Entity, encoder->value) ;
				bargraphWrite("BARGRAPH", encoder->value) ;
//				printf("%s - step:%d - memo:%d - encoder->value:%d \n", encoder->label, step, memo_rotary[step], encoder->value) ;

				// display to LCD
				char textBuffer[16] ;
				sprintf(textBuffer, "%d", encoder->value) ;
				displayShow(encoder->label, textBuffer) ;
				
			}				
			
			++step ;	
		} 
		
		step = 0 ;
		
		// check if any button modified value
		struct button *button = buttons ;
		for (; button < buttons + numberofbuttons ; button++)
		{
			int found = 0 ;
			if (button->value != memo_button[step])
			{	// found one button which has a modified value
			
				struct encoder *encoder = encoders ;
				for (; encoder < encoders + numberofencoders ; encoder++)
				{  					
					if (encoder->label == button->label)
					{	// found the attached rotary encoder
						printf("+++ DIGIPOT: \"%s\" \n", encoder->drived_Entity) ;
						int x = digipotRead(encoder->drived_Entity) ; // read the attached digipot
						found = 1 ;
						break ;
					}
				}				
			
				print = 1 ;
				memo_button[step] = button->value ;
				
				// display to LCD
				char textBuffer[16] ;
				sprintf(textBuffer, "%d", encoder->value) ;
//				sprintf(textBuffer, "%d", button->value) ; // push button name + 1 or 0 depending the button status
				displayShow(encoder->label, textBuffer) ;
				bargraphWrite("BARGRAPH", encoder->value) ;
			}		
			
			if (found == 1) 
			{
				break ;
			}
			++step ;	
		}
		
		//check if any rotary encoder is moving or button pressed and turn display light on if any
		if (touched == 1)
		{
			displayShow("", "") ; // restart backlight
			touched = 0 ;
		}
		
		// and if any value modified, then display the new value (and all others too)
		if (print) 
		{
			struct encoder *encoder = encoders ;
			for (; encoder < encoders + numberofencoders; encoder++)
			{
				// encoder pins, name, address in memory, current value 
				printf("A:%d B:%d \"%s\"[%d]:%-5d ", encoder->pin_a, encoder->pin_b, encoder->label, encoder, encoder->value) ;
				if (encoder == encoders)
				{ // first encoder is "reserved" for PWM_LED dim demo
					pwmWrite (PWM_LED, encoder->value) ;
				}
			}
			// current rotation speed, last pause duration, number of eliminated bounces from starting
			printf("- speed: %-4d - gap: %-10d \nBUTTONS:\n", speed, gap) ;
			
			struct button *button = buttons ;
			for (; button < buttons + numberofbuttons ; button++)
			{
				// button pin, name, address in memory, current value
				printf("\"%s\"[%d](pin:%d):%d - timestamp:%d - previous_timestamp:%d - duration:%d \n", 
					button->label, button, button->pin, button->value, button->timestamp, 
					button->previous_timestamp, button->timestamp - button->previous_timestamp) ; 
			}
			printf("For all, cancelled bounces: %-5d \n\n", bounces) ;
			
			int x = -1 ;
			struct digipot *digipot = digipots ;	
			for (; digipot < digipots + numberofdigipots ; digipot++)
			{
				int loop = 0 ;
				int found = 0 ;
				for (; loop < digipot->digipot_channels ; loop++)
				{
					int slaveAddressByte = digipot->digipot_address << 1 | 0b0 ; // prepare the first byte including the internal sub digipot address, only for displaying and tests, because it's sent automaticaly by wiringpi itself, don't care !
					int instructionByte = loop << 5 ; // this is the "int reg", the second I2C byte sent by wiringpi
					wiringPiI2CWriteReg8(digipot->digipot_setUpIO, instructionByte, digipot->digipot_value[loop]) ; // send the complete I2C frame to the chip, rewrite the current wipper value, because the READ instruction get the last writed digipot
					
					x = -1 ;
					x = wiringPiI2CRead(digipot->digipot_setUpIO) ;	
					if (x > -1)
					{
						printf(">>> Digipot Read addr: Ox%x = %d - setUpIO: 0x%x = %d - slaveAdressByte: 0x%x = %d - instructionByte: 0x%x = %d - dataByte: 0x%x = %d \n", digipot->digipot_address, digipot->digipot_address, digipot->digipot_setUpIO, digipot->digipot_setUpIO, slaveAddressByte, slaveAddressByte, instructionByte, instructionByte, x, x) ;
					}
					else
					{
						printf("Digipot Read error") ;
					}
				}
			}

			print = 0 ;
		}
	}
	return(0) ;
}