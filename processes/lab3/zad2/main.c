//
//  main.c
//  lab3
//
//  Created by Mateusz Zembol on 23.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main.h"

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char *argv[]){
    
    
    if(argc!=2){
        printf("\nGive as argument your file name!\n");
        return 1;
    }
    
    FILE * file;
    char * line = NULL;
    ssize_t read;
    size_t length = 0;
    
    
    file = fopen(argv[1], "r");
    if (file == NULL){
        printf("\nCouldn't open the file :( \n");
        return 1;
    }
    const char space[2] = " ";
    char* tokens[128];
    int i;
    while ((read = getline(&line, &length, file)) != -1) {
        
        i=0;
        tokens[0] = strtok(line,space);
        while( tokens[i] != NULL ){
            ++i;
            tokens[i]=strtok(NULL, space);
        }
        
        if(!feof(file)){//let's delete end of line, if not the end of file
            tokens[i-1][strlen(tokens[i-1])-1]=0;
        }
        
        if(line[0]=='#'){//environment variable
            tokens[0]=tokens[0]+1;
            
            if(i==2){//assigment
                const int overwrite=1;//for overwrite != 0: unconditionally set variable name
                setenv(tokens[0], tokens[1], overwrite);
                
                if(getenv(tokens[0])!=NULL && strcmp(tokens[1],getenv(tokens[0]))==0){
                    printf("Environment variable was set from %s on %s\n",tokens[0],getenv(tokens[0]));
                }
            }
            else if(i==1){//delete
                unsetenv(tokens[0]);
                if(getenv(tokens[0])==NULL){
                    printf("Variable %s has been deleted\n",tokens[0]);
                }
            }
            else{
                printf("Too many arguments to delete environment variable.");
                return 1;
            }
        }
        //execute the program
        else if(i>0){
            //child
            if(fork()==0){
                execvp(tokens[0], tokens);
                perror("execvp");
                return 1;
            }
            //adult
            else{
                int result;
                wait(&result);
                if(result!=0){
                    printf("Command %s ended with failure, result: %d\n",line,result);
                    return 1;
                }
            }
        }
    }
    
    fclose(file);
    if (line)
        free(line);
    return 0;
}
