//
//  main.c
//  lab5
//
//  Created by Mateusz Zembol on 15.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <signal.h>
#include <sys/stat.h>

static void intAction(int sigNum, siginfo_t* info, void* vp){
    killpg(0, SIGINT); // KILLPG TO WYSYLAMY DO GRUPY PROCESOW
    exit(EXIT_SUCCESS); // MAKRO POWIADAMIA ZE APLIKACJA ZAKONCZONA PRAWIDLOWO
}


int childPid;
int masterPid;
int childPids[4096];


int main(int argc, char *argv[]) {
    
    // we need to put name, number of children, number of writes
    int slaveNumber;
    int N;
    struct sigaction sigAction;
    
    if (argc != 4){
        printf("Wrong main arguments!\n");
        exit(EXIT_FAILURE);
    }
    
    sigfillset(&sigAction.sa_mask);
    sigAction.sa_flags = SA_SIGINFO;
    sigAction.sa_sigaction = &intAction;
    sigaction(SIGINT, &sigAction, NULL);
    
    mkfifo(argv[1], 0777);
    
    slaveNumber = (int) strtol(argv[2], NULL, 10);
    N = (int) strtol(argv[3], NULL, 10);
    
    for (int i = 0; i < slaveNumber; ++i) {
        childPids[i] = fork();
        if (childPids[i] == 0){
            execlp("./slave","slave", argv[1], argv[3], 0);
        }
    }
    
    masterPid = fork();
    if (masterPid == 0){
        execlp("./master", "master", argv[1], 0);
    }
    
    for (int i = 0; i < slaveNumber; ++i) {
        waitpid(childPids[i], NULL, WUNTRACED); // czekamy na zmiane stanu - tutaj czy zakonczyl
        printf("%d'th slave exited\n", i);
    }
    
    kill(masterPid, SIGINT);
    killpg(0, SIGINT);
    
    waitpid(masterPid, NULL, WUNTRACED);
    
    return 0;
}
