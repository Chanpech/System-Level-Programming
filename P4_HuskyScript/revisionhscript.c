
//Regular passing in of argument
if(mkdir(directory,0700) == -1){}

//To way to wait for the child 
//Waitpid()
//Signal() way

open( file, O_WRONLY, O_TRUNC, O_CREATE) //No O_Append

//child fork()
close(stdin_pipe[1]);
close(stdout_pipe[0]);
close(stderr_pip[0]);

close(0);close(1);close(2);

dup();dup();dup();
close();
execvp();
//Parent
while(1){
    FD_ZERO();
    FD_SET();

    if(!stdin_close)
        FD_SET(stdin_pipe_to_program[1], &write_set);
    if(select_rtrn = select()){

        if(FD_ISSET(0,&read_set)&& !stdin_close){
            if(FD_ISSET(stdin_pipe_to_program[1], &write_set)){
                if(stdin_bytes_read == 0){

                }else{

                }
            }
        }

        if(FD_ISSET(0,&read_set)&& !stdin_close){
            if(FD_ISSET(stdin_pipe_to_program[1], &write_set)){
                if(stdin_bytes_read == 0){

                }else{

                }
            }
        }
        if(FD_ISSET(0,&read_set)&& !stdin_close){
            if(FD_ISSET(stdin_pipe_to_program[1], &write_set)){
                if(stdin_bytes_read == 0){

                }else{

                }
            }
        }
    }

    
}
