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

#define BLOCK_SIZE 1024
#define END_FILE 256
#define UNIQUE 257

//This header includes prototypes for the proposed bit abstractions
/*Include any additional headers you require*/

/* main - czy compression implementation
 * Develop a program called czy which compresses the data stream directed 
 * at its standard input and writes the compressed stream to its standard 
 * output.
 *
 * The compression algorithm reads the input one symbol (i.e., byte) at a 
 * time and compares it with each of the 8 bytes previously seen.  It also 
 * checks to see if the following n characters are the same as the current 
 * symbol.  If the byte has been previously seen, it outputs the position of 
 * the previous byte relative to the current position, Otherwise, the symbol 
 * is output as is by prefixing it with a binary one.
 *
 * To compile czy: make czy
 * To execute: ./czy < somefile.txt > somefile.encoded
 */

unsigned short charSets[2][BLOCK_SIZE]; // Short is convienent for the 9 bits case. 
//Need to handle the extension of the blocksize in case it is full. 


/**
 * This is a replacement of flush_write_buffer() from bitsy.h
 * Modulo check and pad by just writing necessary amount '1' bit.
*/
void padIfNecessary(unsigned short bitWritten){ 
  unsigned short bitToPadd = 8 - (bitWritten % 8);
  
  while(bitToPadd > 0){
    write_bit(1);
    bitToPadd--;
  }
}

/**
 * intialized the charSets[][] array to 0
*/
void initializeArray(){
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < BLOCK_SIZE; j++){
        charSets[i][j] = 0; //Setting every to 0 equivalent to null in char
    }  
  }  
}

/**
 * search previous 8 bytes within out charSet[] globals
 * @return UNIQUE if it couldn't find any element with the same value
 * Otherwise
 * @return currentPos of searched byte - position of matched element -1
*/
unsigned long searchPrevious8( int currentPos, unsigned short value ){
  int initialPos = currentPos;

  if(currentPos <= 8)
  {
    while(currentPos >= 0){ 
      --currentPos;
      if(charSets[0][currentPos] == value){
        return initialPos - currentPos -1;
      }
    }
  }
  else
  {
    int count = 8;
    while(1){
      if(count == 0)
        break;
      --currentPos;
      if(charSets[0][currentPos] == value){
        return initialPos - currentPos -1;
      }  
      count--;
    }
  }
  return UNIQUE;
}

/**
 * Write the <pst> to bit buffer
*/
void write3Bit(unsigned long pst){
  write_bit((pst >> 2) & 1);  //write the 2nd bit
  write_bit((pst >> 1) & 1);  //write the 1st bit
  write_bit((pst >> 0) & 1);  //write the 0th bit
}

/**
 * Write 9 bit to the byte buffer
*/
void setInfrequent( unsigned short byte ){
  //<1> <byte>
  write_bit(1);
  write_byte(byte);
}

/**
 * Write 4 bit to the byte buffer
*/
void setFrequent( unsigned long pst ) {
  // <0> <pst>
  write_bit(0);
  write3Bit(pst);
}
/**
 * Based on the initial read bit. Encode will search previous the 8 byte from the charSet[][] array.
 * If position return a unique (meaning it doesn't exist) then it will call setInFrequent().
 * Otherwise call setFrequent().
 * @return Number of bitWritten to the byteBuffer from bitsy
*/
unsigned short encode(unsigned short value, unsigned long currentPos){
  unsigned long position = searchPrevious8(currentPos, value);
  unsigned short bitWritten;

  if(position == UNIQUE){
    setInfrequent(value);
    bitWritten = 9;
  }
  else
  {
    setFrequent(position);
    bitWritten = 4;
  }
  return bitWritten;
} 


int main(int argc, char *argv[]){
  unsigned long stdinPos = 0;
	int index = 0;
  unsigned short bitWritten = 0;

  initializeArray();

	//Loop through entered standard inputs
	while(1){
    unsigned short value = read_byte();
    if(value == 0){
      break;
    }else if(value == 10){ 
      padIfNecessary(bitWritten);
      break;
    }else if(value == END_FILE){
      padIfNecessary(bitWritten);
      break;
    }
    charSets[0][index]= value;
    bitWritten += encode(value, stdinPos);
    stdinPos++;
		index++;
	}

	return 0; //exit status. success=0, error=-1
}
