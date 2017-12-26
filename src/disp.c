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

// This controlls the OLED display
// It has a resolution of 128 by 32 pixels
// It uses the SSD1306

#include <stdint.h>
#include "i2c.h"
#include "disp.h"

#define DISP_I2C_ADDR 0x78



static void sendCommand( uint8_t byteCount, uint8_t *bytes );
static void sendData(    uint8_t byteCount, uint8_t *bytes );



void disp_setScrolling( uint8_t on ) {
	if( on ) {
		sendCommand( 7, (uint8_t[]){ // setup H-Scrolling
			0x26, 0x00,
			0x04, // start page
			0x07, // scroll speed
			0x07, // end page
			0x00, // dummy
			0xFF  // dummy
		} );

		sendCommand( 1, (uint8_t[]){ 0x2F } ); // start scrolling
	}
	else {
		sendCommand( 1, (uint8_t[]){ 0x2E } ); // stop scrolling
	}
}



void disp_clear( void ) {
	sendCommand( 1, (uint8_t[]){ 0x2E } ); // stop scrolling
	sendCommand( 1, (uint8_t[]){ 0xE3 } ); // NOP
	sendCommand( 3, (uint8_t[]){ 0x22, 3, 7 } );   // set x range
	sendCommand( 3, (uint8_t[]){ 0x21, 0, 127 } ); // set y range
	for( uint8_t i=0; i<128; ++i ) {
		sendData( 4, (uint8_t[]){ 0x00, 0x00, 0x00, 0x00 } );
	}
	for( uint8_t i=0; i<128; ++i ) {
		sendData( 4, (uint8_t[]){ 0x00, 0x00, 0x00, 0x00 } );
	}
}



typedef struct {
	uint8_t w;
	uint8_t h;
	uint8_t *dat;
} sprite_t;



// TETRIS BRICKS
//   76543210   76543210   76543210   76543210   76543210
// 0 xxxx----   xxxx----   xxxx----   xxxx----   xxxxxxx-
// 1 x--x----   x--x----   x--x----   x--x----   x--x--x-
// 2 x--x----   x--x----   x--x----   x--x----   x--x--x-
// 3 xxxx----   xxxxxxx-   xxxxxxx-   xxxx----   xxxxxxx-
// 4 x--x----   x--x--x-   x--x--x-   x--x----   x--x--x-
// 5 x--x----   x--x--x-   x--x--x-   x--x----   x--x--x-
// 6 xxxxxxx-   xxxxxxx-   xxxxxxx-   xxxx----   xxxxxxx-
// 7 x--x--x-   x--x----   ---x--x-   x--x----
// 8 x--x--x-   x--x----   ---x--x-   x--x----
// 9 xxxxxxx-   xxxx----   ---xxxx-   xxxx----
// A                                  x--x----
// B                                  x--x----
// C                                  xxxx----
//
//   mirror     -          mirror     -          -
//   rotate     rotate     rotate     rotate     -


static uint8_t brick_L[] = { 0xF0, 0x90, 0x90, 0xF0, 0x90, 0x90, 0xFE, 0x92, 0x92, 0xFE }; // new L
static uint8_t brick_T[] = { 0xF0, 0x90, 0x90, 0xFE, 0x92, 0x92, 0xFE, 0x90, 0x90, 0xF0 }; // T
static uint8_t brick_S[] = { 0xF0, 0x90, 0x90, 0xFE, 0x92, 0x92, 0xFE, 0x12, 0x12, 0x1E }; // S
static uint8_t brick_I[] = { 0xF0, 0x90, 0x90, 0xF0, 0x90, 0x90, 0xF0, 0x90, 0x90, 0xF0, 0x90, 0x90, 0xF0 }; // I
static uint8_t brick_O[] = { 0xFE, 0x92, 0x92, 0xFE, 0x92, 0x92, 0xFE}; // O



static sprite_t blocks[] = {
	{ .w = 1, .h = 10, .dat = brick_L },
	{ .w = 1, .h = 10, .dat = brick_T },
	{ .w = 1, .h = 10, .dat = brick_S },
	{ .w = 1, .h = 13, .dat = brick_I },
	{ .w = 1, .h =  7, .dat = brick_O }
};



void writeSprite( sprite_t *sprite, uint8_t page, uint8_t y ) {
	uint8_t p = (page > 3)? 3: page; // offset & clamp
	p += 4;
	uint8_t w = (sprite->w);
	uint8_t h = sprite->h;
	uint8_t len = w * h;
	y = (y>127)?  127: y; // clamp

	sendCommand( 3, (uint8_t[]){ 0x22, p, (p+w-1) } ); // set x
	sendCommand( 3, (uint8_t[]){ 0x21, y, (y+h) } );  // set y
	sendData( len, sprite->dat );
}


void disp_startTetris( void ) {
	writeSprite( &blocks[0], 0, 30 );
	writeSprite( &blocks[1], 1, 100 );
	writeSprite( &blocks[2], 2, 10 );
	writeSprite( &blocks[3], 3, 50 );
	writeSprite( &blocks[4], 0, 80 );
}



void disp_setBrightness( uint8_t brightness ) {
	sendCommand( 2, (uint8_t[]){ 0x81, brightness } ); // contrast
}



void disp_init( void ) {
	// all off
	sendCommand( 1, (uint8_t[]){ 0x2E } ); // stop scrolling
	sendCommand( 1, (uint8_t[]){ 0xAE } ); // set disp off
	sendCommand( 1, (uint8_t[]){ 0xE3 } ); // NOP
	sendCommand( 2, (uint8_t[]){ 0x8D, 0x14 } ); // disable charge pump

	// back on again
	sendCommand( 2, (uint8_t[]){ 0xA8, 0x3F } ); // mux ratio
	sendCommand( 2, (uint8_t[]){ 0xD3, 0x00 } ); // disp offset
	sendCommand( 1, (uint8_t[]){ 0x40 } ); // start line
	sendCommand( 1, (uint8_t[]){ 0xA1 } ); // seg remap
	sendCommand( 1, (uint8_t[]){ 0xC8 } ); // com output scan dir
	sendCommand( 2, (uint8_t[]){ 0xDA, 0x02 } ); // com pin config
//	sendCommand( 2, (uint8_t[]){ 0x81, 0x7F } ); // max contrast (brightness)
	sendCommand( 2, (uint8_t[]){ 0x81, 0x01 } ); // low contrast for battery saving
	sendCommand( 1, (uint8_t[]){ 0xA4 } ); // entire disp on
	sendCommand( 1, (uint8_t[]){ 0xA6 } ); // non-inverse
	sendCommand( 2, (uint8_t[]){ 0xD5, 0x80 } ); // set disp clk div
	sendCommand( 2, (uint8_t[]){ 0x8D, 0x14 } ); // enable charge pump
	sendCommand( 1, (uint8_t[]){ 0xAF } ); // set disp on
	sendCommand( 2, (uint8_t[]){ 0x20, 0x01 } ); // page addr mode

	sendCommand( 3, (uint8_t[]){ 0x21, 0x00, 0x7F } ); // activate all cols
	sendCommand( 3, (uint8_t[]){ 0x22, 0x04, 0x07 } ); // use page 4-7
	sendCommand( 1, (uint8_t[]){ 0xB4 } ); // start page 0
}



static void sendCommand( uint8_t byteCount, uint8_t *bytes ) {
	i2c_send_start( DISP_I2C_ADDR );
	i2c_send_byte( 0x00 );
	for( uint8_t i=0; i < byteCount; ++i ) {
		i2c_send_byte( bytes[i] );
	}

	i2c_send_stop();
}



static void sendData( uint8_t byteCount, uint8_t *bytes ) {
	i2c_send_start( DISP_I2C_ADDR );
	i2c_send_byte( 0x40 );
	for( uint8_t i=0; i < byteCount; ++i ) {
		i2c_send_byte( bytes[i] );
	}

	i2c_send_stop();
}
