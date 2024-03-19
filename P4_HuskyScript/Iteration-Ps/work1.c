#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <string.h>
#include <signal.h>
/**
 * pIpe from terminal to hscript already exist
 *
 * pipe we need to worry about is hscript to ls -l
 * hscript stdin is going out to ls-l
 * ls -l going out
 */
// Have hscript
//  0 should have whatever you're typing don't include <arglist and ls. Inlucde redirectional <>//
// Use select to check terminal 0
// Use select to check ls -l 1 and 2.
//  char* assembledArguments(char **argv, int size){
//      char *arugments = (char*)malloc(size * 2);
//      int i = 2;
//      while(i < size -1){
//          strcat(arugments, argv[i]);
//          i++;
//      }
//      return arugments;
// }
volatile sig_atomic_t status = 1;
void childHandler()
{ // Handle SIG_Child signal
    status = 0;
    write(1, "Child been handled\n", sizeof("Child been handled\n"));
}

char **assembledArguments(char **argc, char **argv, int size)
{
    int i = 2;
    int j = 1;
    while (i < size - 1)
    {
        argc[j] = argv[i];
        printf("%s ", argc[j]);
        i++;
        j++;
    }
    argc[j] = NULL;
    printf("\n");
    return argc;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        char error[] = "./hscript <program name> <arguments> <directory>\n";
        write(2, error, sizeof(error));
        return -1;
    }
    // for (int i = 1; i < argc; i++)
    // {
    //     printf("%s ", argv[i]);
    // }
    // printf("Size %d\n", argc);

    int fd0, fd1, fd2, fd_dir;
    pid_t is_parent;
    // int p2c[2], c2p[2];
    int p2c0[2], c2p1[2], c2p2[2];
    char *program = argv[1];
    char *arguments[argc - 1];
    char *dir = argv[argc - 1];
    char file1[50];
    char file2[50];
    char file3[50];
    if (argc < 3)
    {
        arguments[0] = program;
        arguments[1] = NULL;
    }
    else
    {
        arguments[0] = program;
        assembledArguments(arguments, argv, argc);
        //printf("%s %s\n",arguments[0], arguments[1]);
    }
    snprintf(file1, strlen(dir) + 3, "%s/0", dir);
    snprintf(file2, strlen(dir) + 3, "%s/1", dir);
    snprintf(file3, strlen(dir) + 3, "%s/2", dir);

    // printf("%s\n", file1);
    // printf("%s\n", argv[3]);
    if ((fd_dir = mkdir(dir, 0700)) == -1)
    {
        perror("EXISTS\n");
        return -1;
    }
    fd0 = open(file1, O_WRONLY | O_TRUNC | O_CREAT | O_APPEND, 0600);
    fd1 = open(file2, O_WRONLY | O_TRUNC | O_CREAT | O_APPEND, 0600);
    fd2 = open(file3, O_WRONLY | O_TRUNC | O_CREAT | O_APPEND, 0600);

    // write(fd2, "stdin", strlen("stdin"));
    if (fd0 == -1 | fd1 == -1 | fd2 == -1)
    {
        perror("Files 0,1,2");
        return -1;
    }

    // write(fd0, "stdin", sizeof("stdin"));
    if (pipe(p2c0) == -1 || pipe(c2p1) == -1 || pipe(c2p2) == -1)
    { // fd_pip[0] = read and fd_pip[1] is write
        perror("PIPE");
        return -1;
    }
    char *stdinBuffer = (char *)malloc(sizeof(1024));
    // int byteRead;

    // fd_set write_err_set;
    // fd_set read_set_terminal;

    // printf("Argument %s\n", args);
    is_parent = fork();

    if (!is_parent)
    {
       // Connect stdin to our pipe
        close(p2c0[1]); // stdin
        // close(0); //Closing stdin so we can attach it to our pipe
        dup2(p2c0[0], 0); // Create alias of our pipe
        close(p2c0[0]);
        // //look into  dup2
        close(c2p1[0]);
        // close(1);
        dup2(c2p1[1], 1);
        close(c2p1[1]);

        close(c2p2[0]);
        // close(2);
        dup2(c2p2[1], 2);
        close(c2p2[1]);

        // Child waiting for parent to send something from p2c[0]
        // This is the content of the file read in
        //  char readBuff[1024];
        //  while(read(p2c0[1], readBuff, sizeof(readBuff)) != 0){
        //  }
        execvp(program, arguments); // Overwrite current processs with given program
        // free(args);
        perror("Execvp Returned\n");
        exit(1);
    }
    else
    {
        // Parent
        struct sigaction childDied;
        // childDied.sa_flags = SA_RESTART;
        childDied.sa_handler = &childHandler;
        sigemptyset(&childDied.sa_mask);
        childDied.sa_flags = 0;

        if (sigaction(SIGCHLD, &childDied, NULL) == -1)
        { // Handle when the child terminate
            perror("sigaction");
            exit(1);
        }

        int i = 10;
        int ndfs_decrement = 6;
        int ndfs = c2p2[0] + 1; // Highest number fd plus 1
        int pipeEmpty = 0;   
        while (1)
        {
            fd_set read_set;
            fd_set write_set;
            // fd_set error_set;
            //  Clear the set
            FD_ZERO(&read_set);
            FD_ZERO(&write_set);
            // Add fd0 to read_set
            FD_SET(0, &read_set);
            FD_SET(c2p1[0], &read_set);
            FD_SET(c2p2[0], &read_set);
            // Add fd1 to write_set
            FD_SET(1, &write_set);
            FD_SET(2, &write_set);
            FD_SET(fd0, &write_set);
            FD_SET(fd1, &write_set);
            FD_SET(fd2, &write_set);
            FD_SET(p2c0[1], &write_set);

            char *buffTermIn = (char*)malloc(sizeof(char) * 1024);
            char buffChildOut[1024];
            char buffChildErr[1024];
            int byteRead = 0;
            int fdClose = 0;
            
            int triggerWrite = 0;
            int count = 0;
            int beenWritten = 0;
            if (select(ndfs, &read_set, &write_set, NULL, NULL) > 0)
            {
                // If child process the content should still exists.
                // We should empty this part out.
                // We're handling multiple arguments
               
                if (FD_ISSET(0, &read_set)) // 0 stdin is ready
                {
                    //printf("Pipe read terminal 0 stream is Read\n");
                    byteRead = read(0, buffTermIn, 1);
                    if (byteRead == -1)
                    {
                        perror("READ");
                        exit(-1);
                    }
                    if (byteRead == 0)
                    {
                        // When there is nothing left to read
                        // Remove the 0 fd from the read_set
                        //close(0);
                        //close(p2c0[1]);
                        //close(p2c0[0]);
                        ndfs_decrement--;
                        fdClose++;
                        count++;
                    }
                }
                if (FD_ISSET(c2p1[0], &read_set))
                { // c2p1 read is ready
                    // Equivalent to their stdout on the child
                    //printf("Pipe from c2p1[0] read\n");
                    // Parent read in the child fd1 (previously stdout) from the c2p1[0]
                    int readSize = read(c2p1[0], buffChildOut, sizeof(buffChildOut));
                    if (readSize == 0)
                    {
                        // When there's nothing left to read
                        // Remove the c2p1[0] from the read_set
                        // Then close c2p1[0]
                        write(1, "READ 0: Nothing from c2p1[0] to read\n", sizeof("READ 0: Nothing from c2p1[0] to read\n"));
                        //close(c2p1[0]);
                        ndfs_decrement--;
                        fdClose++;
                    }
                    else if(readSize == -1){
                        perror("READ");
                    }else
                    {
                        // Write c2p2[0] to the 1 file
                        write(fd1, buffChildOut, strlen(buffChildOut));
                    }
                }
                if (FD_ISSET(c2p2[0], &read_set))
                { // There's data from the c2p2[0]
                    // Equivalent to the child stderr
                    //printf("Pipe from c2p2[0] read\n");
                    int readSize = read(c2p2[0], buffChildErr, sizeof(buffChildErr));
                    if (readSize == 0)
                    {
                        // When there's nothing left to read
                        // Remove the c2p2[0] from the read_set
                        // Then close c2p2[0]
                        write(1, "READ 0: Nothing from c2p2[0] to read\n", sizeof("READ 0: Nothing from c2p2[0] to read\n"));
                        //close(c2p2[0]);
                        ndfs_decrement--;
                        fdClose++;
                    }
                    else
                    {
                        // Write c2p2[0] to the 2 file
                        write(fd2, buffChildErr, strlen(buffChildErr));

                    }
                }

                if (FD_ISSET(1, &write_set))
                { // Parent's stdout fd 1 is ready for writing
                    //printf("fd 1 is ready\n");
                    int writeSize = 0;
                    if (byteRead != 0)
                    {
                        writeSize = write(1, buffChildOut, strlen(buffChildOut));
                        if (writeSize == -1)
                        {
                            perror("WRITE\n");
                            return -1;
                        }
                    }
                    if (writeSize == 0)
                    {
                        // There's nothing left to write to stdout
                        //close(1);
                        ndfs_decrement--;
                        fdClose++;
                    }
                }
                if (FD_ISSET(2, &write_set))
                { // Parent fd 2 stderr is ready to be written
                    // Writing child stderr to parent stderr
                    //printf("fd 2 is ready\n");
                    int writeSize = 0;
                    if (byteRead != 0)
                    {
                        writeSize = write(2, buffChildErr, strlen(buffChildErr));
                        if (writeSize == -1)
                        {
                            perror("WRITE\n");
                            return -1;
                        }
                    }
                    if (writeSize == 0)
                    {
                        // There's nothing left to write to stdout
                        //close(2);
                        ndfs_decrement--;
                        fdClose++;
                    }
                }
                if (FD_ISSET(p2c0[1], &write_set))
                { // p2c0[1] pipe is ready to be written
                    // Send what ever the parent read from terminal stdin to the child

                    //printf("p2c0 pipe fd Write Stream\n");
                    int writeSize = 0;
                    if (byteRead != 0)
                    {
                        writeSize = write(p2c0[1], buffTermIn, strlen(buffTermIn));
                        if (writeSize == -1)
                        {
                            perror("WRITE\n");
                            return -1;
                        }
                    }
                    if (writeSize == 0)
                    {
                        // There's nothing left to write to stdout
                        //close(p2c0[1]);
                        ndfs_decrement--;
                        fdClose++;
                    }
                }
                if (FD_ISSET(fd0, &write_set))
                {
                    int wroteByte = write(fd0, buffTermIn, strlen(buffTermIn));
                    if (wroteByte == -1)
                    {
                        perror("Wrote fd0");
                        exit(1);
                    }
                    else if (wroteByte == 0)
                    {
                       // close(fd0);
                        fdClose++;
                    }
                }
                // if (FD_ISSET(fd1, &write_set))
                // {
                //     int wroteByte = write(fd1, buffChildOut, strlen(buffChildOut));
                //     if (wroteByte == -1)
                //     {
                //         perror("Wrote fd0");
                //         exit(1);
                //     }
                //     else if (wroteByte == 0)
                //     {
                //         //close(fd1);
                //         fdClose++;
                //     }
                //  }
                // if (FD_ISSET(fd2, &write_set))
                // {
                //     int wroteByte = write(fd2, buffChildErr, strlen(buffChildErr));
                //     if (wroteByte == -1)
                //     {
                //         perror("Wrote fd0");
                //         exit(1);
                //     }
                //     else if (wroteByte == 0)
                //     {
                //         //close(fd2);
                //         fdClose++;
                //     }
                // }
            }
            //printf("After ndfs: %d\n", ndfs_decrement);

            // Write to file 0.
            // sleep(1);
            if (status == 0)
            {
                break;
            }
            
        }
        //     //close(fd_pipe[0]); //File been close and nothing left to read.
    }
    // wait(NULL);

    // Writing back to the terminal
    // write(1,"", );
    // write(2,"",);

    // Storing these to the out_dir

    // might need child PID
    close(fd0);
    close(fd1);
    close(fd2);
    // free(args);
    return 0;
}
