#Chip choice
#MCU		= atmega168
#CHIP	= m168
#### larger chip needed for webserver.PROGMEM
MCU = atmega328p
CHIP   = m328p
PROGRAMMER = avrispmkII
PORT = usb
UPLOAD_RATE = 115200
F_CPU = 8000000
#Project name
NAME    =  test

#Compiler flags
OPT = s
# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)
# Place -I options here
CINCS = -I/usr/local/avr/ -I.
# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes 

CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
CFLAGS = -mmcu=$(MCU) $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) 


#Compiled apps
APPS	=  simple_ajax
#### webserver.PROGMEM needs 19k of RAM for the default setup
#APPS	= webserver.PROGMEM
#APPS	= dhcpc webserver.PROGMEM

#Compiled drivers
DEVS	= enc28j60 #The rest are determined by makefiles for apps

#Compiled libs
LIBS	= uart websrv_helper_functions 1wire#Determined by makefiles for apps or drivers

#Project specific files
$(NAME)_SOURCES = main.c clock-arch.c

#Low fuse
LFUSE	= 0xC2

#High fuse
HFUSE	= 0xD9

#Ext fuses
EFUSES = 0x07

#Include global configurations
-include ../../MakeConf.include

#Include the genetic makefile
-include ../../Makefile.include
