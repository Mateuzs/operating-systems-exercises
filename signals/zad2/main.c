//
//  main.c
//  lab4
//
//  Created by Mateusz Zembol on 08.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>



int requests; // numbers of requests
int* childrenPids;
int parentPid;
char isFree = 0; // checks if parent received as many sigUsr signals as amount of children
int signalsReceived=0;// amount of sigUsr signals from children
int globKid=-1;//currently checked child, -1 due to incrementing at the begining of the loop
int childPermission=0;//change to 1 when a child get a permission
int numberOfRealtimeSignals = 0;
int childNumber;



void signalUserHandler(int sigNum, siginfo_t* info, void* context){
   
    if(getpid()!=parentPid){
        printf("child with process number %d get a permission \n",getpid());
        childStop=1; // case sigUsr (allow) we need to stop the loop
         return;
    }
    
    ++globKid;
    childrenPids[globKid]=info->si_pid;
    
    if(isFree==1){//we can send signal immediately
        kill(childrenPids[globKid],SIGUSR1);
    }
    else{//jak jeszcze nie bo isFree=0
        signalsReceived+=1;
        if(signalsReceived>=requests){//we need to send permission every former children
            isFree=1;
            for(int i=0;i<=globKid;i++){
                kill(childrenPids[i],SIGUSR1);
            }
        }
    }
}

void realtimeSignalHandler(int sigNum,siginfo_t* info,void* context){
    numberOfRealtimeSignals++;
    printf("I've received %d realtime signal with number \"%d\" from child with number %d\n", numberOfRealtimeSignals,sigNum,info->si_pid);
}


void sigintHandler(int sigNum){
    if(getpid()!=parentPid){
        printf("Child %d with process id %d received sigint and terminates\n",childNumber,getpid());
        exit(0);
    }
    for(int i=0;i<=globKid;i++){
        if(childrenPids[i]!=0){
            kill(childrenPids[i],SIGINT);
        }
    }
    printf("Parent %d received sigint and terminates\n",parentPid);
    exit(0);
}


int main(int argc,char *argv[]){
    
    if(argc!=3){
        printf("You need to define N children and K requests!\n");
        return 1;
    }
    
    int N = atoi(argv[1]);
    requests=atoi(argv[2]);
   
    if(requests>N){
        printf("We cannot handle more requests than children!\n");
        exit(1);
    }
   
    childrenPids=calloc(sizeof(int),N);
    parentPid=getpid();
    
    struct sigaction oldAct;
    sigaction(SIGUSR1, NULL,&oldAct);
    struct sigaction newAct=oldAct;
    newAct.sa_flags=SA_SIGINFO;
    newAct.sa_sigaction=signalUserHandler;
    sigaction(SIGUSR1, &newAct,NULL);

    
    
    for (int i = SIGRTMIN;i<=SIGRTMAX;i++){
        
        sigaction(i, NULL,&oldAct);
        struct sigaction newrltAct=oldAct;
        newrltAct.sa_flags=SA_SIGINFO;
        newrltAct.sa_sigaction=realtimeSignalHandler;
        sigaction(i, &newrltAct,NULL);
    
    }
   
    signal(SIGINT, sigintHandler);
   
    for(int currentChild=0;currentChild<N;currentChild++){
        if(fork()==0){//child
            
            printf("created process : %d\n", getpid());
            childNumber=currentChild;
            srand(time(NULL) ^ (getpid()<<16));
            int sleepingTime = rand()%11;
            sleep(sleepingTime);//will sleep about 0-10 sec
            
            time_t timeStart = time(NULL);
            kill(getppid(),SIGUSR1);
            for(int i=0;i<20;i++){//another children wait for max 20 sec
                sleep(1);
                if(childPermission==1){
                    break;
                }
            }
            if(childPermission!=1){
                printf("child %d : process %d : waited for %d sec and didn't get a permission\n",currentChild,getpid(),(int)(time(NULL)-timeStart));
                exit(1);
            }
            // let's pick on of 32 realtime signals
            int rltsig = (int)(time(NULL)-timeStart);
            //send signal
            printf("child %d : process %d : sended realtime signal\"%d\"\n",        currentChild,getpid(),rltsig);
            kill(getppid(),(SIGRTMIN+(rand()%(SIGRTMAX-SIGRTMIN))));
            
            
            printf("child %d : process %d : waited for %d sec, sleeped for %d\n",currentChild,getpid(),rltsig,sleepingTime);
            exit(0);
        }
    }
    
    //need to wait for all children
    int numberOfChildren=0;
    int secondsOfWaiting = 45;//can afford waiting max for 45 sec
    while(secondsOfWaiting>0 && numberOfChildren<N){
        if(wait(NULL)!=-1){//wchodzimy tu gdy jakis potomek skonczyl
            numberOfChildren++;
        }
        else{
            secondsOfWaiting--;
            sleep(1);
        }
    }
    if(numberOfChildren!=N){
        printf("Some of the children didn't proceeded! I'm sending sigint to me in order to end every child\n");
        raise(SIGINT);
        exit(1);
    }
    return 0;
}
