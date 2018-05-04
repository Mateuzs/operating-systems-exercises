//
//  main3.c
//  lab3
//
//  Created by Mateusz Zembol on 23.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc,char *argv[], char *envp[]){
    if(argc!=2){
        printf("Give an amount of MB to allocate : \n");
        return 1;
    }
    long long int size;
    sscanf(argv[1], "%lld", &size);
    char* a;
    
    a = (char *) malloc(1024*1024*size);

    if(a != NULL){
        printf("main3.c: allocation of %lld MB was successful",size);
    }
    else{
        printf("main3.c: allocation of %lld MB wasn't successful",size);
    }
    
    struct rlimit timeLimit;
    struct rlimit virtualMemoryLimit;
    getrlimit(RLIMIT_AS, &virtualMemoryLimit);
    getrlimit(RLIMIT_CPU, &timeLimit);
    printf("\ntime limit: %ld sec, memory limit: %f MB\n",(long)(timeLimit.rlim_max),(double)(virtualMemoryLimit.rlim_max/(1024*1024)));
    return 0;
}

