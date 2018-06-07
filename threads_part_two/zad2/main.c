//
//  main.c
//  threads_part_two
//
//  Created by Mateusz Zembol on 05.06.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>

char **buffor;

sem_t *semaphore;

int P, K, N, L, nk, search_mode, verbose_mode;

FILE *file;
char file_name[FILENAME_MAX];

int production_index = 0, consumption_index = 0;

pthread_t *producer_threads;
pthread_t *consumer_threads;
int end = 0;




// ------------ AUXILIARY FUNCTIONS ------------- //

int length_search(int line_length){
    return search_mode == (line_length > L ? 1 : line_length < L ? -1 : 0);
}

void signal_handler(int signo) {
    fprintf(stderr, "Received signal %s, let's cancels all threads!\n", signo == SIGALRM ? "SIGALRM" : "SIGINT");
    for (int p = 0; p < P; p++)
        pthread_cancel(producer_threads[p]);
    for (int k = 0; k < K; k++)
        pthread_cancel(consumer_threads[k]);
    exit(EXIT_SUCCESS);
}


// -------------------- MAIN FUNCTIONS ------------------- //

void prepare_resources() {
    
    signal(SIGINT, signal_handler);
    if (nk > 0) signal(SIGALRM, signal_handler);
    
    if ((file = fopen(file_name, "r")) == NULL) {
        printf("\nError occured while opening the source file :(\n");
        exit(EXIT_FAILURE);
    }
    
    buffor = calloc((size_t) N, sizeof(char *));
    semaphore = malloc((N + 2) * sizeof(sem_t));
    
    for (int i = 0; i < N + 2; ++i)
        sem_init(&semaphore[i], 0, 1); // TO ZERO OZNACZA ZE W OBREBIE JEDNEGO PROCESU DZIELONE W WATKACH, 1 TO WARTOSC USTAWIONA
    
    
    producer_threads = malloc(P * sizeof(pthread_t));
    consumer_threads = malloc(K * sizeof(pthread_t));
}


// -------------- THREAD FUNCTIONS ------------ //

void *producer(void *pVoid) {
    
    int index;
    char line[LINE_MAX];
    
    
    while (fgets(line, LINE_MAX, file) != NULL) {
        
        if(verbose_mode) fprintf(stderr, "Producer id -> %ld: takes file line\n", (long)pthread_self());
        
        sem_wait(&semaphore[N]);
        
        while (buffor[production_index] != NULL) {} // JUST WAIT
        
        
        index = production_index;
        if(verbose_mode) fprintf(stderr, "Producer id -> %ld: taking buffor  index -> (%d)\n",  (long)pthread_self(), index);
        
        production_index = (production_index + 1) % N;
        
        
        sem_wait(&semaphore[index]);
        
        buffor[index] = malloc((strlen(line) + 1) * sizeof(char));
        strcpy(buffor[index], line);
        
        if(verbose_mode) fprintf(stderr, "Producer id -> %ld: line copied to buffer at index  -> %d\n",  (long)pthread_self(), index);
        
       
        sem_post(&semaphore[index]);
        sem_post(&semaphore[N]);

    }
    
    if(verbose_mode) fprintf(stderr, "Producer id -> %ld: has ended his job :)\n", (long)pthread_self());
    return NULL;
}

void *consumer(void *pVoid) {
    char *line;
    int index;
    
    while (1) {
        
        sem_wait(&semaphore[N + 1]);
        
        while (buffor[consumption_index] == NULL) {
            
            if (end) {
                
                sem_post(&semaphore[N + 1]);
                if(verbose_mode) fprintf(stderr, "Consumer id -> %ld: has ended his job  :)\n",  (long)pthread_self());
                return NULL;
            }

        }
        
        
        index = consumption_index;
        
        if(verbose_mode) fprintf(stderr, "Consumer id -> %ld: taking buffer index -> %d\n",  (long)pthread_self(), index);
        
        consumption_index = (consumption_index + 1) % N;
        
        sem_wait(&semaphore[index]);
        
        line = buffor[index];
        free(buffor[index]);
        buffor[index] = NULL;
        
        if(verbose_mode) fprintf(stderr, "Consumer id -> %ld: taking line from buffer at index ->  %d\n",  (long)pthread_self(), index);
        
        sem_post(&semaphore[index]);
        sem_post(&semaphore[N + 1]);

        
        if(length_search((int) strlen(line))){
            if(verbose_mode) fprintf(stderr, "Consumer id -> %ld: found line with length ->  %d,  search mode -> %c, defined length ->  %d\n", (long)pthread_self(), (int) strlen(line), search_mode == 1 ? '>' : search_mode == -1 ? '<' : '=', L);
            fprintf(stderr, "Consumer id -> %ld: Buffor Index -> %d, value -> %s",  (long)pthread_self(), index, line);
        }
        
        free(line);
        
    }
}


// ------- THREADS ------ //

void start_threads() {
    for (int p = 0; p < P; ++p)
        pthread_create(&producer_threads[p], NULL, producer, NULL);
    for (int k = 0; k < K; ++k)
        pthread_create(&consumer_threads[k], NULL, consumer, NULL);
    
    if (nk > 0) alarm(nk);
}


void join_threads(){
    for (int p = 0; p < P; ++p)
        pthread_join(producer_threads[p], NULL);
    
    end = 1;
    
    for (int k = 0; k < K; ++k)
        pthread_join(consumer_threads[k], NULL);
}


// ------ CLEANUP -------------- //

void free_resources() {
    if (file) fclose(file);
    
    for (int i = 0; i < N; i++)
        if (buffor[i]) free(buffor[i]);
    
    free(buffor);
    
    for (int i = 0; i < N + 3; i++)
        sem_destroy(&semaphore[i]);
    
    free(semaphore);
    
   
}

//--------- MAIN ---------------- //

int main(int argc, char **argv) {
    
    if (argc < 2){
        printf("\nDefine P, K, N, Filename, L, search mode, write mode, nk\n");
        exit(EXIT_FAILURE);
    }
    
    FILE *inputConfig;
    
    if ((inputConfig = fopen(argv[1], "r")) == NULL) {
        printf("\nError occured while opening the configuring file :(\n");
        exit(EXIT_FAILURE);
    }
    
    fscanf(inputConfig, "%d %d %d %s %d %d %d %d", &P, &K, &N, file_name, &L, &search_mode, &verbose_mode, &nk);
    
    printf("CONFIGURATION\nP: %d\nK: %d\nN: %d\nFile Name: %s\nL: %d\nSearch Mode: %d\nVerbose: %d\nnk: %d\n", P, K, N,file_name, L, search_mode, verbose_mode, nk);
    
    
    fclose(inputConfig);
   
    
    
    prepare_resources();
    
    start_threads();

    join_threads();
    
    free_resources();
    
    return 0;
}
