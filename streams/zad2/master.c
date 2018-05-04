//
//  master.c
//  lab5
//
//  Created by Mateusz Zembol on 15.04.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//
#define _BSD_SOURCE
#include "master.h"

#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define LINE_MAX 256

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("wrong number of arguments!");
        return 1;
    }
    mkfifo(argv[1], S_IWUSR | S_IRUSR); // tworzymy potok nazwany, flagi odczyt zapis
    char buffer[LINE_MAX]; // buforek
    
    FILE *pipe = fopen(argv[1], "r");
    while (fgets(buffer, LINE_MAX, pipe) != NULL) {
        write(1, buffer, strlen(buffer)); // 1 standard output, czyli jak printf
    }
    fclose(pipe);
    return 0;
}
