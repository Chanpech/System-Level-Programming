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


/*************************************************************************/
//                       DO NOT REMOVE THIS FUNCTION!!                   //
/*************************************************************************/

void this_is_for_the_autograder_do_not_delete(unsigned short port) {
    
    int fd;
    if((fd = open(".server_port", O_CREAT | O_TRUNC | O_WRONLY, 0600)) == -1)
        perror("autograder port file open error");

    char buf[6];
    sprintf(buf, "%d%c", port, '\0');
    if(write(fd, buf, strlen(buf)) == -1)
       perror("autograder port file write error"); 

    close(fd);
    printf("SERVER LISTENING ON PORT %d\n", port);

}


/*************************************************************************/
//                                                                       //
//     read_from_client(): reads next packet of data                     //
//                         and returns pointer to the                    //
//                         data read                                     //
//                                                                       //
//     Calling Convention:                                               //
//                                                                       //
//              1. Reading an integer:                                   //
//                                                                       //
//                      int var = *(int*) read_from_client();            //
//                                                                       //
//              2. Reading a string:                                     //
//                                                                       //
//                      char *string = read_from_client();               //
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
//                      server --> [size of int] --> client              //
//                      server --> [integer value] --> client            //
//                                                                       //
//              Ex 2:                                                    //
//                                                                       //
//                      Sending a string is done like so:                //
//                                                                       //
//                      server --> [length of string] --> client         //
//                      server --> [string data] --> client              //
//                                                                       //
/*************************************************************************/

void* read_from_client(){
    size_t size;
    char *buf;
    int rtrn; 

    if((rtrn = read(0, &size, sizeof(size))) < 1) {
        if(rtrn == -1) {
            perror("read_from_client read 1");
            exit(-1);
        }
        return NULL;
    }

    size = ntohl(size);
    
    buf = malloc(size);
    if((rtrn = read(0, buf, size)) < 1) {
        if(rtrn == -1) {
            perror("read_from_client read 2");
            exit(-1);
        }
        return NULL;
    }
    return buf;
}

/*************************************************************************/
//                                                                       //
//     send_to_client(): sends data to the client by sending             //
//                       a packet containing the size of the             //
//                       data being sent prior to the data itself        //
//                                                                       //
//     Calling Convention:                                               //
//                                                                       //
//              1. Sending an integer:                                   //
//                                                                       //
//                      int my_int = 5;                                  //
//                      send_to_client(&my_int, sizeof(my_int);          //
//                                                                       //
//              2. Sending a string:                                     //
//                                                                       //
//                      char string[] = "some string";                   //
//                      send_to_client(string, strlen(string));          //
//                                                                       //
/*************************************************************************/

int send_to_client(void *data, size_t size){
   
    size_t nb_size = htonl(size);
    
    if(write(1, &nb_size, sizeof(size)) == -1)
        return -1;

    return write(1, data, size);

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
//       CHECKSUM        //
/*************************/

int checksum(int fd) {
    
    // Seek to beginning of file
    
    // Read blocks of size 4096 from file
        // Parse all 4096 bytes in block and perform checksum
    
    // Return the checksum computed
    return -1;
}


/*************************/
//      HANDLE OPEN      //
/*************************/

void handle_open() {
       
        // Read in the argumets of sent by client 

        // Call the syscall with arguments

        // Send the return of open to the client

        // Send the errno information to the client

} 


/*************************/
//      HANDLE CLOSE     //
/*************************/

void handle_close() {
        
        // Read in the argumets of sent by client 

        // Call the syscall with arguments

        // Send the return of close to the client

        // Send the errno information to the client

} 


/*************************/
//      HANDLE READ      //
/*************************/

void handle_read() {
        
        // Read in the argumets of sent by client 

        // Call the syscall with arguments

        // Send the return of read to the client

        // Send the errno information to the client
        
        // Send the read data to the client

} 


/*************************/
//      HANDLE WRITE     //
/*************************/

void handle_write() {
        
        // Read in the argumets of sent by client 

        // Call the syscall with arguments

        // Send the return of write to the client

        // Send the errno information to the client

} 


/*************************/
//      HANDLE LSEEK     //
/*************************/

void handle_lseek() {
        
        // Read in the argumets of sent by client 

        // Call the syscall with arguments

        // Send the return of lseek to the client

        // Send the errno information to the client

} 


/*************************/
//    HANDLE CHECKSUM    //
/*************************/

void handle_checksum() {
        
        // Read in the argumets of sent by client 

        // Call the syscall with arguments

        // Send the return of checksum to the client

        // Send the errno information to the client

}


/*************************/
//          MAIN         //
/*************************/

int main() {

    struct sockaddr_in client_addr, server_addr;

    // Zero out server and client addr structs

    // Get socket file descriptor
    const int sock_fd = 0;
   
    // Load server address information (family, sin_addr.s_addr, sin_port)

    // Bind server address to socket
   
    // These lines will print out the port number used and write
    // it to a file that will be used by the grader
    //                          
    //                          DO NOT DELETE!! 
    //                          
    socklen_t s_len = sizeof(server_addr);
    getsockname(sock_fd,(struct sockaddr *) &server_addr, &s_len);
    this_is_for_the_autograder_do_not_delete(ntohs(server_addr.sin_port));

    // Set socket to listen for connections    
    
    /************************************/
    //         PART 2 OF PROJECT        //                                    
    /************************************/
        
        // Wait for client to connect
        const int conn = 0;
        
        // Redirects stdin and stdout to the socket
        // These lines must be here to work with the send_to_client and read_from_client functions
        dup2(conn, 0);
        dup2(conn, 1);
        close(conn); 

        // Read the type sent by the client

        // Do the appropriate action for the type sent by calling the appropriate handler
        
        // Wait for more commands (maybe a loop)
        
        // Exit when EOF is sent to server (i.e. child has closed the connection)

    /************************************/
    //        PART 2.5 OF PROJECT       //                                    
    /************************************/
    
        // Modify the Part 2 code to now fork for every incoming connection

    return 0;        
}
