//
//  barber.c
//  lab7
//
//  Created by Mateusz Zembol on 30.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "barber.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>


#define BARBER 0
#define FIFO 1
#define CUT 2

int semId, shmId;
int *shmTab;

// SIGNAL HANDLERS //

void sigTermHandler(int signum){
    printf("\nsigterm\n");
    exit(0);
}

void sigIntHandler(int signum){
    printf("\nsigint\n");
    exit(0);
}


// ADDITIONAL FUNCTIONS  FIFO //

int fifo_is_empty(pid_t *shmTab) {
    return shmTab[1] == 0 ? 1 : 0;
}

pid_t fifo_pop(pid_t *shmTab) {
    if(fifo_is_empty(shmTab)) return -1;
    
    int index = shmTab[2];
    shmTab[2] = (shmTab[2] + 1 + 4 * ((shmTab[2] + 1) / shmTab[0])) % shmTab[0]; // guard of the index range
    
    --shmTab[1];
    return shmTab[index];
}


// UTILITY STUFF //

long getTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1){
        printf("we've got problem with time\n");
    }
    return time.tv_nsec/1000;
}

void give_semaphore(int semId, unsigned short semNumber) {
    struct sembuf smbuf;
    smbuf.sem_num = semNumber;
    smbuf.sem_op = 1;
    smbuf.sem_flg = 0;
    
    if (semop(semId, &smbuf, 1) == -1){
        printf("error occured during releasing sempahore\n");
        exit(1);
    }
}

void take_semaphore(int semId, unsigned short semNumber) {
    struct sembuf smbuf;
    smbuf.sem_num = semNumber;
    smbuf.sem_op = -1;
    smbuf.sem_flg = 0;
    
    if (semop(semId, &smbuf, 1) == -1){
        printf("error occured during taking semaphore\n");
        exit(1);
    }
}

void clean(){
    if (shmTab != NULL) {
        shmdt(shmTab);
    }
    if(shmId!=-1){
        shmctl(shmId,IPC_RMID,NULL);
    }
    if(semId != -1) {
        semctl(semId,0,IPC_RMID);
    }
    printf("\nReleased the resources,\n"
           "cleaned the stuff,\n"
           "and finally barber closed his workshop.\n");
}



void make_cut(int client_pid, int semId) {
    
    printf("TIME: %ld,  CLIENT: %d: Start haircutting!\n", getTime(), client_pid);
    give_semaphore(semId,CUT);
    printf("TIME: %ld,  CLIENT: %d: End haircutting!\n", getTime(), client_pid);

}


int main(int argc, char** argv){
    
    if(argc != 2){
        printf("Give the number of chairs!\n");
        return 1;
    }
    
    int N = atoi(argv[1]); // number of chairs!
    
    atexit(clean);
    signal(SIGTERM,sigTermHandler);
    key_t key = ftok("./barber.c",'k');
    
    
    // OPEN SET OF SEMAPHORES
    semId = semget(key, 4, IPC_CREAT | 0600);
    if(semId == -1) {
        printf("Error with semId\n");
    }
    
    // let's configure semaphores!
    
    if(semctl(semId,BARBER,SETVAL,0) != 0 ||
       semctl(semId,FIFO,SETVAL,1) != 0 ||
       semctl(semId,CUT,SETVAL,0) != 0 ){
        printf("Error while initializing the semaphores value\n");
    }
    
    // OPEN SHARED MEMORY
    
    shmId = shmget(key, (N+4)*sizeof(pid_t), IPC_CREAT | 0600);
    if(shmId == -1)    {
        printf("Error while creating shared memory\n");
        exit(1);
    }
    
    shmTab = shmat(shmId, NULL, 0);
    if(shmTab == (int *) -1) {
        printf("Error while getting address of shared memory\n");
        exit(1);
    }

    // let's configure shared memory 
    
    shmTab[0] = N+4; //length of the queue
    shmTab[1] = 0; // how many clients
    shmTab[2] = 4; // index
    
    
    
    
    int client;
    
    printf("TIME: %ld, BARBER: %d : I go to sleep...\n", getTime(), getpid());

    while(1){
        
        take_semaphore(semId,BARBER); // sleep
        take_semaphore(semId,FIFO); // waiting for fifo in order to take client
        
        
        printf("TIME: %ld : Someone awakes me!\n", getTime());
        client = shmTab[3];//first client
        printf("TIME: %ld : Take a seat, dear client number: %d!\n", getTime(), client);

        
        give_semaphore(semId,FIFO);
        make_cut(client, semId);
        
        while(1){
            take_semaphore(semId,FIFO);
            client = fifo_pop(shmTab);
            
            if(client != -1){
                printf("TIME: %ld : Take a seat, dear client number: %d!\n", getTime(), client);
                make_cut(client, semId);
                give_semaphore(semId,FIFO);
            }
            else{
                printf("TIME: %ld, BARBER: %d : I go to sleep...\n", getTime(), getpid());
                take_semaphore(semId,BARBER);
                give_semaphore(semId,FIFO);
                break;
            }
        }
    }
    
    return 0;
}
