//
//  main.c
//  lab4
//
//  Created by Mateusz Zembol on 10.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <memory.h>


volatile int L; // number of signals
volatile int ARG; // type of action <- <1,3>
volatile int sentToChild = 0;
volatile int receivedByChild = 0;
volatile int receivedFromChild = 0;
volatile pid_t child;




void printStats() {
    printf("Signals sent: %d\n", sentToChild);
    printf("Signals received from child: %d\n", receivedFromChild);
}


void childHandler(int signal, siginfo_t *info, void *context) {
   
    if (signal == SIGINT) {
        sigset_t mask;
        sigfillset(&mask);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        printf("Child received SIGINT: Signals received by child: %d\n", receivedByChild);
        exit((unsigned) receivedByChild);
    }
    
    
    if (info->si_pid != getppid()) return;
    
    if (ARG == 1 || ARG == 2) {
        if (signal == SIGUSR1) {
            receivedByChild++;
            kill(getppid(), SIGUSR1);
            printf("Child: SIGUSR1 received and sent back\n");
        } else if (signal == SIGUSR2) {
            receivedByChild++;
            printf("Child: SIGUSR2 received Terminating\n");
            printf("Signals received by child: %d\n", receivedByChild);
            exit((unsigned) receivedByChild);
        }
    } else if (ARG == 3) {
        if (signal == SIGRTMIN) {
            receivedByChild++;
            kill(getppid(), SIGRTMIN);
            printf("Child: SIGRTMIN received and sent back\n");
        } else if (signal == SIGRTMIN + 1) {
            receivedByChild++;
            printf("Child: SIGRTMIN+1 received Terminating\n");
            printf("Signals received by child: %d\n", receivedByChild);
            exit((unsigned) receivedByChild);
        }
    
    }
    else printf("trolololo\n\n");
}


void parentHandler(int signal, siginfo_t *info, void *context) {
    if (signal == SIGINT) {
        printf("Parent: Received SIGINT\n");
        kill(child, SIGINT);
        printStats();
        exit(0);
    }
    if (info->si_pid != child) return;
    
    if ((ARG == 1 || ARG == 2) && signal == SIGUSR1) {
        receivedFromChild++;
        printf("Parent: Received SIGUSR1 form Child\n");
    } else if (ARG == 3 && signal == SIGRTMIN) {
        receivedFromChild++;
        printf("Parent: Received SIGRTMIN from Child\n");
    }
}

void childProcess() {
    
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = childHandler;
    
    if (sigaction(SIGUSR1, &act, NULL) == -1) printf("Can't catch SIGUSR1\n");
    if (sigaction(SIGUSR2, &act, NULL) == -1) printf("Can't catch SIGUSR2\n");
    if (sigaction(SIGINT, &act, NULL) == -1) printf("Can't catch SIGINT\n");
    if (sigaction(SIGRTMIN, &act, NULL) == -1) printf("Can't catch SIGRTMIN\n");
    if (sigaction(SIGRTMIN+1, &act, NULL) == -1) printf("Can't catch SIGRTMIN+1\n");

    
   
    while (1) {
        sleep(1);
    }
}


void parentProcess() {
    sleep(1);
    
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = parentHandler;
    
    if (sigaction(SIGUSR1, &act, NULL) == -1) printf("Can't catch SIGUSR1\n");
    if (sigaction(SIGUSR2, &act, NULL) == -1) printf("Can't catch SIGUSR2\n");
    if (sigaction(SIGINT, &act, NULL) == -1) printf("Can't catch SIGINT\n");
    if (sigaction(SIGRTMIN, &act, NULL) == -1) printf("Can't catch SIGRTMIN\n");
    if (sigaction(SIGRTMIN+1, &act, NULL) == -1) printf("Can't catch SIGRTMIN+1\n");

    

    if (ARG == 1 || ARG == 2) {
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGINT);
        for (; sentToChild < L; sentToChild++) {
            printf("Parent: Sending SIGUSR1\n");
            kill(child, SIGUSR1);
            if (ARG == 2) sigsuspend(&mask);
        }
        printf("Parent: Sending SIGUSR2\n");
        kill(child, SIGUSR2);
    } else if (ARG == 3) {
        for (; sentToChild < L; sentToChild++) {
            printf("Parent: Sending SIGRTMIN\n");
            kill(child, SIGRTMIN);
        }
        sentToChild++;
        printf("Parent: Sending SIGRTMIN+1\n");
        kill(child, SIGRTMIN+1);
    }
    
    int status = 0;
    waitpid(child, &status, 0);
    if (WIFEXITED(status))
        receivedByChild = WEXITSTATUS(status);
    else{
        printf("Error with termination of Child!\n");
        return;
    }
}


int main(int argc, char *argv[]) {
    
    if (argc < 3){
        printf("Wrong number of arguments!\n");
        return 1;
    }
    
    L = atoi(argv[1]);
    ARG = atoi(argv[2]);
    
    if (L < 1){
        printf("wrong number of signals\n");
        return 1;
    }
    if (ARG < 1 || ARG > 3){
        printf("Wrong number of action!\n");
        return 1;
    }
    
    child = fork();
    if (!child) childProcess();
    else if (child > 0) parentProcess();
    else{
        printf("Error while Forking\n");
        return 1;
    }
    
    printStats();
    
    return 0;
}

