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
#include <mqueue.h>

#include "messages.h"

mqd_t clients[maxClients];
int clientsId[maxClients];
mqd_t queueId;
int currentClient;


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
        mq_close(queueId);
        int i = 0;
        for(i; i< maxClients; i++){
            if(clientsId[i] == 1)
            mq_close(clients[i]);
        }
        mq_unlink(serverName);
        queueId=-1;
        exit(0);
    }
}


void closeClientQueue(int clientQueueId, int Cpid){
    
    printf("server deletes queue %d of process %d\n",clientQueueId, Cpid);
    mq_close(clients[clientQueueId]);
    
}

//----------- MAIN -------------

int main(int argc,char *argv[], char *envp[]){
    
    int endASAPflag = 0;
    
    atexit(closeQueue);
    signal(SIGINT, closeQueue);
    
    struct Order order;
    int size = sizeof(struct Order);
    struct mq_attr attributes;
    
    attributes.mq_flags = 0;
    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = size;
    attributes.mq_curmsgs = 0;
    
    queueId=mq_open(serverName, O_CREAT | O_RDWR, 0644, &attributes);
    if(queueId==-1){
        printf("cannot open the queue\n");
        exit(1);
    }

    int i;
    for(i = 0 ;i<maxClients;i++ ) clientsId[i] = 0;
    
    mqd_t tmpQid;
   
    printf("server has started his work!\n");
  
    struct timespec timeout={time(NULL)+10,0};

    while(1){
        
        if(endASAPflag == 1){
            timeout.tv_sec=time(NULL);
            if( mq_timedreceive(queueId, (char*)&order, size, NULL,&timeout)<0){
             printf("server has ended his work!\n");
             break;
            }
            
        }else{
            mq_receive(queueId, (char*)&order, size, NULL);
        }
        
    
        if(order.type==NEWCLIENT){
            
            printf("NEWCLIENT\n");
            for(i = 0 ; i<maxClients ; i++){
                if(clientsId[i] == 0){
                    currentClient = i;
                    break;
                }
            }
            if(currentClient>=maxClients){
                printf("too much clients!\n");
                exit(1);
                }
            
            clientsId[currentClient] = 1;
            printf("Current client id given : %d\n", currentClient);
            
            clients[currentClient] = mq_open(order.value, O_RDWR, 0644, &attributes);

            order.givenId = tmpQid = currentClient;
            mq_send(clients[tmpQid],(char *)&order,size,NULL);
            
        }else if(order.type==MIRROR){
            
            printf("MIRROR\n");
            tmpQid=clients[order.givenId];
            char * newValue = mirror(order.value);
            strcpy(order.value, newValue);
            order.pid=getpid();
            mq_send(tmpQid,(char *)&order,size,NULL);

        } else if(order.type==TIME){
            
            printf("TIME\n");

            tmpQid=clients[order.givenId];
            time_t rawtime;
            struct tm *info;
            time( &rawtime );
            info = localtime( &rawtime );
            strftime(order.value, maxOrderValueLen,"%x - %I:%M:%S%p", info);
            order.pid=getpid();
            mq_send(tmpQid,(char *)&order,size,NULL);

        } else if(order.type==CALC){
            
            printf("CALC\n");
            tmpQid=clients[order.givenId];
            char * newValue = calc(order.value);
            strcpy(order.value, newValue);
            order.pid=getpid();
            mq_send(tmpQid,(char *)&order,size,NULL);

        }else if(order.type==STOP){
            
            printf("STOP\n");
           
            
            closeClientQueue(order.givenId, order.pid);
            clientsId[order.givenId] = 0;
            
        }else if(order.type==END){
            
            printf("END\n");
            endASAPflag = 1;
        }
        
        sleep(1);
    }
    
    return 0;
}
