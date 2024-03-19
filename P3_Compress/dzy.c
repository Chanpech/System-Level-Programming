/**
 * Developer: Chanpech Hoeng
 * Last update: Oct. 25 2023
 * 
*/
#include "bitsy.h" 
/*Include any additional headers you require*/
#define END_FILE 256
#define BLOCK_SIZE 1024
/*You may use any global variables/structures that you like*/


static unsigned char readBitBuf = 0x0000;
unsigned short readBufCount = 7;
unsigned short charSets[2][BLOCK_SIZE];
static unsigned increment = 0;

/* main - dzy de-compression implementation
 * This program decompresses a compressed stream directed at its standard input
 * and writes decompressed data to its standard output.
 *
 * To compile dzy: make dzy
 * To execute: ./dzy < somefile.encoded > somefile_decoded.txt
 */

/**
 * Initialized the global charSets[][] array. 
*/
void initializeArray(){
  for(int i = 0; i < 2; i++){
    for(int j = 0; j < 17; j++){
        charSets[i][j] = 0; //Setting every to 0 equivalent to null in char
        //Might be an edge case
    }  
  }  
}

/**
 * Find the char or byte located in the specified position (pst).
 * @return byte that was found.
*/
unsigned char findCharAtPst(unsigned long pst){
	unsigned char byte = charSets[0][pst];
	return byte;
}

/**
 * Insert the byte into the charSets[][] array.
*/
void insertIntoSet(unsigned char byte){
	charSets[0][increment] = byte;
	increment++;
}

/**
 * Will decode and output based on the following format <0> <pst>.
 * Where pst is something like 001. 
 * @return 1 on sucess otherwise return END_FILE.
*/
unsigned short frequentOutput(unsigned long currentPst)
{
	unsigned long pst = 0;
	unsigned short threeBit = 0x0000;

	for(int i = 2; i >= 0; i--){
		unsigned short readBitResult = read_bit();
		if(readBitResult == END_FILE){
			return END_FILE;
		}
		threeBit |= readBitResult << i;
	}

	pst = threeBit;
	unsigned char symbol = findCharAtPst(currentPst - pst -1);
	write_byte(symbol);
	insertIntoSet(symbol);
	return 1;
}

/**
 * Will decode and output based on the following format <0> <symbol>.
 * Symbol is made up of 8 bit = 1 byte.
 * @return 1 on sucess otherwise return END_FILE.
*/
unsigned short infrequentOutput()
{
	//output 8 bits from the input
	//1 01100001
	unsigned short byte = read_byte();

	if(byte == END_FILE)
		return END_FILE;

	write_byte(byte); 
	insertIntoSet(byte);
	return 1;
}
/**
 * Decode based on the initial readBit. 
 * If bit is 1 call infrequentOutput().
 * Otherwise call frequentOutput().
 * @return the number of bit read from stdin on success otherwise it will return END_FILE.
*/
unsigned short decode(unsigned short bit, unsigned long currentPst)
{
	unsigned short bitRead = 0;
	readBitBuf |= (bit << 7);

	if(bit == 1)
	{
		unsigned short result = infrequentOutput();
		if(result == END_FILE)
			return END_FILE;
		bitRead = 8;
	}
	else
	{
		unsigned short result = frequentOutput(currentPst);
		if(result == END_FILE)
			return END_FILE;
		bitRead = 3;
	}

	readBitBuf = 0x0000;
	return bitRead;
}

int main(int argc, char *argv[])
{
	unsigned long stdinPos = 0;
	unsigned short bitRead = 0;
	initializeArray();

	while (1)
	{
		unsigned short bit = read_bit();

		if (bit == END_FILE)
			break;
	
		bitRead++;
		unsigned short decodeResult = decode(bit, stdinPos);

		if (decodeResult == END_FILE)
			break;

		bitRead += decodeResult;
		stdinPos++;
	}
	return 0; // exit status. success=0, error=-1
}
