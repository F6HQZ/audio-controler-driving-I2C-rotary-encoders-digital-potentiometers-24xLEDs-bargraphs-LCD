/* digital-pot.h :
 * Permits to drive a gang of I2C digital pots behind a Raspberry Pi.
 * To be used with digital-pot.c rotaryencoder.c and rotaryencoder.h libraries.
 * Permits to test the "rotaryencoder" and "I2C-Display" libraries.
 * V 1.1.0
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
| in ther axis and use them as "objects" stored in structures. Like this,|
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

#include "binary-values.h"

#define MAX_DIGIPOTS 8 // how many digipots handled by this system
#define MAX_POT_CHIP 8 // how many potentiometers in a chip

struct digipot
{
	char *digipot_label[MAX_POT_CHIP] ;             // name or label as "Volume" or "Balance" or "Treble", etc...
	char *digipot_bus_type ;                        // 0 for I2C or 1 for SPI
	int digipot_address ;                           // address of the chip on the I2C or SPI bus
	char *digipot_reference ;                       // digipot provider (factory) reference
	int digipot_ohms ;                              // RAB potentiometer resistor value
	char *digipot_curve[MAX_POT_CHIP] ;             // linear ,log, antilog, whatever described in the library
	char *digipot_0_position[MAX_POT_CHIP] ;        // default ZERO dB gain position, right, centered, left
	int wiper_positions ;                           // 128 256 512 1024 positions from 0 to max value (B to A digipot connectors)
	char digipot_channels ;                         // number of independant digipots in the same chipset : single, dual, quad, octo...
	int wiper_memo[MAX_POT_CHIP] ;                  // record the last position before to shutdown to restore at restarting for each channel
	volatile long int digipot_value[MAX_POT_CHIP] ; // current digipot register value for each channel
	double digipot_att[MAX_POT_CHIP] ;              // in dB, current attenuation, calculated from the current digipot_value
	char *digipot_group_type ;                      // "PARALLEL" or "SERIAL" stack of several digipot to make only one "superdigipot" bloc
	char digipot_group_qty ;                        // number of single digipots grouped there
	char *digipot_single_name[MAX_POT_CHIP] ;       // real name of each single digipot composing the superdigipot stack, from lower to upper if SERIAL	
	int digipot_switch[MAX_POT_CHIP] ;              // address of each switch which turns on each single digipot wiper
	int digipot_setUpIO ;
} ;

struct digipot digipots[MAX_DIGIPOTS] ;

struct digipot *setupdigipot(char *digipot_bus_type, int digipot_address, 
	char digipot_channels, char *digipot_reference,	int digipot_ohms, int wiper_positions, 
	char *digipot_label0, char *digipot_curve0, char *digipot_0_position0,
	char *digipot_label1, char *digipot_curve1, char *digipot_0_position1, 
	char *digipot_label2, char *digipot_curve2, char *digipot_0_position2,
	char *digipot_label3, char *digipot_curve3, char *digipot_0_position3,
	char *digipot_label4, char *digipot_curve4, char *digipot_0_position4,
	char *digipot_label5, char *digipot_curve5, char *digipot_0_position5,
	char *digipot_label6, char *digipot_curve6, char *digipot_0_position6,
	char *digipot_label7, char *digipot_curve7, char *digipot_0_position7,
	char *digipot_group_type, char digipot_group_qty, 
	char *digipot_single_name0, char *digipot_single_name1, char *digipot_single_name2,
	char *digipot_single_name3, char *digipot_single_name4, char *digipot_single_name5,
	char *digipot_single_name6, char *digipot_single_name7,
	int digipot_switch0, int digipot_switch1, int digipot_switch2, int digipot_switch3,
	int digipot_switch4, int digipot_switch5, int digipot_switch6, int digipot_switch7) ; 
