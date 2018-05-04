//
//  server.c
//  lab6
//
//  Created by Mateusz Zembol on 22.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include "messages.h"

struct Clients{
    int pid;
    int qid;
};

int queueId=-1;


//-----ADDITIONAL FUNCTIONS-----------

char * mirror( char * sentence){
    
    unsigned long length = strlen(sentence);
    char *newSentence = (char*)malloc(length * (sizeof(char)));
    int i = 0;
    
    for(; i<length; i++){
        newSentence[i] = sentence[length - i - 1];
    }
    
    return newSentence;
}

char * calc( char * sentence){
    
    double a = atoi(&sentence[0]);
    double b = atoi(&sentence[2]);
    double c = 0.0;
    char * newSentence = (char*)malloc(maxOrderValueLen * (sizeof(char)));;
    
    
    
    if(sentence[1] == '+') c = a + b;
    else if(sentence[1] == '-') c = a - b;
    else if(sentence[1] == '*') c = a * b;
    else if(sentence[1] == '/') c = a / b;
    else return "Error!";
    
    snprintf(newSentence, maxOrderValueLen, "%f", c);
    return newSentence;
    
}

//------- QUEUE HANDLERS ---------------


void closeQueue(){
    if(queueId!=-1){
        printf("process %d deletes queue %d\n",getpid(),queueId);
        msgctl(queueId, IPC_RMID, NULL);//usuwa kolejke
        queueId=-1;
        exit(0);
    }
}


void closeClientQueue(int clientQueueId, int Cpid){
    
    printf("server deletes queue %d of process %d\n",clientQueueId, Cpid);
        msgctl(clientQueueId, IPC_RMID, NULL);//usuwa kolejke
    
}

//----------- MAIN -------------

int main(int argc,char *argv[], char *envp[]){
    
    int endASAPflag = 0;
    
    atexit(closeQueue);
    signal(SIGINT, closeQueue);
    
    const char* home = getenv("HOME");
    
    struct Clients clients[maxClients];
    int i;
    for(i = 0 ;i<maxClients;i++ ) clients[i].qid = -1;
    
    key_t key = ftok(home,keyGen);
    if(key == -1){
        printf("error on ftok\n");
        exit(1);
    }
    

    queueId=msgget(key,IPC_CREAT | 0600);//queue with acces 2+4 read and write IPC_CREAT if doesn't exists
   
    struct Order order;
    int size = sizeof(struct Order)-sizeof(long);
    int tmpQid;
    int currentClient=-1;
   
    printf("server has started his work!\n");
    
    while(1){
        
        if(endASAPflag == 1){
            if(msgrcv(queueId,&order,size,0,IPC_NOWAIT)<0){
             
             printf("server has ended his work!\n");
             break;
                
            }
        }else{
                msgrcv(queueId,&order,size,0,0);
        }
        
        printf("request from process %d: ",order.pid);

        
        if(order.type==NEWCLIENT){
            
            printf("NEWCLIENT\n");
            for(i = 0 ; i<maxClients ; i++){
                if(clients[i].qid == -1){
                    currentClient = i;
                    break;
                }
            }
            if(currentClient>=maxClients){
                printf("too much clients!\n");
                exit(1);
                }
            
            
            clients[currentClient].pid=order.pid;
            clients[currentClient].qid=tmpQid=order.qid;

            order.qid=currentClient;
            msgsnd(tmpQid,&order,size,0);
            
        }else if(order.type==MIRROR){
            
            printf("MIRROR\n");
            tmpQid=clients[order.qid].qid;
            char * newValue = mirror(order.value);
            strcpy(order.value, newValue);
            order.pid=getpid();
            order.qid=queueId;
            msgsnd(tmpQid,&order,size,0);
   
        } else if(order.type==TIME){
            
            printf("TIME\n");

            tmpQid=clients[order.qid].qid;
            time_t rawtime;
            struct tm *info;
            time( &rawtime );
            info = localtime( &rawtime );
            strftime(order.value, maxOrderValueLen,"%x - %I:%M:%S%p", info);
            order.pid=getpid();
            order.qid=queueId;
            msgsnd(tmpQid,&order,size,0);
        
        } else if(order.type==CALC){
            
            printf("CALC\n");
            tmpQid=clients[order.qid].qid;
            char * newValue = calc(order.value);
            strcpy(order.value, newValue);
            order.pid=getpid();
            order.qid=queueId;
            msgsnd(tmpQid,&order,size,0);
            
        }else if(order.type==STOP){
            
            printf("STOP\n");
            
            tmpQid = clients[order.qid].qid;
            closeClientQueue(tmpQid, order.pid);
            clients[order.qid].qid = -1;
            
        }else if(order.type==END){
            
            printf("END\n");
            endASAPflag = 1;
        }
        
        sleep(1);
    }
    
    return 0;
}
