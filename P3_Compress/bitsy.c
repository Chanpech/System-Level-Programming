/**
 * Developer: Chanpech Hoeng
 * Last update: Oct. 25 2023
 * 
*/
#include "bitsy.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


/* Add any global variables or structures you need.*/
#define END_FILE 256
/* read_byte
 * Abstraction to read a byte.
 * Relys on readBit.
 */

// Write globals
static unsigned char writeBitBuf = 0x0000;
static int wCurrentBit = 7;


/*
  zeroOutBuf()
  -Zero out puffer with '0' byte
 */
void zeroOutBuf(unsigned char buf[], int size)
{
	for (int i = 0; i < size; i++)
	{
		buf[i] = '0';
	}
}

unsigned short read_byte()
{
	/* This function should not call read() directly.
	 * If we are buffering data in read_bit, we dont want to skip over
	 * that data by calling read again. Instead, call read_bit and
	 * construct a byte one bit at a type. This also allows us to read
	 * 8 bits that are not necessarily byte alligned. */

	unsigned char byteChar = 0x0000;
	unsigned short bitRead;

	for(int i = 7; i >= 0; i--){
		bitRead =read_bit();
		if(bitRead == END_FILE){
			return END_FILE; //eof
		}
		byteChar = byteChar | (bitRead << i);
	}
	return byteChar;
}

/* read_bit
 * Abstraction to read a bit.
 * */
unsigned short read_bit()
{
/* This function is responsible for reading the next bit on the
* input stream from stdin (fd = 0). To accomplish this, keep a
* byte sized buffer. Each time read bit is called, use bitwise
* operations to extract the next bit and return it to the caller.
* Once the entire buffered byte has been read the next byte from
* the file. Once eof is reached, return a unique value > 255
*
*/
	static unsigned char buffer = 0x0000, byte = 8; //Byte sized buffer


	//bytes_read = read(0, read_buf, 1)* 8	
	if(byte % 8 == 0)
	{
		byte = 0;	
	//Returning something that is not mapped already.
	//So something > 255
		if (!read(0, &buffer, 1))
			return END_FILE;
	}
	
	unsigned char value = (buffer & 0x80U) >> 7; //higher bit
	buffer = buffer << 1;
	byte++;

	return value; // placeholder
}

/* write_byte
 * Abstraction to write a byte.
 */
void write_byte(unsigned char byt)
{
	/* Use writeBit to write each bit of byt one at a time. Using writeBit
	 * abstracts away byte boundaries in the output.*/
	for (int i = 7; i >= 0; i--)
	{
		write_bit((byt & (1U << i)) >> i);
	}
}

/* write_bit
 * Abstraction to write a single bit.
 */
void write_bit(unsigned char bit)
{
	/* Keep a byte sized buffer. Each time this function is called, insert the
	 * new bit into the buffer. Once 8 bits are buffered, write the full byte
	 * to stdout (fd=1).
	 */
	writeBitBuf |= (bit << wCurrentBit--);
	if (wCurrentBit == -1)
	{
		//Added condition statement
		if(write(1, &writeBitBuf, 1) == -1){
			perror("WRITE");
			exit(0);
		}
		wCurrentBit = 7;
		writeBitBuf = 0x0000;
	}
}

/* flush_write_buffer
 * Helper to write out remaining contents of a buffer after padding empty bits
 * with 1s.
 */
void flush_write_buffer()
{
	/* This will be utilized when finishing your encoding. It may be that some bits
	 * are still buffered and have not been written to stdout. Call this function
	 * which should do the following: Determine if any buffered bits have yet to be
	 * written. Pad remaining bits in the byte with 1s. Write byte to stdout
	 */

	//Was told it is okay to implement this in czy
}
