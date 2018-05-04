//
//  main2.c
//  lab3
//
//  Created by Mateusz Zembol on 23.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc,char *argv[], char *envp[]){
    //this program runs for about 3 sec, so limitation of 1s will cause break
    int a=2;
    while(1){
        a+=1;
        if(a>1000000000){
            break;
        }
    }
    
    struct rlimit timeLim;
    struct rlimit virMemLim;
    getrlimit(RLIMIT_AS, &virMemLim);
    getrlimit(RLIMIT_CPU, &timeLim);
    printf("main2: %d loops, time limitation: %ld sec, memory limitation: %f MB\n",a,(long)(timeLim.rlim_max),(double)(virMemLim.rlim_max/(1024*1024)));
    return 0;
}
