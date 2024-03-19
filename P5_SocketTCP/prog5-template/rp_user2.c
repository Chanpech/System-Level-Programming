/**
 * Author: Chanpech Hoeng
 * Dates: 11/6/2023
 * Description: Client user 2 simulates the operations of syscall on the server.
 * This user2 will open local and remote files. Then lseek to 10th bytes
 * and performs the copy texts in the file from remote to local.
*/

#include "rp_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    //Seek to position 10 in a remote file
     char *remhost; unsigned short remport;
    // int sock, left, num, put;
    // struct sockaddr_in remote;
    // struct hostent *h;

    //Read a remote file
    if(argc < 5){
        exit(-1);
    }
    for(int i = 0; i < argc; i++){
        printf("%s ", argv[i]);
    }
    printf("\n");

    remhost = argv[1];
    remport = atoi(argv[2]);
    
    int connect = rp_connect(remhost,remport);
    int remoteFd = rp_open(argv[3], O_RDWR);  
    //int remoteFd = rp_open("remote.files", O_RDWR);
    int localFd = open(argv[4], O_RDWR | O_CREAT | O_TRUNC, 0644);    
    
    printf("Socket is now connected: %d\n", connect);
    printf("Remote files fd %d\n", remoteFd);
    printf("Local files fd %d\n", localFd);

    if(remoteFd == -1){
        perror("Remote Open");
        return -1;
    }
    if(localFd == -1){
        perror("Local Open");
        return -1;
    }

    //Copy the remaining contents to the local file
    int byteRead = 0;
    char buff[2];

    off_t offset = rp_lseek(remoteFd, 10, 1);
    printf("Offset to %ld\n", offset);
    while((byteRead = rp_read(remoteFd, buff, 1)) > 0){
        if(byteRead == -1){
            perror("Rp_read");
            exit(-1);
        }
       int written = write(localFd, buff, strlen(buff));
        if(written == -1){
            perror("Write");
            exit(-1);
       }
    }
    //printf("File been copied remote to local\n");
   
    //rp_checksum(remoteFd);
    printf("CheckSum Result %02x\n",rp_checksum(remoteFd)); //Padding zero with 2 hex long

    rp_close(remoteFd);
    close(localFd);
    return 0;
}
