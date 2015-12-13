/* bargraph.h :
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
| in their axis, I2C LCD displays, I2C LEDs bargraphes and I2C digital   |
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

#include "binary-values.h"

#define MAX_BARGRAPHS 2 // or any you want with different I2C adresses

// HT16K33 is a 16 x 8 LEDs matrix I2C controler 
// Following are its registers and variables

# define HT16K33_I2C_ADDRESS            0b01110000 // 0x70 to 0x77 depending 3 bits to solder on PCB

# define HT16K33_DATA_ADDRESS_POINTER   0b00000000 // 0x00 - LSB 0000 to 1111 define one of the 16 RAM addresses
# define HT16K33_SYS_SETUP_REGISTER_OFF 0b00100000 // 0x20 - stop clock
# define HT16K33_SYS_SETUP_REGISTER_ON  0b00100001 // 0x21 - start clock
# define HT16K33_REGISTER_DISPLAY_OFF   0b10000000 // 0x80
# define HT16K33_REGISTER_DISPLAY_ON    0b10000001 // 0x81
# define HT16K33_BRIGHTNESS             0b11101111 // 0xef _ LSB 0000 to 1111 (E0 to EF) for min to max brightness (1/16 steps)

# define HT16K33_ROW_INT_SET_REGISTER   0b10100000 // 0xa0 - all rows ON (as outputs)

# define LED_OFF    0
# define LED_RED    1
# define LED_AMBER  2
# define LED_GREEN  3

# define I2C_DELAY 0 // millisecondes

struct bargraph
{
	char *bargraph_label ;  // name or label as "Volume" or "Balance" or "Treble", etc...
	int bargraph_address ;  // address of the chip on the I2C or SPI bus (check with "gpio i2cdetect" Linux console instruction)
	char *bargraph_ref ;    // model, provider/manufacturer reference
	int bargraph_steps ;    // number of LEDs steps without counting colors, only what is visible when lights turned OFF
	unsigned char bargraph_bicolor ;  // 1 or 2. Number of different colors/LEDs (ex: GREEN and RED LEDs = 2, ORANGE is ONLY this 2 colors ON simultaneously.)
	unsigned char bargraph_reversed ; // 0: no - 1: yes - left to right or right to left, depending hardware integration in a front pannel
	int bargraph_value ; // memory, for balistic use (decay tempo, etc...) 
	int bargraph_setUpIO ;  // used to point to the correct I2C system object by wiringPi
};

struct bargraph bargraphs[MAX_BARGRAPHS] ;

struct bargraph *setupbargraph(char *bargraph_label, int bargraph_address, 
	char *bargraph_ref, int bargraph_steps, unsigned char bargraph_bicolor,
	unsigned char bargraph_reversed) ; 
