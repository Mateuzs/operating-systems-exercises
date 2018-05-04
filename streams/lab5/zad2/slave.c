
//
//  slave.c
//  lab5
//
//  Created by Mateusz Zembol on 15.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//
#define _BSD_SOURCE
#include "slave.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>

#define LINE_MAX 256

int main(int argc, char** argv) {
    srand(time(NULL));
    if (argc < 3) { // sciezka potoku nazwanego i liczba N
        printf("wrong number of arguments!");
        return 1;
    }
    
    int pipe = open(argv[1], O_WRONLY); // otwieramy plik reprezentujacy potok nazwany
    
    char buffer1[LINE_MAX];
    char buffer2[LINE_MAX];
    
    int it = (int) strtol(argv[2], NULL, 10);
    
    for (int i = 0; i < it; i++) {
        FILE *date = popen("date", "r");
        fgets(buffer1, LINE_MAX, date);
        int pid = getpid();
        sprintf(buffer2, "Slave: %d - %s", pid, buffer1);
        write(pipe, buffer2, strlen(buffer2));
        sleep((rand() % 3) + 2);
    }
    close(pipe);
    return 0;
}
