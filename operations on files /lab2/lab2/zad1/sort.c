//
//  sort.c
//  lab2
//
//  Created by Mateusz Zembol on 16.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>





int sort_insert(FILE* file, long int endOfFile, int length) {
    int swap = 0;
    
    char* record1 = (char*) calloc(length, sizeof(char));
    char* record2 = (char*) calloc(length, sizeof(char));
    
    if(ftell(file) < endOfFile) {
        fread(record1, length, 1, file);
    }
    
    while(ftell(file) < endOfFile) {
        fread(record2, length, 1, file);
        
        if(record1[0] > record2[0]) {
            fseek(file, -2*length, SEEK_CUR);
            
            fwrite(record2, length, 1, file);
            fwrite(record1, length, 1, file);
            
            swap = 1;
        } else {
            char* tmp = record1;
            record1 = record2;
            record2 = tmp;
        }
        
        
    }
    
    return swap;
}



void sort(char * file_name, int record_length) {
    
    FILE* file = fopen(file_name, "r+");
    
    // calculate file end position
    fseek(file, 0, SEEK_END);
    long int endOfFile = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    int swapped = 1;
    
    while(swapped) {
        fseek(file, 0, SEEK_SET);
        swapped = sort_insert(file, endOfFile, record_length + 2);
    }
    
    fclose(file);
    
}



int sort_insert_sys(int file, long int endOfFile, int length) {
   
    int swap = 0;
    
    char* record1 = (char*) calloc(length, sizeof(char));
    char* record2 = (char*) calloc(length, sizeof(char));
    
    if(lseek(file, 0, SEEK_CUR) < endOfFile) {
        read(file, record1, length);
    }
    
    while( lseek(file, 0, SEEK_CUR) < endOfFile ) {
        read(file, record2, length);
        
        
        if(record1[0] > record2[0]) {
            lseek(file, -2*length, SEEK_CUR);
            
            write(file, record2, length);
            write(file, record1, length);

            swap = 1;
        } else {
            char* tmp = record1;
            record1 = record2;
            record2 = tmp;
        }
        
        
    }
    
    return swap;
}



void sort_sys(char * file_name, int record_length) {
    
    int file = open(file_name, O_RDWR);
    
    // calculate file end position
    lseek(file, 0, SEEK_END);
    long int endOfFile = lseek(file, 0, SEEK_CUR);
    lseek(file, 0, SEEK_SET);

    int swapped = 1;
    
    while(swapped) {
       

        lseek(file, 0, SEEK_SET);
        swapped = sort_insert_sys(file, endOfFile, record_length + 2);
    }
    
    close(file);
    
}


