/**
 * Course: CS3411 R01
 * Author: Chanpech Hoeng
 * Goal of the programs:
 *  -compute a 32-bit checksum of the block
 *  -print this number as a hexadecimal number
 *  -write the block to the output file.
 *  -continue by block computation until end of file input
 */
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
extern int errno;

int isIntger(const char *argv)
{
    printf("Is Integer check %s\n", argv);
    while (*argv)
    {
        if (!isdigit(*argv))
            return -1;
        argv++;
    }
    return 1;
}
int isMultipleOfFour(int blocksize)
{ // Might have to double check
    if (blocksize % 4 != 0)
    {
        int round_up = 4 - (blocksize % 4);
        blocksize = blocksize + round_up;

        char str[] = "Warning: input blocksize is not multiple of 4 thus been rounded up\n";
        write(2, str, sizeof(str));
    }
    return blocksize;
}
void *padToFourByte(int currentByteSize, const char *buff)
{
    int startPadIndex = (currentByteSize % 4);
    int numByteToPadd = 4 - startPadIndex;
    char *padBuff = (char*)(buff + currentByteSize);
    while (startPadIndex < 4)
    {
        *padBuff = '\0';
        padBuff++;
        startPadIndex++;
    }
    return (void *)buff;
}
/**
 * The checksum is a running XOR of the block
 * data where four consecutive
 * bytes are treated as an unsigned integer
 * (i.e. cast the data to an unsigned int)
 * and xor’ed to the sum.
 */
unsigned int checkSum(const char *buff, int byteToRead)
{

    printf("ByteToRead Size: %d \n", byteToRead);
    if( byteToRead % 4 != 0){
        printf("PADDING...\n");
        buff = padToFourByte(byteToRead, buff);
    }
    unsigned int *currentFourB = (unsigned int *)buff;
    // Might be trouble because you're accessing something undefined
    unsigned int *previousFourB = (unsigned int *)(buff - 4);
    unsigned int XOR = *currentFourB ^ *previousFourB;

    int count = 4;
    while (count < byteToRead)
    {
        int checkPad = abs(byteToRead - count);
        const char *tempBuff = (buff + count);

        currentFourB = (unsigned int *)(tempBuff);
        XOR = *currentFourB ^ XOR;
        count += 4;
    }
    
    char str[9];
    sprintf(str, "%08x ", XOR);
    write(1, str, sizeof(str)); 

    printf("\n");
    return XOR;
}

int main(int argc, char **argv, char **env)
{
    // Inital file variables
    int outfile;
    int infile;
    int blocksize;
    char *tempStr;

    // Invalid input checking
    if (argc <= 2)
    {
        char str[] = "copy <infile> <outfile> [blocksize]\n";
        write(2, str, sizeof(str));
        exit(1);
    }
    // If blocksize input is skipt we assume 1024 bytes
    //printf("Argc %d argv %s\n", argc, argv[3]);
    if (argc == 3)
    {
        blocksize = 1024;
    }
    else if (isIntger(argv[3]) == -1)
    {
        char str[] = "Invalid blocksize\n";
        write(2, str, sizeof(str));
        exit(1);
    }
    else
    {
        blocksize = atoi(argv[3]);
    }

    blocksize = isMultipleOfFour(blocksize);
    printf("Blocksize is %d\n", blocksize);

    // Initialize files

    infile = open(argv[1], O_RDONLY, 0600);
    outfile = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0600);
    // Notes O_TRUNC pretty much wipe the file and length is truncated to 0
    // and the mode and owner are unchanged.
    if (infile == -1 || outfile == -1)
    {
        char str[15];
        sprintf(str, "Error Number %d\n", errno);
        write(2, str, sizeof(str));
        char error[] = "Program: No such file or directory\n";
        write(2, error, sizeof(error));
        exit(1);
    }
    int current_byte = 0;
    ssize_t byteToRead;
    ssize_t byteWritten;
    char buff[blocksize];

    byteToRead = read(infile, buff, blocksize);
    while (1)
    {
        unsigned int checksum = checkSum(buff, byteToRead); // Checksum should check one block at a time
        
        char str[9];
        //sprintf(str, "%08x ", checksum);
        //write(1, str, sizeof(str)); // Should be change to the output file
        
        sprintf(str, "%08x ", checksum);
        byteWritten = write(outfile, str, sizeof(str));
        byteToRead = read(infile, buff, blocksize);

        if (!byteToRead)
            break;
        if (byteWritten == -1)
        {
            char tempBuff[30];
            sprintf(tempBuff, "Error Number %d\n", errno);
            write(2, tempBuff, sizeof(tempBuff));
            exit(1);
        }
    }
    close(infile);
    close(outfile);
    return 1;
}