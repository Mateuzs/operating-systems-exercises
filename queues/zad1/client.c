#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include "messages.h"

int queueId=-1;

void closeQueue(){
    if(queueId!=-1){
        printf("process %d ends his task, deletes queue %d\n",getpid(),queueId);
        msgctl(queueId, IPC_RMID, NULL);//usuwa kolejke
        queueId=-1;
        exit(0);
    }
}

int main(int argc,char *argv[], char *envp[]){
    
    //we close the queue in case of exit the program:
    atexit(closeQueue);
    signal(SIGINT, closeQueue);
    
    const char* home = getenv("HOME");

    key_t key = ftok( home ,getpid()%keyGen);
    if(key == -1){
        printf("error on self ftok\n");
        exit(1);
    }
    
    queueId=msgget(key, IPC_CREAT |IPC_PRIVATE | 0600);//creates IPC queue: access rights write + read

    key = ftok(home, keyGen);
    if(key == -1){
        printf("error on servers ftok\n");
        exit(1);
    }
    
    int serverId=msgget(key, 0600);//checking if the queue exists
    if(serverId ==-1){
        printf("Server is shutdown\n");
        exit(1);
    }
    
    struct Order order;
    int size = sizeof(struct Order)-sizeof(long);
    
    //we send our key to the server
    
    order.type=NEWCLIENT;
    order.qid=queueId;
    order.pid=getpid();
    
    msgsnd(serverId,&order,size,0);
    msgrcv(queueId,&order,size,0,0);
    
    
    int givenID = order.qid;
    printf("I've got %d id\n",givenID);

    
    while(1){
       
        
        printf("\n2 - MIRROR, 3 - CALC, 4 - TIME, 5 - END, 6 - STOP\n");
        printf("type a number: ");
        scanf("%ld", &order.type);
       
        if(order.type==END || order.type == STOP){
            
            order.qid = givenID;
            order.pid  = getpid();
            msgsnd(serverId,&order,size,0);
            break;
        }else if(order.type==MIRROR){
         
            printf("type a sentence: ");
            scanf("%s", order.value);
            
        }else if(order.type == CALC){
            
            printf("type arithmetic task: ");
            scanf("%s", order.value);
            if(strlen(order.value) != 3){
                printf("wrong sentence! Example: 2+3, 4*5\n");
                continue;
            }
        
        }
        
        
        order.qid = givenID;
        order.pid=getpid();
        msgsnd(serverId,&order,size,0);
        
        char timeout=1;
        for(int i=0;i<10;i++){
            if(msgrcv(queueId,&order,size,0,IPC_NOWAIT)<0){
                sleep(1);
            }
            else{
                timeout=0;
                break;
            }
        }
        if(timeout){
            printf("\ntimeout\n");
            exit(1);
        }
        printf("\n%s\n",order.value);
    }
    return 0;
}
