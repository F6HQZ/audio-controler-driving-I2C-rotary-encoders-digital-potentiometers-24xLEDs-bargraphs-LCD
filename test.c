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
// "gcc -lwiringPi -lm test.c -o test"
// then, input "./test" to execute the compiled program.  

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h> // need to add "-l" during compiling phase, to link the "llibm.a" library

#include <wiringPi.h>

#include "binary-values.h"

#include "rotaryencoder.c"
#include "I2C-Display.c"
#include "digital-pot.c"
#include "bargraph.c"
#include "extensions-modules.c"

// used GPIO for 2 UP/DOWN monitor flashing LEDs (arbitrary, you can change as desired)
#define	LED_DOWN	25
#define	LED_UP		29

// used PWM GPIO for LED dim demo with the first rotary encoder
#define PWM_LED 	1
// the first encoder is used to play with PWM_LED dim, "1" is mandatory: only one PWM output !
// steps are from 0 to 1024 for no light to max.

extern int speed ;

int voltmeterInput ; // select one of the 4 AD converter inputs;
unsigned long int voltmeterReadingInterval = 1 ; // microsecondes
unsigned int voltmeterTempo = 500 ; // bargraph is displaying voltmeter measurement a while (ms) after stop to play with knobs (rotary encoders)
unsigned long int voltmeterTimer ; // to let some time between two consecutive measurements
unsigned long int voltmeterTimer2 ; // used to let the Bargraph displaying the Rotary Encoder postion before to display the real time measured audio value, works with "voltmeterTempo"
char VuMeterWakeUp = 0 ;

void pwmWrite (int pin, int value) ;

extern void displayInit (void) ;
extern void displayShow (char *line1, char *line2) ;
extern int bargraphInit (void) ;

// =====================================================================

int main (void)
{	
	printf("\n * ROTARY ENCODER DEMO SOFTWARE FOR RASPBERRY PI * \n\n");

	wiringPiSetup () ;
	
	displayInit() ;
	
/*	LEDs (outputs)
	enlighted for 2 sec at starting to check them, 
	then when moving values up or down 

	pinMode (25,OUTPUT) ;			// output to drive LED
	pinMode (29,OUTPUT) ;			// output to drive LED
	digitalWrite (25,ON) ;			// ON
	digitalWrite (29,ON) ;			// ON
	pinMode (PWM_LED, PWM_OUTPUT) ; // pin 1 is the only one PWM capable pin on RapsberryPi pcb
	pinMode (PWM_LED,1024) ; 		// Max bright value at starting
	delay (2000) ;					// mS
	digitalWrite (25,OFF) ;			// OFF
	digitalWrite (29,OFF) ;			// OFF
*/

 /*
 *  Please, see variables meaning in the rotaryencoder.c and rotaryencoder.h files
 *  and adapt them to your case, suppress or add the neccessary encoders and witches
 *  depending your project, in this two following structure types (one "object" each line) :
 */
 
 	printf("rotary encoders declaration start \n") ;
	// rotary encoders declaration :
	struct encoder *encoder = 
	setupencoder ("GAIN",  "DIGIPOT-GAIN",   0, 1,YES,NO,NO,0,255, 50,500000,30000,15000,6000,10,25,50) ;  // pins 0 and 1, rotary encoder name, what linked digipot
	setupencoder ("VOLUME","DIGIPOT-VOLUME", 2, 3,YES,NO,NO,0,255, 25,500000,30000,15000,6000,10,25,50) ;  // pins 2 and 3
	setupencoder ("GRAVE", "DIGIPOT-GRAVE",  4, 5,YES,NO,NO,0,255,128,500000,30000,15000,6000,10,25,50) ;  // pins 4 and 5
	setupencoder ("MEDIUM","DIGIPOT-MEDIUM", 6, 7,YES,NO,NO,0,255,128,500000,30000,15000,6000,10,25,50) ;  // pins 6 and 7
	setupencoder ("AIGUE", "DIGIPOT-AIGUE", 10,11,YES,NO,NO,0,255,128,500000,30000,15000,6000,10,25,50) ;  // pins 10 and 11
	setupencoder ("BOUCLE","DIGIPOT-BOUCLE",12,13,YES,NO,NO,0,255,  0,500000,30000,15000,6000,10,25,50) ;  // pins 12 and 13
	setupencoder ("SORTIE","DIGIPOT-SORTIE",14,21,YES,NO,NO,0,255,  0,500000,30000,15000,6000,10,25,50) ;  // pins 14 and 21
	setupencoder ("CASQUE","DIGIPOT-CASQUE",22,23,YES,NO,NO,0,255,  0,500000,30000,15000,6000,10,25,50) ;  // pins 22 and 23
	printf("rotary encoders declaration end \n") ;
	
	printf("buttons declaration start \n") ;
	// their axis buttons (or any buttons) are there :
	struct button *button = 
//	setupbutton("Button0",  0,1) ; // pin 0 and pin 1, already used by a rotary encoder
//	setupbutton("Button1",  1,1) ; // already used by a rotary encoder
//	setupbutton("Button2",  2,1) ; // already used by a rotary encoder
//	setupbutton("Button3",  3,1) ; // already used by a rotary encoder
//	setupbutton("Button4",  4,1) ; // already used by a rotary encoder
//	setupbutton("Button5",  5,1) ; // already used by a rotary encoder
//	setupbutton("Button6",  6,1) ; // already used by a rotary encoder
//	setupbutton("Button7",  7,1) ; // already used by a rotary encoder
//	setupbutton("Button8", 10,1) ; // already used by a rotary encoder
//	setupbutton("Button9", 11,1) ; // already used by a rotary encoder
//	setupbutton("Button10",12,1) ; // already used by a rotary encoder
//	setupbutton("Button11",13,1) ; // already used by a rotary encoder
//	setupbutton("Button12",14,1) ; // already used by a rotary encoder
//	setupbutton("Button13",21,1) ; // already used by a rotary encoder
//	setupbutton("Button14",22,1) ; // already used by a rotary encoder
//	setupbutton("Button15",23,1) ; // already used by a rotary encoder
	setupbutton("GAIN",    25,1) ; // pin 25, "1" level at starting (default input level when buton not pulling down when pushed due to pullup resistor presence programmed)
	setupbutton("VOLUME",  26,1) ; 
	setupbutton("BOUCLE",  27,1) ; 
	setupbutton("SORTIE",  28,1) ; 
	setupbutton("CASQUE",  29,1) ; 
	printf("buttons declaration end \n") ;
	
	printf("digipots declaration start \n") ;
	// digipots declaration :
	struct digipot *digipot = 
	setupdigipot("0",0x2c,4,"AD5263",200000,256,"DIGIPOT-GAIN","LIN","DIGIPOT-VOLUME","LOG","DIGIPOT-GRAVE","LIN","DIGIPOT-MEDIUM","LIN","","","","","","","","") ; // 0=I2C (1=SPI), addr, channels, ref, Ohms, positions, name#1, name#2, name#3, name#4, ...
	setupdigipot("0",0x2d,4,"AD5263",200000,256,"DIGIPOT-AIGUE","LIN","DIGIPOT-BOUCLE","LIN","DIGIPOT-SORTIE","LIN","DIGIPOT-CASQUE","LIN","","","","","","","","") ; // 0=I2C (1=SPI), addr, channels, ref, Ohms, positions, name#1, name#2, name#3, name#4, ...
//	setupdigipot("0",0x70,1,"HP16K33",20000,0xffff,"BARGRAPH","","","","","","","","","","","") ; // 0=I2C (1=SPI), addr, channels, ref, Ohms, positions, name#1, name#2, name#3, name#4, ...
	printf("digipots declaration end \n") ;
	
	printf("bargraphs declaration start \n") ;
	// bargraphs declaration :
	struct bargraph *bargraph = 
	setupbargraph("BARGRAPH",0x70,"adafruit1721",24,2,NO) ; // name, I2C address, ref, LEDs number, colors, installed reversed
	printf("bargraphs declaration end \n") ;
	
	printf("A/D D/A converters and Digit I/O extensions modules declaration start \n") ;
	// converters modules declaration :
	struct extension_module *extension_module =
	setupModule("ANALOG-CONVERTER#1","GAIN","VOLUME","SORTIE","CASQUE","","","","","PCF8591","0",0x48,200,4) ; // name, name of chan#0, chan#1, chan#2, chan#3, chan#4, chan#5, chan#6, chan#7, chip type, bus type, bus address, I/O pins base (>64 and different of the others)
	setupModule("DIGITAL-OUTPUT#1","RELAY#1","RELAY#2","RELAY#3","RELAY#4","RELAY#5","RELAY#6","RELAY#7","RELAY#8","PCF8574","0",0x3e,210,8) ; // name, name of chan#0, chan#1, chan#2, chan#3, chan#4, chan#5, chan#6, chan#7, chip type, bus type, bus address, I/O pins base (>64 and different of the others)
	printf("A/D D/A converters and Digit I/O extensions modules declaration end \n") ;
	
	extern numberofencoders ;
	extern numberofbuttons ;
	extern numberofdigipots ;
	extern numberofbargraphs ;
	extern numberofmodules ;
	
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
		printf("BUS Type: %s \n address: %d \n chipset ref: %s \n R value: %d \n Wiper positions: %d \n channels: %d \n mem address: 0x%x \n-----------------\n", 
			digipot->digipot_bus_type, digipot->digipot_address, digipot->digipot_reference, digipot->digipot_ohms, digipot->wiper_positions, digipot->digipot_channels, digipot) ; 
	}
		printf("\nEXTENSIONS-MODULES list :\n-----------------\n") ;
		
	for (; extension_module < modules + numberofmodules ; extension_module++)
	{ 
		int loop = 0 ;
		for (; loop < extension_module->module_channels ; loop++)
			{
				printf("EXTENSIONS-MODULE:[%d]: \"%s\" \n", loop, extension_module->module_input_name[loop]) ;
			}
		printf("BUS Type: %s \n address: %d \n chipset ref: %s \n module label: %s \n mem address: 0x%x \n-----------------\n", 
			extension_module->module_bus_type, extension_module->module_address, extension_module->module_type, extension_module->module_label, extension_module) ; 
	}

	printf("\nROTARY ENCODERS list :\n-----------------\n") ;
	for (; encoder < encoders + numberofencoders ; encoder++)
	{
		printf("Label:\"%s\" \n driver_Entity:\"%s\" \n pin A: %d \n pin B: %d \n mem address: 0x%x \n full sequence each step: %d \n reverse rotation: %d \n looping if limit reached: %d \n low_Limit value: %d \n high_Limit value: %d \n operator rotation pause duration detection (time between two steps in microsec): %d \n speed_Level_Threshold_2 (time between two steps in microsec): %d \n speed_Level_Multiplier_2: %d \n speed_Level_Threshold_3 (time between two steps in microsec): %d \n speed_Level_Multiplier_3: %d \n speed_Level_Threshold_4 (time between two steps in microsec): %d \n speed_Level_Multiplier_4: %d \n-----------------\n",
			encoder->label, encoder->drived_Entity, encoder->pin_a, encoder->pin_b, encoder, encoder->sequence, encoder->reverse, encoder->looping, encoder->low_Limit, encoder->high_Limit, encoder->pause, encoder->speed_Level_Threshold_2, encoder->speed_Level_Multiplier_2, encoder->speed_Level_Threshold_3, encoder->speed_Level_Multiplier_3, encoder->speed_Level_Threshold_4, encoder->speed_Level_Multiplier_4) ;
	}
	printf("\nBUTTONS list :\n-----------------\n") ;
	for (; button < buttons + numberofbuttons ; button++)
		{ printf("Label:\"%s\" \n pin: %d \n mem address: 0x%x \n-----------------\n", button->label, button->pin, button) ; }

	printf("\n Two LEDs must be connected at #25 and #29 pins if you want to observe the rotation direction (normal or reverse). \n") ;
	printf(" The positive pin of the LED is to connect to the Raspi output pin, the negative pin of the LED to the minus (the \"ground\" or \"0V\"),\n but a serial resistor of about 1kOhms must be inserted to limit the current.") ;
	printf("\n The first rotary encoder is used to modify the PWM-LED dim which must be connected on pin #1, not another, which s the only one PWM capable. \n") ;
	printf("\n Some rotary encoders own a push button in their axis to trigger some extra feature, \n as to load or save something in memory, or change the feature to modify, etc... \n\n") ;

	if (bargraphInit() < 0) { printf("bargrapInit: Unable to intialise: %s\n", strerror(errno)) ; } // bargraph init sequence
	
	// =================================================================
	// digipots values test
/*	digipot = digipots ;
	for (; digipot < digipots + numberofdigipots ; digipot++)
	{ 
		printf("-------------- digipots test starting...\n") ;
		int loop = 0 ;
		for (; loop < digipot->digipot_channels ; loop++)
		{
			checkOneDigipot(digipot->digipot_label[loop]) ;
		}
	}
*/

	// =================================================================
	// main loop is starting here :
	while (1)
	{
//		delay (10) ; // 10 ms default, decreasing the loop speed (and the CPU load from about 25% to minus than 0.3%)
		digitalWrite (LED_DOWN, OFF) ;	// OFF
		digitalWrite (LED_UP, OFF) ; 	// OFF

		int step = 0 ;
		unsigned char print = 0 ;
		
		// check if it's time to cut off the backlight of the LCD Display
		if ( (backlightTempo != 0) && ((millis() - backlightTimer) > backlightTempo) && (backlightStatus == ON) )
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
				touched = encoder->label ;
				print = 1 ;
				
				memo_rotary[step] = encoder->value ;
				updateOneDigipot(encoder->drived_Entity, encoder->value) ;
//				printf("%s - step:%d - memo:%d - encoder->value:%d \n", encoder->label, step, memo_rotary[step], encoder->value) ;
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
						touched = encoder->label ;
						print = 1 ;
					
						bargraphWrite("BARGRAPH", encoder->low_Limit, encoder->high_Limit, 0, encoder->value) ;
						
						struct digipot *digipot = digipots ;	
						for (; digipot < digipots + numberofdigipots ; digipot++)
						{
							int loop = 0 ;
							for (; loop < digipot->digipot_channels ; loop++)
							{
								if (encoder->drived_Entity == digipot->digipot_label[loop])
								{	// found the attached digipot, then, display to LCD
									char textBuffer[16] ;
									sprintf(textBuffer, "%-5d%8.2f dB", encoder->value, encoder->value, digipot->digipot_att[loop]) ; // to char type conversion
									displayShow(encoder->label, textBuffer) ;	
								}
							}
						}
					
//						printf("\n +++ reading DIGIPOT: \"%s\" \n", encoder->drived_Entity) ;
						double x = digipotRead(encoder->drived_Entity) ; // read the attached digipot
						found = 1 ;
						break ;
					}
				}				
				print = 1 ;
				memo_button[step] = button->value ;
			}		

			if (found == 1) 
			{
				break ;
			}
			++step ;	
		}	
		
		// modify what is displayed on LCD and BARGRAPH when touched any rotary encoder
		if ((touched != "0") && (touched != "1"))
		{	// printf("\n > touched:%s %d", touched, touched) ;
			struct extension_module *extension_module = modules ;
			for (; extension_module < modules + numberofmodules ; extension_module++)
			{ 	// select what analog input value to use for last "touched" rotary encoder
				int loop = 0 ;					
				for (; loop < MAX_CHANNELS ; loop++)
				{
					if (extension_module->module_input_name[loop] == touched) 
					{
						voltmeterInput = extension_module->module_pinBase + loop ;
						break ;
					} 
				}
				if (voltmeterInput > 0) { break ; }
			}
//			printf("\n >>> touched:%s - voltmeterInput:%d", touched, voltmeterInput) ;
			// display the values on bargraph and LCD
			displayShow("", "") ; // restart immediatly the LCD display backlight without to delete the already displayed info
			struct encoder *encoder = encoders ;
			for (; encoder < encoders + numberofencoders ; encoder++)
			{
				if (encoder->label == touched)
				{	// temporary display encoder value on Bargraph when turning or pushing it
					bargraphWrite("BARGRAPH", encoder->low_Limit, encoder->high_Limit, 0, encoder->value) ; // display encoder position value to Bargraph
					// display digipot position value and attenuation in dB to LCD Display
					struct digipot *digipot = digipots ;	
					for (; digipot < digipots + numberofdigipots ; digipot++)
					{
						int found = 0 ;
						int loop = 0 ;
						for (; loop < digipot->digipot_channels ; loop++)
						{
							if (encoder->drived_Entity == digipot->digipot_label[loop])
							{
								// display to LCD
								char textBuffer[16] ;
								sprintf(textBuffer, "%-5d%8.2f dB", encoder->value, digipot->digipot_att[loop]) ; // to char type conversion
								displayShow(encoder->label, textBuffer) ;	
								found = 1 ;
								break ;
							}
						}
						if (found) { break ; }
					}
					VuMeterWakeUp = 1 ; // to restart to display measured value to bargraph
					break ;
				}
			}
			touched = "0" ; // reset the touched value
			voltmeterTimer2 = millis() ; // reset pause measurement timer
			print = 1 ; // display on video screen for debugging
		}	
		
		// it's time to return to analog measurements
//		if ((backlightStatus == OFF) && ((micros() - voltmeterTimer) > voltmeterReadingInterval))
		if (((millis() - voltmeterTimer2) > voltmeterTempo ) && ((micros() - voltmeterTimer) > voltmeterReadingInterval))
		{	// read the analog input and update the bargraph value
			int peakValue = 0 ;
			int value = 0 ;
//			printf("\n --- touched:%s - bargraphInput:%s - voltmeterInput:%d", touched, bargraphInput, voltmeterInput) ;
			long int sampleDuration = 0 ;
			long int sampleStartingTime = micros() ;
			int loop = 0 ;
			for(; sampleDuration < 5000 ; loop++) // few audio samples for peak detection during a short period (abt 5ms as requested for integration)
			{
				value = analogRead(voltmeterInput) ;
				if (value > peakValue) 
				{ 
					peakValue = value ; // peak memo after few analog samples
				}		
				sampleDuration = micros() - sampleStartingTime ;
//				printf("loop:%d - value:%d - peakValue:%d - ", loop, value, peakValue) ;
			}
			bargraphWrite("BARGRAPH", 0, 255, 2, (long int) peakValue) ; // datas : bargraph name, min, max, VU-Meter type: 1 normal linear with one long green bar zone + 1 small amber bar + 1 small red bar, 2 same but logarythmic, 3 Digital log Peak meter ie green bar and only one RED (for max value)
			voltmeterTimer = micros() ; // reset gap timer
//			printf("\n ~~~ value:%d - sampleDuration:%d", value, sampleDuration) ;
		}	
		
		//--------------------------------------------------------------

		// and if any value modified, then display through HDMI output the new value (and all others too)
		if (print) 
		{
			struct encoder *encoder = encoders ;
			for (; encoder < encoders + numberofencoders; encoder++)
			{
				// encoder pins, name, address in memory, current value 
				printf("\nA:%d B:%d \"%s\"[0x%x]:%-5d ", encoder->pin_a, encoder->pin_b, encoder->label, encoder, encoder->value) ;
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
				printf("\"%s\"[0x%x](pin:%d):%d - timestamp:%d - previous_timestamp:%d - duration:%d \n", 
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
					
//					x = -1 ;
					x = wiringPiI2CRead(digipot->digipot_setUpIO) ;	
					if (x > -1)
					{ 
						// convert tap position to attenuation in dB
						double tap = -(x - digipot->wiper_positions) ;
						double ratio = ((digipot->wiper_positions - tap) / (digipot->wiper_positions -1)) ;
						double dB = (20 * log10(ratio)) ;
						printf(">>> Digipot Read addr: Ox%x=%d - setUpIO: 0x%x=%d - slaveAddrByte: 0x%x=%d - instrucByte: 0x%2x=%3d - dataByte: 0x%2x=%3d - value:%5d - tap:%3.0f - att:%3.2f(dB) \n", 
								digipot->digipot_address, digipot->digipot_address, digipot->digipot_setUpIO, digipot->digipot_setUpIO, slaveAddressByte, slaveAddressByte, instructionByte, instructionByte, x, x, digipot->digipot_value[loop], tap, dB) ; 
					}
					else
					{ 
						printf("Digipot Read error\n") ; 
					}
				}
			}
					
			// read the analog I2C module equiped with a PCF8591
			struct extension_module *extension_module = modules ;
			for (; extension_module < modules + numberofmodules ; extension_module++)
			{ 
				int loop = 0 ;					
				for (; loop < extension_module->module_channels ; loop++)
				{
					int voltmeterInput =  0 ;
					voltmeterInput = extension_module->module_pinBase + loop ;
					
					int value = 0 ;
					int loop = 0 ;
					for(; loop < 20 ; loop++) // few audio samples for peak detection during a short period
					{
						int peakValue = 0 ;
						value = analogRead(voltmeterInput) ;
						if (value > peakValue) 
						{ 
							peakValue = value ; // peak memo during few analog samples
						}		
					}
					if (extension_module->module_type == "PCF8591")
					{
	//					printf ("### Analog Input #%d  %5.2f (peak) \n", voltmeterInput, (double)value * 3.3 / 255) ; // for 3.3V powered
						printf ("### Analog Input #%d  %5.2f (peak) \n", voltmeterInput, (double)value * 5 / 255) ; // for 5V powered
					}
					else if (extension_module->module_type == "PCF8574")
					{
						printf ("### Digital Input #%d  %d \n", voltmeterInput, value) ; 
					}
				} 
			}
			print = 0 ;
		}
				
	}
	return(0) ;
}
