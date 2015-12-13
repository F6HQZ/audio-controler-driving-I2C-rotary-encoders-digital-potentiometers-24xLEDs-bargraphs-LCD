/* extensions-modules.h :
 * 
 * Permits to drive a gang of I2C AD/DA PCF8591 behind a Raspberry Pi.
 * To be used with digital-pot.c digital-pot.h I2C-Display.c rotaryencoder.c 
 * and rotaryencoder.h bargraph.h bargraph.c libraries.
 * 
 * V.1.0.0
 */

/*=======================================================================\
|      - Copyright (c) - 2015 October- F6HQZ - Francois BERGERET -       |
|                                                                        |
| This "extensions-modules.h" and "extensions-modules.c" files can run   | 
| only with the necessary and excellent wiringPi tools suite for         |
| Raspberry Pi from the "Gordons Projects" web sites                     |
| from Gordon Henderson :                                                |
| https://projects.drogon.net/raspberry-pi/wiringpi/                     |
| http://wiringpi.com/                                                   |
|                                                                        |
| My libraries permit an easy use of few rotary encoders with push switch|
| in their axis, I2C LCD displays, I2C LEDs bargraphes and I2C digital   |
| potentiometers, I2C extensions modules for GPIO digital oe analog I/O  |
| and use them as "objects" stored in structures.                        |
|                                                                        |
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

#define MAX_MODULES 8  // or any you want with different I2C adresses
#define MAX_CHANNELS 8 // how many channels in a chip

// PCF8591 is an I2C A/D D/A converter, offering 4 A/D inputs and 1 D/A output.

struct extension_module
{
	char *module_label ;    // name given to this chip, as "CONVERTER#1", as a reminder of "what is this $#% of chip for ?"
	char *module_input_name[MAX_CHANNELS] ;  // name or label as "INPUT", "VOLUME" or "STAGE1" or "OUTPUT", etc... for each chip I/O channel
	char *module_type ;     // type of chip, PCF8591, or any of the described chips at top of this file
	char *module_bus_type ; // 0 for I2C or 1 for SPI
	int module_address ;    // address of the chip on the I2C or SPI bus (check with "gpio i2cdetect" Linux console response)
	int module_pinBase ;    // first pin (base) address, need to be different for each converter and >64
	char module_channels ;  // number of available channels
	int module_setUpIO ;    // used to point to the correct I2C system object by wiringPi
} ;

struct extension_module modules[MAX_MODULES] ;

struct extension_module *setupModule(char *module_label, 
	char *module_input_name_0, char *module_input_name_1, 
	char *module_input_name_2, char *module_input_name_3, 
	char *module_input_name_4, char *module_input_name_5,
	char *module_input_name_6, char *module_input_name_7,
	char *module_type, char *module_bus_type, int module_address, 
	int module_pinBase, char module_channels) ; 
