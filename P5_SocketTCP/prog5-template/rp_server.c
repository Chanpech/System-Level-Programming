/**
 * Author: Chanpech Hoeng
 * Course: CS3411
 * Latest Update: Dec 8 2023
 * Description: TCP server that communicate with rp_client through 
 * manipulation of sockets. 
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

/*************************************************************************/
//                       DO NOT REMOVE THIS FUNCTION!!                   //
/*************************************************************************/

void this_is_for_the_autograder_do_not_delete(unsigned short port)
{

    int fd;
    if ((fd = open(".server_port", O_CREAT | O_TRUNC | O_WRONLY, 0600)) == -1)
        perror("autograder port file open error");

    char buf[6];
    sprintf(buf, "%d", port);
    if (write(fd, buf, strlen(buf)) == -1)
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

void *read_from_client()
{
    size_t size;
    char *buf;
    int rtrn;

    if ((rtrn = read(0, &size, sizeof(size))) < 1)
    {
        if (rtrn == -1)
        {
            perror("read_from_client read 1");
            exit(-1);
        }
        return NULL;
    }

    size = ntohl(size);

    buf = malloc(size);
    if ((rtrn = read(0, buf, size)) < 1)
    {
        if (rtrn == -1)
        {
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

int send_to_client(void *data, size_t size)
{

    size_t nb_size = htonl(size);

    if (write(1, &nb_size, sizeof(size)) == -1)
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
int checksum(int fd)
{ 

    // Seek to beginning of file
    char buf[4096];
    lseek(fd, 0, SEEK_SET);

    // Read blocks of size 4096 from file
    ssize_t byteRead;
    unsigned char checksum = 0;

    while((byteRead = read(fd, buf, sizeof(buf))) > 0)
    {
        // Parse all 4096 bytes in block and perform checksum
        // for everybyte do the check sum
        for (int i = 0; i < byteRead; i++)
        {
            checksum = checksum ^ buf[i];
            //dprintf(2, "%08x \n", checksum);
        }
    }

    // Return the checksum computed
    return checksum;
}

/*************************/
//      HANDLE OPEN      //
/*************************/

void handle_open()
{

    int fd;
    // Read in the argumets of sent by client
    char *pathname = (char *)read_from_client();
    int *flags = (int *)read_from_client(); // Might have to dereference
    mode_t *mode = (mode_t *)read_from_client();

    // Call the syscall with arguments
    fd = open(pathname, *flags, *mode);
    // Send the return of open to the client
    send_to_client(&fd, sizeof(fd));
    // Send the errno information to the client
    send_to_client(&errno, sizeof(errno));
}

/*************************/
//      HANDLE CLOSE     //
/*************************/

void handle_close()
{

    // Read in the argumets of sent by client
    int *fd = (int *)read_from_client();
    // Call the syscall with arguments
    int closeSuccess = close(*fd);
    free(fd);
    // Send the return of close to the client
    send_to_client(&closeSuccess, sizeof(closeSuccess));
    // Send the errno information to the client
    send_to_client(&errno, sizeof(errno));
}

/*************************/
//      HANDLE READ      //
/*************************/

void handle_read()
{
    // Read in the argumets of sent by client
    int *fd = (int *)read_from_client();
    // void *buf = (void*)read_from_client();
    size_t *count = (size_t *)read_from_client();

    char *buf = malloc(sizeof(char *) * (*count));
    // Call the syscall with arguments
    ssize_t byteRead = read(*fd, buf, *count);
    // Send the return of read to the client
    send_to_client(&byteRead, sizeof(byteRead));
    // Send the errno information to the client
    send_to_client(&errno, sizeof(errno)); // Is this legal
    // Send the read data to the client
    send_to_client(buf, *count);

    // Free the variables
    free(fd);
    free(buf);
    free(count);
}

/*************************/
//      HANDLE WRITE     //
/*************************/

void handle_write()
{
    // sleep(10);
    //  Read in the argumets of sent by client
    int *fd = (int *)read_from_client();
    void *buf = (void *)read_from_client();
    size_t *count = (size_t *)read_from_client();

    // Call the syscall with arguments
    ssize_t byteWritten = write(*fd, buf, *count);
    // Send the return of write to the client
    send_to_client(&byteWritten, sizeof(byteWritten));
    // Send the errno information to the client
    send_to_client(&errno, sizeof(errno));
    // Free Variables
    free(fd);
    free(buf);
    free(count);
}

/*************************/
//      HANDLE LSEEK     //
/*************************/

void handle_lseek()
{

    // Read in the argumets of sent by client
    int *fd = (int *)read_from_client();
    off_t *offset = (off_t *)read_from_client();
    int *whence = (int *)read_from_client();

    // Call the syscall with arguments
    off_t numOffset = lseek(*fd, *offset, *whence);

    // Send the return of lseek to the client
    send_to_client(&numOffset, sizeof(numOffset));

    // Send the errno information to the client
    send_to_client(&errno, sizeof(errno));
    // Free Variables
    free(fd);
    free(offset);
    free(whence);
}

/*************************/
//    HANDLE CHECKSUM    //
/*************************/
// Assumed file is opened
// Return errono as is. Don't worry what the value is.
void handle_checksum()
{
    // Read in the argumets of sent by client
    int *fd = (int *)read_from_client();

    // Call the syscall with arguments
    unsigned char checkSumResult = (unsigned char)checksum(*fd);

    // Send the return of checksum to the client
    send_to_client(&checkSumResult, sizeof(checkSumResult));
    // Send the errno information to the client
    send_to_client(&errno, sizeof(errno));
    // Free variables
    free(fd);
}

/*************************/
//          MAIN         //
/*************************/

int main()
{

    struct sockaddr_in client_addr, server_addr;

    // Zero out server and client addr structs
    bzero((char *)&client_addr, sizeof(client_addr));
    bzero((char *)&server_addr, sizeof(server_addr));

    // Get socket file descriptor
    const int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("The server's listening socket # is %d\n", sock_fd);

    // Load server address information (family, sin_addr.s_addr, sin_port)
    // The sin_addr.s_addr should be fill with wild card so that it works for any machine.
    server_addr.sin_family = (short)AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); /*WildCARD*/
    server_addr.sin_port = htons(0);                 /* bind() will gimme unique port. */

    // Bind server address to socket
    bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // These lines will print out the port number used and write
    // it to a file that will be used by the grader
    //
    //                          DO NOT DELETE!!
    //
    socklen_t s_len = sizeof(server_addr);
    getsockname(sock_fd, (struct sockaddr *)&server_addr, &s_len); // Get the port assigned by bind()
    this_is_for_the_autograder_do_not_delete(ntohs(server_addr.sin_port));

    // Set socket to listen for connections
    listen(sock_fd, 1);

    /************************************/
    //         PART 2 OF PROJECT        //
    /************************************/

    while (1)
    {
        // Wait for client to connect
        int length = sizeof(client_addr);
        const int conn = accept(sock_fd, (struct sockaddr *)&client_addr, (socklen_t *)&length);
        printf("Accepted the requested client. Will talk to the client through descriptor: %d\n", conn);

        if (fork() == 0)
        {
            // fork() == 0 {do the rest}
            //  Redirects stdin and stdout to the socket
            //  These lines must be here to work with the send_to_client and read_from_client functions
            dup2(conn, 0);
            dup2(conn, 1);
            close(conn);

            // int syscallType =  *typeReturn;
            while (1)
            {
                // Read the type sent by the client
                void *typeReturn = read_from_client();
                int syscallType = *(int *)typeReturn;

                if (typeReturn == NULL)
                {
                    free(typeReturn);
                    break;
                }
                free(typeReturn);

                // // Do the appropriate action for the type sent by calling the appropriate handler
                //dprintf(2, "%d ", syscallType);

                if (syscallType == OPEN_CALL)
                {
                    handle_open();
                }
                else if (syscallType == CLOSE_CALL)
                {
                    handle_close();
                    break;
                }
                else if (syscallType == READ_CALL)
                {
                    handle_read();
                }
                else if (syscallType == WRITE_CALL)
                {
                    handle_write();
                }
                else if (syscallType == LSEEK_CALL)
                {
                    handle_lseek();
                }
                else if (syscallType == CHECKSUM_CALL)
                {
                    handle_checksum();
                }
                else
                {
                    break;
                }
                // // Wait for more commands (maybe a loop)
                // // Exit when EOF is sent to server (i.e. child has closed the connection)
            }
        }
    }

    dprintf(2, "Exiting...\n");
    /************************************/
    //        PART 2.5 OF PROJECT       //
    /************************************/

    // Modify the Part 2 code to now fork for every incoming connection

    return 0;
}
