/**
 * Author: Chanpech Hoeng
 * Dates: 11/6/2023
 * Description: Client user 1 simulates the operations of syscall on the server.
 * This user1 will open local and remote files. Then copy files from remote to local.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <sys/types.h>

#include <errno.h>
#include <error.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "rp_calls.h"


static int sock_fd;

/*************************************************************************/
//                                                                       //
//     read_from_server(): reads next packet of data                     //
//                         and returns pointer to the                    //
//                         data read                                     //
//                                                                       //
//     Calling Convention:                                               //
//                                                                       //
//              1. Reading an integer:                                   //
//                                                                       //
//                      int var = *(int*) read_from_server();            //
//                                                                       //
//              2. Reading a string:                                     //
//                                                                       //
//                      char *string = read_from_server();               //
//                                                                       //
//     How it works:                                                     //
//                                                                       //
//              The communication for this assignment is                 //
//              abstracted to the following:                             //
//                                                                       //
//                      Any data sent across the network                 //
//                      is preceeded by a fixed size packet              //
//                      indicating the size of the data                  //
//                      contained in the subsequent packet               //
//                                                                       //
//              Ex 1:                                                    //
//                      Sending an integer is done like so:              //
//                                                                       //
//                      client --> [size of int] --> server              //
//                      client --> [integer value] --> server            //
//                                                                       //
//              Ex 2:                                                    //
//                                                                       //
//                      Sending a string is done like so:                //
//                                                                       //
//                      client --> [length of string] --> server         //
//                      client --> [string data] --> server              //
//                                                                       //
/*************************************************************************/
//Will block if it don't receives anything
void* read_from_server(){
    size_t size;
    char *buf;
    int rtrn; 

    if((rtrn = read(sock_fd, &size, sizeof(size))) < 1) {
        if(rtrn == -1) {
            perror("read_from_server read 1");
            exit(-1);
        }
        return NULL;
    }

    size = ntohl(size);
    
    buf = malloc(size);
    if((rtrn = read(sock_fd, buf, size)) < 1) {
        if(rtrn == -1) {
            perror("read_from_server read 2");
            exit(-1);
        }
        return NULL;
    }
    return buf;
}

/*************************************************************************/
//                                                                       //
//     send_to_server(): sends data to the server by sending             //
//                       a packet containing the size of the             //
//                       data being sent prior to the data itself        //
//                                                                       //
//     Calling Convention:                                               //
//                                                                       //
//              1. Sending an integer:                                   //
//                                                                       //
//                      int my_int = 5;                                  //
//                      send_to_server(&my_int, sizeof(my_int);          //
//                                                                       //
//              2. Sending a string:                                     //
//                                                                       //
//                      char string[] = "some string";                   //
//                      send_to_server(string, strlen(string));          //
//                                                                       //
/*************************************************************************/

int send_to_server(void *data, size_t size){
   
    size_t nb_size = htonl(size);
    
    if(write(sock_fd, &nb_size, sizeof(size)) == -1) //Writing in the size first
        return -1;

    return write(sock_fd, data, size);          //Writing the actual data

}



/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
//                                                                       //
//             YOU MUST IMPLEMENT THE FOLLOWING FUNCTIONS                //
//                                                                       //
//                  |               |               |                    //
//                  |               |               |                    //
//                  |               |               |                    //
//                \ | /           \ | /           \ | /                  //
//                 \|/             \|/             \|/                   //
//                  V               V               V                    //
//                                                                       //
//                                                                       //
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
/*$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/


/*************************/
//      RP_CONNECT       //
/*************************/

int rp_connect(const char *address, unsigned short port){

    /************************************/
    //         PART 2 OF PROJECT        //                                    
    /************************************/
    
    struct sockaddr_in server_addr;
    struct hostent *h;

    // 1. Get the IP Address associated with the address given 
    // Fill in hostent struct by calling gethostbyname()
    h = gethostbyname(address);

    // 3. Zero out struct with bzero
    bzero((char*) &server_addr, sizeof(server_addr));

    // 4. Set up the rest of the struct fields (family, sin_addr, and port)
    server_addr.sin_family = (short) AF_INET;
    bcopy((char *) h->h_addr, (char *) &server_addr.sin_addr, h->h_length );
    server_addr.sin_port = htons(port);

    // 5. Create socket **!! USE GLOBAL sock_fd VARIABLE TO STORE FILE DESCRIPTOR !!** 
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    // 6. Connect to server by calling connect()
    int conSuccess = connect(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
    //printf("Connection attempted and it returns %d\n", conSuccess);
    // 7. Return 0 on success and return -1 on error
    return conSuccess;
}


/*************************/
//        RP_OPEN        //
/*************************/

int rp_open(const char *pathname, int flags, ...) { 
    va_list arg_list;
    int checkMode = O_CREAT & flags;
    //int fd;
    mode_t mode;
    if(checkMode == O_CREAT){
        va_start(arg_list, flags);
        mode = va_arg(arg_list, mode_t);
    }

    //fd = open(pathname, flags, mode);//Should be moved to the server
    
    // Check if there will be a mode passed
    // This can be done by chekcing the flags to see if O_CREAT is speacified

    /************************************/
    //         PART 1 OF PROJECT        //                                    
    /************************************/
    
    // Call the syscall with argumnents given, return result to user, and set errno accordingly
    //We're given pathname, flags, ...
    
    va_end(arg_list);
    //errno = errno;
    //return fd;

    /************************************/
    //         PART 2 OF PROJECT        //                                    
    /************************************/
    
    // Send type of call to server
    int openType = (int)OPEN_CALL; 
    send_to_server(&openType,sizeof(openType));
    // Send each argument to the server (pathname, flags, mode)
    send_to_server((char*)pathname, strlen(pathname));
    send_to_server(&flags,sizeof(flags));
    send_to_server(&mode,sizeof(mode));
    // Read return of open coming back from server
    int *returnServer = (int*)read_from_server();
    int fd = *returnServer;
    free(returnServer);
    // Read errno sent from server
    int *tempErrono = (int*) read_from_server();
    errno = *tempErrono;
    free(tempErrono);
    // Return the return of open
    return fd;
}


/*************************/
//        RP_CLOSE       //
/*************************/

int rp_close(int fd){

    /************************************/
    //         PART 1 OF PROJECT        //                                    
    /************************************/

    // Call the syscall with argumnents given, return result to user, and set errno accordingly
    //int success = close(fd); //Return 0 on success or -1 on error
    errno = errno;

    //return success;
    
    /************************************/
    //         PART 2 OF PROJECT        //                                    
    /************************************/
    
    // Send type of call to server
    int closeType = CLOSE_CALL;
    send_to_server(&closeType, sizeof(closeType));
    // Send each argument to the server
    send_to_server(&fd, sizeof(fd));
    // Read return of open coming back from server 
    int *openReturn = (int*)read_from_server();
    int openValue = *openReturn;
    free(openReturn);
    // Read errno sent from server
    int *tempErrono = (int*) read_from_server();
    errno = *tempErrono;
    free(tempErrono);
    // Return result
    return openValue;

}


/*************************/
//        RP_READ        //
/*************************/

ssize_t rp_read(int fd, void *buf, size_t count) {
    /************************************/
    //         PART 1 OF PROJECT        //                                    
    /************************************/

    // Call the syscall with argumnents given, return result to user, and set errno accordingly
    //ssize_t byteRead = read(fd, buf, count);
    //errno = errno;
    //return byteRead;
    
    /************************************/
    //         PART 2 OF PROJECT        //                                    
    /************************************/
    // Send type of call to server
    int readType = (int)READ_CALL;
    send_to_server(&readType, sizeof(readType));
    // Send each argument to the server
    send_to_server(&fd, sizeof(fd));
    //send_to_server(buf, count);
    send_to_server(&count, sizeof(count));
    
    // Read return of read coming back from server 
    ssize_t *readReturn = (ssize_t*)read_from_server();
    ssize_t byteRead = *readReturn;
    //dprintf(2, "%d", byteRead);
    free(readReturn);
    // Read errno sent from server
    int *tempErrono = (int*) read_from_server();
    errno = *tempErrono;
    free(tempErrono);
    //
    char *buffReturn = (char*)read_from_server();
    //buf = buffReturn;
    memcpy(buf, buffReturn, count);
    //dprintf(2,"%s", buf);
    //free(buffReturn);
    // Return result
    return byteRead;
}


/*************************/
//       RP_WRITE        //
/*************************/

ssize_t rp_write(int fd, const void *buf, size_t count) {

    /************************************/
    //         PART 1 OF PROJECT        //                                    
    /************************************/

    // Call the syscall with argumnents given, return result to user, and set errno accordingly
    // ssize_t written = write(fd, buf, count);
    // errno = errno;
    // return written;
    
    /************************************/
    //         PART 2 OF PROJECT        //                                    
    /************************************/
    
    // Send type of call to server
    int writeType = WRITE_CALL;
    send_to_server(&writeType, sizeof(writeType));
    // Send each argument to the server
    send_to_server(&fd, sizeof(fd));
    send_to_server((void*)buf, count);
    send_to_server(&count, sizeof(count));

    // Read return of open coming back from server 
    ssize_t *writeReturn = (ssize_t*)read_from_server();
    ssize_t written = *writeReturn;
    // Read errno sent from server
    int *tempErrono = (int*) read_from_server();
    errno = *tempErrono;

    free(writeReturn);
    free(tempErrono);
    // Return result
    return written;
}


/*************************/
//       RP_LSEEK        //
/*************************/

off_t rp_lseek(int fd, off_t offset, int whence) {

    /************************************/
    //         PART 1 OF PROJECT        //                                    
    /************************************/

    // Call the syscall with argumnents given, return result to user, and set errno accordingly
    // off_t numOffset = lseek(fd, offset, whence);
    // errno = errno;
    // return numOffset;
    
    /************************************/
    //         PART 2 OF PROJECT        //                                    
    /************************************/
    
    // Send type of call to server
    int lseekType = LSEEK_CALL;
    send_to_server(&lseekType, sizeof(lseekType));

    // Send each argument to the server
    send_to_server(&fd, sizeof(fd));
    send_to_server(&offset, sizeof(offset));
    send_to_server(&whence, sizeof(whence));

    // Read return of open coming back from server 
    off_t *lseekReturn = (off_t*) read_from_server();
    off_t numOffset = *lseekReturn;
    // Read errno sent from server
    int *tempErrono = (int*) read_from_server();
    errno = *tempErrono;
    free(tempErrono);
    // Return result
    return numOffset;
}


/*************************/
//      RP_CHECKSUM      //
/*************************/

short rp_checksum(int fd) {
   
    /************************************/
    //         PART 2 OF PROJECT        //                                    
    /************************************/

    unsigned char checksum = 0;
    
    // Send type of call to server
    int checkSumType = CHECKSUM_CALL;
    send_to_server(&checkSumType, sizeof(checkSumType));
    // Send each argument to the server
    send_to_server(&fd, sizeof(fd));

    // Read return of open coming back from server 
    int *lseekReturn = (unsigned char*)read_from_server();
    checksum = *lseekReturn; //Might be wrong
    free(lseekReturn);
    // Read errno sent from server
    int *tempErrono = (int*) read_from_server();
    errno = *tempErrono;
    free(tempErrono);
    // Return result
    return checksum; //LOOK INTO:Right now returning an unsigned char 
}
