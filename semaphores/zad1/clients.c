//
//  clients.c
//  lab7
//
//  Created by Mateusz Zembol on 30.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

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

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0


int semId;
int *shmTab;
int actualCuts = 0;



// SIGNAL HANDLERS //

void sigTermHandler(int signum){
    printf("\nsigterm\n");
    exit(0);
}


// ADDITIONAL FUNCTIONS //

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


void give_semaphore(int semId, unsigned short semIndex) {
    struct sembuf smbuf;
    smbuf.sem_num = semIndex;
    smbuf.sem_op = 1;
    smbuf.sem_flg = 0;
    if (semop(semId, &smbuf, 1) == -1){
        printf("error on giving semaphore\n");
        exit(1);
    }
}

void take_semaphore(int semId, unsigned short semIndex) {
    struct sembuf smbuf;
    smbuf.sem_num = semIndex;
    smbuf.sem_op = -1;
    smbuf.sem_flg = 0;
    if (semop(semId, &smbuf, 1) == -1){
        printf("error on taking semaphore\n");
        exit(1);
    }
}

// IMPORTANT ADDITIONAL FUNCTIONS //

int try_to_seat() {
    
    int status;
    take_semaphore(semId,FIFO);
    int barber_semval = semctl(semId,BARBER,GETVAL);
    
    if(barber_semval == -1) {
        exit_program(EXIT_FAILURE, "Barber semaphore checking error");
    }
    
    if(barber_semval == 0) {    // if barber is sleeping
        printf("TIME: %ld, %d CLIENT: Barber is sleeping. Let's wake him up.\n", getTime(), getpid());
        give_semaphore(semId,BARBER); // let's wake him up
        give_semaphore(semId,BARBER); //  (indicates that barber is working
        shmTab[3]=getpid(); // sit on chair
        printf("TIME: %ld, %d CLIENT: Takes the seat in order to cut!.\n", getTime(), getpid());
        status = 0;
        
     }else{ // if barber doing something try to seat in the waiting room
        if(fifo_push(shmTab,getpid()) == -1) { // if the queue is full
            printf("TIME: %ld, %d CLIENT: Barber is full. I'm leaving.\n", getTime(), getpid());
            status = -1;
        } else {
            printf("TIME: %ld, %d CLIENT: Barber is busy. Taking seat in waiting room. \n", getTime(), getpid());
            status = 0;
        }
    }
    
    give_semaphore(semId,FIFO); // give the semaphore to someone else
    return status;
}



int visit_barber(int S) {
    
    while(actualCuts < S) {
        
        int result = try_to_seat();

        if(result == 0) {
            take_semaphore(semId,CUT);
            ++actualCuts;
            printf("TIME: %ld, %d CLIENT: Pays and leaves!.\n", getTime(), getpid());
        }
    }
    if(shmdt(shmTab) == -1)
        exit_program(EXIT_FAILURE,"Error while detaching from shared memory");
    return 0;
}



void init() {
    
    key_t key = ftok("./barber.c",'k');
    int shm_id = shmget(key,0,0);
    if(shm_id == -1)
        exit_program(EXIT_FAILURE, "Error while getting shared memory");
    
    shmTab = shmat(shm_id,NULL,0);
    if(shmTab == (int *) -1)
        exit_program(EXIT_FAILURE, "Error while getting address of shared memory");
    
    semId = semget(key,0,0);
    if(semId == -1)
        exit_program(EXIT_FAILURE,"Error while getting semaphores");
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
        if(fork() == 0) { // don't let children create their own children
            visit_barber(S) == -1 ? _exit(EXIT_FAILURE) : _exit(EXIT_SUCCESS);
        }
    }
    
    int clients_served = 0;
    int status;
    while(clients_served < N) {
        wait(&status);
        ++clients_served;
        if(status == EXIT_SUCCESS) ++clients_served;
        else exit_program(EXIT_SUCCESS, "Child failed to execute properly"); // success, because it's not fault of parent process
    }
    
    return 0;
}
