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
#include "i2c.h"



/*
 * In I2C the lines are never driven HIGH. Instead the uC floats the pin and a resistor pulls it up.
 * That's why we prevent the line from being driven high
*/
static void float_sda( void ) {
	PORTB &=  ~(1<<SDA_PIN);
	DDRB  &= ~(1<<SDA_PIN);
}

static void pullup_sda( void ) {
	DDRB  &= ~(1<<SDA_PIN); // don't drive output
	PORTB |=  (1<<SDA_PIN); // activate pullup
}

static void pullup_scl( void ) {
	DDRB  &= ~(1<<SCL_PIN); // don't drive output
	PORTB |=  (1<<SCL_PIN); // activate pullup
}


static void drive_sda ( void ) {
	PORTB &=  ~(1<<SDA_PIN); // prevent line from beeing driven high
	DDRB  |=   (1<<SDA_PIN);
}

static void drive_scl ( void ) {
	PORTB &=  ~(1<<SCL_PIN); // prevent line from beeing driven high
	DDRB  |=   (1<<SCL_PIN);
}

static void manual_clk_pulse( void ) {
	pullup_scl();
	_delay_us( BIT_DELAY );
	drive_scl();
}



void i2c_init( void ) {
	pullup_scl();
	pullup_sda();
}



void i2c_send_start( uint8_t addr ) {
	// generate start condition manually

	pullup_scl();
	pullup_sda();
	while( !(PINB & (1<<SCL_PIN)) );
	_delay_us( BIT_DELAY );

	drive_sda();
	_delay_us( BIT_DELAY );
	drive_scl();
	_delay_us( BIT_DELAY );

	i2c_send_byte( addr );
}



void i2c_send_stop( void ) {
	drive_sda();
	_delay_us( BIT_DELAY );
	pullup_scl();
	while( !(PINB & (1<<SCL_PIN)) );
	_delay_us( BIT_DELAY );
	pullup_sda();
	while( !(PINB & (1<<SDA_PIN)) );
}



void i2c_send_byte( uint8_t byte ) {
	uint8_t ack = 0;
	uint8_t dat = byte;

	drive_scl();
	drive_sda();

	// send actual data
	for( uint8_t i=0; i<8; i++ ) {
		_delay_us( BIT_DELAY );
		if( dat & 0x80 ) {
			pullup_sda();
		}
		else {
			drive_sda();
		}
		_delay_us( BIT_DELAY );
		manual_clk_pulse();
		dat <<= 1;
	}
	_delay_us( BIT_DELAY );

	// get ACK or NACK
	float_sda();
	manual_clk_pulse();

	_delay_us( BIT_DELAY );
}
