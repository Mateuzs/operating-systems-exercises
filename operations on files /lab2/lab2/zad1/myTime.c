//
//  myTime.c
//  lab2
//
//  Created by Mateusz Zembol on 16.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "myTime.h"
#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <errno.h>
#include <dlfcn.h>



void setTime(struct rusage * usage, double *systime,double *usertime){
    
    getrusage(RUSAGE_SELF, usage);
    
    *systime = -(usage->ru_stime.tv_sec + ((usage->ru_stime.tv_usec)/(double)1000000  ));
    *usertime = -(usage->ru_utime.tv_sec + ((usage->ru_utime.tv_usec)/(double)1000000 ));
    
}

void countTime(struct rusage * usage,double *systime,double *usertime, FILE *file){
    
    getrusage(RUSAGE_SELF, usage);
    
    *systime  += (usage->ru_stime.tv_sec + ((usage->ru_stime.tv_usec)/(double)1000000 ));
    *usertime += (usage->ru_utime.tv_sec + ((usage->ru_utime.tv_usec)/(double)1000000 ));
    
    fprintf(file, "\n\nTime taken to perform this task:\nsystem time: %f s\nuser time: %f s\n", *systime,*usertime);

    
}
