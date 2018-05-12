//
//  barber.c
//  lab7
//
//  Created by Mateusz Zembol on 10.05.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "barber.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <fcntl.h>


#define BARBER 0
#define FIFO 1
#define CUT 2

static const char* BARBER_STR = "BARBER";
static const char* FIFO_STR = "FIFO";
static const char* CUT_STR = "CUT";

static const char* MEMORY_STR = "MEMORY";

pid_t *shmTab;
sem_t* semTab[3];
int N;

// SIGNAL HANDLERS //

void sigTermHandler(int signum){
    printf("\n sigterm \n");
    exit(0);
}

void sigIntHandler(int signum){
    printf("\n sigint \n");
    exit(0);
}

// ADDITIONAL FUNCTIONS  FIFO//
//tab[0] = queue's length
//tab[1] = current number of clients
//tab[2] = current index of table
//tab[3] = barber's chair

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


// UTILITY STUFF

long getTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1){
        printf("we've got problem with time\n");
    }
    return time.tv_nsec/1000;
}



void make_cut(int client_pid) {
    printf("TIME: %ld,  CLIENT: %d: Start haircutting!\n", getTime(), client_pid);
    
    sem_post(semTab[CUT]);

    printf("TIME: %ld,  CLIENT: %d: End haircutting!\n", getTime(), client_pid);
    
}


void clean(){
    
    if (munmap(shmTab, (N+4)*sizeof(int)) == -1) {
        perror("Error occured during detaching shared memory");
    }
    if(shm_unlink(MEMORY_STR) == -1) {
        perror("Error occured during deleting memory segment");
    }
    if(sem_unlink(BARBER_STR) == -1 || sem_unlink(FIFO_STR) == -1 || sem_unlink(CUT_STR) == -1) {
        perror("Error occured during deleting semaphores");
    }

    
     printf("\nReleased the resources,"
            "\ncleaned the stuff,\n"
            "and finally barber closed his workshop.\n");
}


int main(int argc, char** argv){
    
    if(argc != 2){
        printf("Give the number of chairs!\n");
        return 1;
    }
    
    
    N = atoi(argv[1]); // number of chairs!
    
    // EXIT HANDLERS
    atexit(clean);
    signal(SIGTERM,sigTermHandler);
    signal(SIGINT,sigIntHandler);

    
    // OPEN SEMAPHORES
    semTab[BARBER] = sem_open(BARBER_STR, O_CREAT | O_RDWR | O_EXCL, 0600, 0);
    semTab[FIFO] = sem_open(FIFO_STR, O_CREAT | O_RDWR | O_EXCL, 0600, 1);
    semTab[CUT] = sem_open(CUT_STR, O_CREAT | O_RDWR | O_EXCL, 0600, 0);
    
    
    if(semTab[BARBER] == SEM_FAILED || semTab[FIFO] == SEM_FAILED || semTab[CUT] == SEM_FAILED) {
        perror( "Semaphore creating error");
    }
    
    // CREATE MEMORY SEGMENT
    
    int shm_id = shm_open(MEMORY_STR, O_CREAT | O_RDWR | O_EXCL, 0600);
    
    if(shm_id == -1) {
        perror("Error occured during creating shared memory");
    }
    
    if(ftruncate(shm_id, (N+4)*sizeof(pid_t)) == -1) {
        perror("Error occured during truncating shared memory");
    }
    
    shmTab = mmap(NULL, (N+4)*sizeof(pid_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
    
    if(shmTab == (void*)-1) {
        perror("Error occured during getting address of shared memory");
    }
    
    
    // CONFIGURE INIT VALUES OF OUR QUEUE, FIRST FOUR CELLS ARE RESERVED
    shmTab[0] = N + 4; //queue length
    shmTab[1] = 0;  //how many clients
    shmTab[2] = 4; // index
    
    
    int client;
    
    printf("TIME: %ld, BARBER: %d : I go to sleep...\n", getTime(), getpid());
    
    while(1){
        
        sem_wait(semTab[BARBER]); // sleep
        sem_wait(semTab[FIFO]); // waiting for fifo in order to take client
        
        
        printf("TIME: %ld : Someone awakes me!\n", getTime());
        client = shmTab[3];//first client, or waking client, whatever
        printf("TIME: %ld : Take a seat, dear client number: %d!\n", getTime(), client);

        
        sem_post(semTab[FIFO]);
        make_cut(client);
        
        while(1){
            sem_wait(semTab[FIFO]);
            client = fifo_pop(shmTab);
            if(client != -1){
                printf("TIME: %ld : Take a seat, dear client number: %d!\n", getTime(), client);
                make_cut(client);
                sem_post(semTab[FIFO]);
            }else{
                printf("TIME: %ld, BARBER: %d : I go to sleep...\n", getTime(), getpid());
                sem_wait(semTab[BARBER]);
                sem_post(semTab[FIFO]);
                break;
            }
        }
    }
    
    return 0;
}
