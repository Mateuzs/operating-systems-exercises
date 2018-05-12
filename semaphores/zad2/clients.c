//
//  clients.c
//  lab7
//
//  Created by Mateusz Zembol on 10.05.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "clients.h"
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


int shmSize;
sem_t *semTab[3];
pid_t *shmTab;
int actualCuts = 0;



// SIGNAL HANDLERS //

void sigTermHandler(int signum){
    printf("\nsigterm\n");
    exit(0);
}


// ADDITIONAL FUNCTIONS  FIFO//

int fifo_is_empty(pid_t *shmTab) {
    return shmTab[1] == 0 ? 1 : 0;
}

int fifo_is_full(pid_t *tab) {
    return tab[0]-4 == tab[1] ? 1 : 0;
}


int fifo_push(pid_t *tab, pid_t client_pid) {
    if(fifo_is_full(tab)) return -1;
    
    int i = ((tab[1] + tab[2]) % tab[0]) + 4 * ((tab[1]+tab[2]) / tab[0]); // guard of the index
    
    tab[i] = client_pid;
    ++tab[1]; // incrementing actual clients
    return 0;
}

pid_t fifo_pop(pid_t *tab) {
    if(fifo_is_empty(tab)) return -1;
    
    int i = tab[2];
    tab[2] = (tab[2] + 1 + 4 * ((tab[2] + 1) / tab[0])) % tab[0];
    
    --tab[1];
    return tab[i];
}


// UTILITIES

long getTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1){
        printf("problem with time\n");
    }
    return time.tv_nsec/1000;
}


void exit_program(int status, char *message) {
    status == EXIT_SUCCESS ? printf("%s\n",message) : perror(message);
    exit(status);
}

void clean(){
    
    if(sem_close(semTab[BARBER]) == -1 ||
       sem_close(semTab[FIFO]) == -1 ||
       sem_close(semTab[CUT]) == -1 ) {
        
        exit_program(EXIT_FAILURE, "Error occured while closing semaphores.");
    }
    
    if(munmap(shmTab,shmSize*sizeof(int)) == -1) {
        exit_program(EXIT_FAILURE,"Error occured while detaching from shared memory");
    }
    
    
}

// IMPORTANT ADDITIONAL FUNCTIONS //

int try_to_seat() {
    
    int status;
    sem_wait(semTab[FIFO]);
    int barber_semval;
    sem_getvalue(semTab[BARBER],&barber_semval);
    
    if(barber_semval == -1) {
        exit_program(EXIT_FAILURE, "Barber semaphore checking error");
    }
    
    if(barber_semval == 0) {    // if barber is sleeping
        printf("TIME: %ld, %d CLIENT: Barber is sleeping. Let's wake him up.\n", getTime(), getpid());
        
        sem_post(semTab[BARBER]); // let's wake him up
        sem_post(semTab[BARBER]); //  (indicates that barber is working
        
        shmTab[3]=getpid(); // sit on chair
        printf("TIME: %ld, %d CLIENT: Takes the seat in order to cut!.\n", getTime(), getpid());
        status = 0;
        
    }else{ // if barber performing his art, try to seat in the waiting room
        if(fifo_push(shmTab,getpid()) == -1) { // if the waiting room is full
            printf("TIME: %ld, %d CLIENT: Barber is full. I'm leaving.\n", getTime(), getpid());
            status = -1;
        } else {
            printf("TIME: %ld, %d CLIENT: Barber is busy. Taking seat in waiting room. \n", getTime(), getpid());
            status = 0;
        }
    }
    
    sem_post(semTab[FIFO]); // give the semaphore to the others
    return status;
}



int visit_barber(int S) {
    
    while(actualCuts < S) {
        
        int result = try_to_seat();
        
        if(result == 0) {
            sem_wait(semTab[CUT]);
            ++actualCuts;
            printf("TIME: %ld, %d CLIENT: Pays and leaves!.\n", getTime(), getpid());
        }
    }
    
    clean();
    return 0;
}



void init() {
    
    semTab[BARBER] = sem_open(BARBER_STR, O_RDWR, 0600, 0);
    semTab[FIFO] = sem_open(FIFO_STR, O_RDWR, 0600, 0);
    semTab[CUT] = sem_open(CUT_STR, O_RDWR, 0600, 0);
    
    if(semTab[BARBER] == SEM_FAILED || semTab[FIFO] == SEM_FAILED || semTab[CUT] == SEM_FAILED) {
        exit_program(EXIT_FAILURE, "Error occured during opening semaphores");
    }
    
    int shm_id = shm_open(MEMORY_STR, O_RDWR, 0600);
    if(shm_id == -1) {
        exit_program(EXIT_FAILURE, "Error occured during creating shared memory");
    }
    
    shmTab = mmap(NULL, sizeof(int), PROT_READ, MAP_SHARED, shm_id, 0);
    if(shmTab == (void*)-1) {
        exit_program(EXIT_FAILURE, "Error occured during getting address of shared memory");
    }
    
    shmSize = shmTab[0]; // getting the size of the shared memory
    
    if(munmap(shmTab,sizeof(int)) == -1) { // detaching
        exit_program(EXIT_FAILURE,"Error occured during detaching from the shared memory");
    }
    
    shmTab = mmap(NULL, shmSize*sizeof(int), PROT_WRITE | PROT_READ, MAP_SHARED, shm_id, 0); // attaching with correct size
    
    if(shmTab == (void*)-1) {
        exit_program(EXIT_FAILURE, "Error occured during getting address of shared memory");
    }

}






int main(int argc, char **argv) {
    // N - number of clients to create and S - number of required cuts
    
    if(argc != 3) {
        exit_program(EXIT_FAILURE, "Bad number of arguments! Pass the number of clients to create and the number of required cuts!");
    }
    
    int N = atoi(argv[1]), S = atoi(argv[2]);
    
    signal(SIGTERM,sigTermHandler);
    
    init();
    
    for(int i = 0; i < N; i++) {
        if(fork() == 0) {
            visit_barber(S) == -1 ? _exit(EXIT_FAILURE) : _exit(EXIT_SUCCESS);
        }
    }
    
    int clients_served = 0;
    int status;
    while(clients_served < N) {
        wait(&status);
        ++clients_served;
        if(status == EXIT_SUCCESS) ++clients_served;
        else exit_program(EXIT_SUCCESS, "Child failed to execute properly"); // it's not a failure of the parent process
    }
    
    return 0;
}
