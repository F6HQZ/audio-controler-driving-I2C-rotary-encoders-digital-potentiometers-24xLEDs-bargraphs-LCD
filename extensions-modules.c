/* extensions-modules.c :
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
| in their axe, I2C LCD displays, I2C LEDs bargraphes and I2C digital    |
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
 
#include <stdio.h>

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8591.h>

#include "extensions-modules.h"

struct extension_module *setupModule(char *module_label, 
	char *module_input_name_0, char *module_input_name_1, 
	char *module_input_name_2, char *module_input_name_3, 
	char *module_type, char *module_bus_type, int module_address, 
	int module_pinBase) ; 
	
int numberofmodules = 0 ; // as writed, number of bargraphs, will be modified by the code later

//======================================================================

struct extension_module *setupModule(char *module_label, 
	char *module_input_name_0, char *module_input_name_1, 
	char *module_input_name_2, char *module_input_name_3, 
	char *module_type, char *module_bus_type, int module_address, 
	int module_pinBase)
{
	if (numberofmodules > MAX_MODULES)
	{
		printf("Max number of modules like pcf8591's exceded: %i\n", MAX_MODULES) ;
		return NULL ;
	}
	
	struct extension_module *newmodule = modules + numberofmodules++ ;
	newmodule->module_label = module_label ;         // name given to this chip, as "CONVERTER#1", as a reminder of "what is this $#% of chip for ?"
	newmodule->module_type = module_type ;           // type of chip, PCF8591, or of the described chips at top of the "extensions-modules.h" file
	newmodule->module_bus_type = module_bus_type ;   // 0 for I2C or 1 for SPI
	newmodule->module_address = module_address ;     // address of the chip on the I2C or SPI bus (check with "gpio i2cdetect" Linux console instruction)
	newmodule->module_pinBase = module_pinBase ;     // first pin (base) address, need to be different for each converter and >64
	
	
	newmodule->module_input_name[0] = module_input_name_0 ; // name or label as "INPUT", "VOLUME" or "STAGE1" or "OUTPUT", etc... for each chip I/O channel
	newmodule->module_input_name[1] = module_input_name_1 ; 
	newmodule->module_input_name[2] = module_input_name_2 ; 
	newmodule->module_input_name[3] = module_input_name_3 ; 
	
	if (module_type == "PCF8591")
	{
		pcf8591Setup(module_pinBase, module_address) ;
		printf("pcf8591#:0x%x I2C:0x%x Pinbase:%d - init done ! \n", modules, module_address, module_pinBase) ;
	}
	
	return newmodule ;
}

