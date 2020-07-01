#include <stdio.h>
#include <unistd.h>
//#include <armbianio.h>
#include <errno.h>
#include "tm1637.h"
//
// Larry's TM1637 library
// Copyright (c) 2018 BitBank Software, Inc.
// Project started 3/17/2018
// written by Larry Bank (bitbank@pobox.com)
//
// The TM1637 from Titan Micro Electronics is a 7-segment LED controller
// It features a 2-wire interface (Clock + Data), but doesn't conform to I2C nor SPI standards
// It can control up to 6 7-segment digits along with decimal points or colons as well as has
// 16 button/keyboard inputs
// The datasheet says that it runs on 5V with 5V logic, but I've run it on 3.3V (both Vcc and signals)
// and it seems to behave correctly.
// Since the communication protocol is custom, it requires "bit-banging" of 2 IO pins to control it.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//
// table which translates a digit into the segments
//
const unsigned char cDigit2Seg[] = {0x3f, 0x6, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
#define CLOCK_DELAY 5
#define LOW "0\n"
#define HIGH "1\n"
#define CLK_GPIO_PATH "/sys/class/gpio/gpio44/value"
#define DATA_GPIO_PATH "/sys/class/gpio/gpio45/value"

#define bClockPin g_myio.clk
#define bDataPin g_myio.data

struct myio {
	FILE *clk;
	FILE *data;
} g_myio;

static int AIOInit()
{
	/* Expect GPIOs to be exported */
	g_myio.clk = fopen(CLK_GPIO_PATH, "w");
	g_myio.data = fopen(DATA_GPIO_PATH, "w");
	if (!g_myio.clk || !g_myio.data) {
		int err = errno;
		printf("failed to open GPIOs %d\n", err);
		return -err;
	}

	return 0;
}

void AIOWriteGPIO(FILE *pin, const char *val)
{
	if (2 != fprintf(pin, val)) {
		perror("write");
	}
	fflush(pin);
}

int tm1637Init(/*unsigned char bClock, unsigned char bData*/)
{
	int rc;

	rc = AIOInit();
	if (rc)
	{
		printf("Init failed\n");
		return -1;
	}
	AIOWriteGPIO(bClockPin, LOW);
	AIOWriteGPIO(bDataPin, LOW);
	return 0;
}

//
// Start wire transaction
//
static void tm1637Start(void)
{
	AIOWriteGPIO(bDataPin, HIGH);
	AIOWriteGPIO(bClockPin, HIGH);
	usleep(CLOCK_DELAY);
	AIOWriteGPIO(bDataPin, LOW);
}

//
// Stop wire transaction
//
static void tm1637Stop(void)
{
  AIOWriteGPIO(bClockPin, LOW); // clock low
  usleep(CLOCK_DELAY);
  AIOWriteGPIO(bDataPin, LOW); // data low
  usleep(CLOCK_DELAY);
  AIOWriteGPIO(bClockPin, HIGH); // clock high
  usleep(CLOCK_DELAY);
  AIOWriteGPIO(bDataPin, HIGH); // data high
}

//
// clock the data ack even we do not read it
//
static void tm1637GetAck(void)
{
	AIOWriteGPIO(bClockPin, LOW); // clock to low
	usleep(CLOCK_DELAY);
	AIOWriteGPIO(bClockPin, HIGH); // clock high
 	usleep(CLOCK_DELAY);
	AIOWriteGPIO(bClockPin, LOW); // clock to low
}

//
// Write a unsigned char to the controller
//
static void tm1637WriteByte(unsigned char b)
{
	unsigned char i;

	for (i=0; i<8; i++)
	{
		AIOWriteGPIO(bClockPin, LOW); // clock low
		if ((b >> i) & 1) // LSB to MSB
			AIOWriteGPIO(bDataPin, HIGH);
		else
			AIOWriteGPIO(bDataPin, LOW);
		usleep(CLOCK_DELAY);
		AIOWriteGPIO(bClockPin, HIGH); // clock high
		usleep(CLOCK_DELAY);
	}
}

//
// Write a sequence of unsigned chars to the controller
//
static void tm1637Write(unsigned char *data, unsigned char len)
{
	unsigned char b;

	tm1637Start();
	for (b = 0; b < len; b++)
	{
		tm1637WriteByte(data[b]);
		tm1637GetAck();
	}
	tm1637Stop();
}

//
// Set brightness (0-8)
//
void tm1637SetBrightness(unsigned char b)
{
	unsigned char val;

	if (b == 0) // display off
		val = 0x80; // display off
	else
	{
		if (b > 8)
			 b = 8;
		val = 0x88 | (b - 1);
	}
	tm1637Write(&val, 1);
}

void tm1637ShowDigits_nocolon(char *str)
{
	unsigned char buf[16]; // commands and data to transmit
	unsigned char i, j;
	
	j = 0;
	buf[j++] = 0x40; // memory write command (auto increment mode)
//	tm1637Write(buf, 1);
	buf[j++] = 0xc0; // set display address to first digit command

	for (i=0; i<4; i++)
		if (str[i] >= '0' && str[i] <= '9')
			buf[j++] = cDigit2Seg[str[i] & 0xf]; // segment data

	tm1637Write(buf, j+1); // send to the display
}

//
// Display a string of 4 digits and optional colon
// by passing a string such as "12:34" or "45 67"
//
void tm1637ShowDigits(char *str)
{
	unsigned char buf[16]; // commands and data to transmit
	unsigned char i, j;
	
	j = 0;
	buf[j++] = 0x40; // memory write command (auto increment mode)
//	tm1637Write(buf, 1);

	buf[j++] = 0xc0; // set display address to first digit command
	for (i=0; i<5; i++)
	{
		if (i == 2 && str[i] == ':') {
			buf[3] |= 0x80;
			continue;
		}
		if (str[i] >= '0' && str[i] <= '9')
			buf[j++] = cDigit2Seg[str[i] & 0xf]; // segment data
	}
	tm1637Write(buf, j + 1); // send to the display
}
