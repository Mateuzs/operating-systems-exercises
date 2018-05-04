

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include "messages.h"
mqd_t queueId=-1;
char qname[maxOrderValueLen];

void closeQueue(){
    if(queueId!=-1){
        printf("process %d ends his task, deletes queue %d\n",getpid(),queueId);
        mq_close(queueId);
        mq_unlink(qname);
        queueId=-1;
        exit(0);
    }
}

int main(int argc,char *argv[], char *envp[]){
    
    //we close the queue in case of exit the program:
    atexit(closeQueue);
    signal(SIGINT, closeQueue);
    
    const char* home = getenv("HOME");
    struct Order order;
    int size = sizeof(struct Order);

    struct mq_attr attributes;

    attributes.mq_flags = 0;
    attributes.mq_maxmsg = 10;
    attributes.mq_msgsize = size;
    attributes.mq_curmsgs = 0;
    
    
    mqd_t serverMq = mq_open(serverName, O_RDWR, 0644, &attributes);
    if(serverMq==-1){
        printf("cannot create server's queue\n");
        exit(1);
    }


    key_t key = ftok( home ,getpid()%keyGen);
    if(key == -1){
        printf("error on self ftok\n");
        exit(1);
    }
    
    
    qname[0]='/';
    snprintf(qname+sizeof(char), maxOrderValueLen-1, "%d", key);
    
    
    queueId =mq_open(qname, O_CREAT | O_RDWR, 0644, &attributes);
    if(serverMq == -1){
        printf("cannot create user's queue\n");
        exit(1);
    }

    
    //we send our key to the server
    
    order.type=NEWCLIENT;
    strcpy(order.value, qname);
    order.pid=getpid();
    
    mq_send(serverMq,(char *)&order,size, 0);
    
    struct timespec timeout={time(NULL)+10,0};
    if( mq_timedreceive(queueId, (char*)&order, size, NULL, &timeout)<0){
        printf("timeout\n");
        exit(1);
    }
    
    
    int givenID = order.givenId;
    printf("I got %d id\n",givenID);

    
    while(1){
       
        
        printf("\n2 - MIRROR, 3 - CALC, 4 - TIME, 5 - END, 6 - STOP\n");
        printf("type a number: ");
        scanf("%ld", &order.type);
       
        if(order.type==END || order.type == STOP){
            
            order.givenId = givenID;
            order.pid  = getpid();
            
            mq_send(serverMq,(char *)&order,size, 0);
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
        
        
        order.givenId = givenID;
        order.pid=getpid();
        mq_send(serverMq,(char *)&order,size, 0);

        timeout.tv_sec=time(NULL)+10;
        if(mq_timedreceive(queueId, (char*)&order, size, NULL, &timeout)<0){
            printf("timeout\n");
            exit(1);
        }

        printf("\n%s\n",order.value);
    }
    return 0;
}
