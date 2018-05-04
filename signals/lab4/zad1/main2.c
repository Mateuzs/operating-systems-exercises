//
//  main.c
//  lab4
//
//  Created by Mateusz Zembol on 28.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>


int flag = 0;


void sig_int_handler(int sigNum){
    printf("\n\nOdebrano sygnał SIGINT\n\n");
    exit(0);
}


void sig_stop_handler(int sigNum){
    
    if(flag ==0){
        flag = 1;
        printf("\n\n Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n\n");
    }else{
        flag = 0;
        printf("\n\n");
        return;
    }
    sigpause(SIGTSTP);
}



int main(int argc,char *argv[]){
    
   
    signal(SIGINT, sig_int_handler);
    
    struct sigaction oldIntAct;
    sigaction(SIGTSTP, NULL, &oldIntAct);
    struct sigaction newIntAct=oldIntAct;
    newIntAct.sa_handler = sig_stop_handler;
    sigaction(SIGTSTP, &newIntAct, &oldIntAct);
    
    
    
    while(1){
       if(fork()==0){
            execlp("date", "j", NULL);
        }
        sleep(1);
        
    }
    
    
    return 0;
}

