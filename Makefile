# oled earrings
#
# Copyright (c) 2017 <Daniel Steuer>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

BINARY = oled-earrings

BROOT = build

SRC_FILES = $(shell ls src/*.c)
HDR_FILES = $(shell ls src/*.h)

DATE=$(shell date +%F)
HOUR=$(shell date +%H)
MIN=$(shell date +%M)
COMPILE_DATE="$(DATE) $(HOUR):$(MIN) CET"
CC=avr-gcc
OBJCPY=avr-objcopy
AVRDUDE=avrdude
AVRDUDEARCH=t85
AVRDUDEPROG=usbasp

BR=$(BROOT)/bin



OBJTMP=$(shell ls src/*.c | sed s/src.//)
OBJR=$(OBJTMP:%.c=$(BR)/obj/%.o)

RELEASE_DEFINES =  -DPROGRAMNAME=\"$(BINARY)\"
RELEASE_DEFINES += -DCOMPILE_DATE=\"$(COMPILE_DATE)\"
RELEASE_DEFINES += -DF_CPU=1000000UL
BASE_CFLAGS= -mmcu=attiny85 -Os -pedantic -std=c99
CFLAGS=$(BASE_CFLAGS)
LDFLAGS=



.PHONY: all objcopy clean
release: $(BR)/$(BINARY).hex $(BR)/$(BINARY).bin
all: release



#####
# Link binary
$(BR)/$(BINARY).elf: $(BR) $(OBJR)
	@printf "LINKING \033\13301;33m-\033\13301;37m %-20s\n\033\13300;39m" $@
	@$(CC) $(CFLAGS) $(OBJR) $(LDFLAGS) -o $(BR)/$(BINARY).elf



#####
# Compile actual files
$(BR)/obj/%.o: src/%.c $(HDR_FILES)
	@printf "BUILD   \033\13301;33m-\033\13301;37m %-20s %s\033\13300;39m\n" $< $@
	@$(CC) -c $(CFLAGS) $(RELEASE_DEFINES) -o $@  $<



#####
# ObjCopy
$(BR)/$(BINARY).hex: $(BR)/$(BINARY).elf
	@printf "OBJCPY  \033\13301;33m-\033\13301;37m %-20s %s\033\13300;39m\n" $@
	@$(OBJCPY) -O ihex $(BR)/$(BINARY).elf $(BR)/$(BINARY).hex



#####
# ObjCopy
$(BR)/$(BINARY).bin: $(BR)/$(BINARY).elf
	@printf "OBJCPY  \033\13301;33m-\033\13301;37m %-20s %s\033\13300;39m\n" $@
	@$(OBJCPY) -O binary $(BR)/$(BINARY).elf $(BR)/$(BINARY).bin



#####
# Cleanup
clean:
	@rm -rf $(BROOT)



#####
# Flash
flash: $(BR)/$(BINARY).hex
	@$(AVRDUDE) -p $(AVRDUDEARCH) -c $(AVRDUDEPROG)   -P usb -U flash:w:"$(BR)/$(BINARY).hex":i



#####
# Setup directories
$(BR):
	@if [ ! -d $(BROOT) ];then mkdir -p $(BROOT);fi
	@if [ ! -d $(BROOT)/gcc_release ];then mkdir -p $(BR); mkdir -p $(BR)/obj;fi
	@if [ ! -d $(@) ];then mkdir $@;fi
