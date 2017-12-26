/*
 * oled earrings
 *
 * Copyright (c) 2017 <Daniel Steuer>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/wdt.h>

#include "i2c.h"
#include "disp.h"



#define RESET_PIN PB5



void powersave( void ) {
	uint8_t mcucr = 0;

	GIMSK = 0; // no interrupts

	ACSR = (1<<ACD); // disable ADC

	DIDR0  = (1<<AIN1D) | (1<<AIN0D); // disable analoge Input pins
	DIDR0 |= (1<<ADC0D) | (1<<ADC2D) | (1<<ADC3D) | (1<<ADC1D); // disable ADC pins

	USICR = 0; // disable USI

	TIMSK = 0;  // disable timer interuppts
	TCCR0B = 0; // disable timer 0
	TCCR1 = 0;  // disable timer 1


	PRR  = (1<<PRTIM1) | (1<<PRTIM0); // deactivate timer0/1
	PRR |= (1<<PRUSI); // deactivate USI
	PRR |= (1<<PRADC); // deactivate ADC
	mcucr  = (1<<ISC01) | (1<<ISC00); // wakeup on rising edge on INT0
	mcucr |= (1<<SM1); // select powerdown for the next sleep mode
	mcucr |= (1<<PUD); // disable all pullups

	// bround out detection deactivation sequence
	MCUCR  = mcucr | (1<<BODSE) | (1<<BODS);
	MCUCR  = mcucr | (1<<BODS);

	MCUCR |= (1<<SE); // enter sleep mode
}



int main( void ) {
	uint8_t ret;
	char    *cmd;

	DDRB  = 0;
	PORTB = (1<<RESET_PIN);

	i2c_init( );
	disp_init( );

	disp_setScrolling( 0 );
	disp_clear();
	disp_startTetris();
	disp_setScrolling( 1 );

	// at this point we don't need the uC anymore
	// power down for maximum power saving
	powersave();

	// should never be reached
	while( 1 );
}
