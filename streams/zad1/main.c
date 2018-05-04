#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

// example ls -l | grep main. | head -n 1
 char **environment;


int main(int argc,char *argv[]){
    size_t len=0;
    char* line;
    char bufor[128];
    const char signPipe[2] = "|";
    const char signSpace[2] = " ";
    char* tokens[128];
    char* tokens2[128];
    int argSize;
    int argSize2;
   
    printf(" hello in my interpreter, write some commands with pipes: \n");
   
    while (getline(&line, &len, stdin) != -1) {
        
        argSize=0;
        tokens[0] = strtok(line,signPipe);    //parsujemy po pipe'ach
        while( tokens[argSize] != NULL ){
            ++argSize;
            tokens[argSize]=strtok(NULL, signPipe);
        }
        
        int pipefd[argSize][2];
       
        for(int i=0;i<argSize;i++){
            if(pipe(pipefd[i])!=0){
                printf("nie udalo sie stworzyc pipe %d\n",i);
                exit(1);
            }
        }
        
        
        
        for(int command=0;command<argSize;command++){
            argSize2=0;
            tokens2[0] = strtok(tokens[command],signSpace);
            while( tokens2[argSize2] != NULL ){
                ++argSize2;
                tokens2[argSize2]=strtok(NULL, signSpace);
            }
            
            tokens2[argSize2]=NULL;
            
            if(command==argSize-1){//jesli ostatni
                tokens2[argSize2-1][strlen(tokens2[argSize2-1])-1]=0;
            }
            if(fork()==0){ // tworzymy dziecko-proces
                
                close(pipefd[command][0]); // zamykamy koncowke odczytu
                
                if(command!=0) dup2(pipefd[command-1][0], STDIN_FILENO);//dostaje dane w fd1 a zwraca w fd2
                if(dup2(pipefd[command][1],STDOUT_FILENO)==-1){
                    printf("errno = %d\n\n",errno);
                }
                execvpe(tokens2[0],tokens2,environment);
                perror("execvpe");
                exit(1);
            }
            
            else{
              
                close(pipefd[command][1]);
                if(command!=0) close(pipefd[command-1][0]);
                wait(NULL);//czekamy az sie skonczy
                if(command==argSize-1){
                    while(read(pipefd[command][0],&bufor,1)>0){
                        write(STDOUT_FILENO,&bufor,1); // przesylamy dane do polecenia
                    }
                    write(STDOUT_FILENO,"\n",1); // przesylamy "/n"
                }
            }
        }
    }
    
    return 0;
}
