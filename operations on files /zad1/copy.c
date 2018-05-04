//
//  copy.c
//  lab2
//
//  Created by Mateusz Zembol on 16.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "copy.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


void copy(char * file1_name, char *file2_name, int record_length){
    
    FILE* file1 = fopen(file1_name, "r");
   
    FILE* file2 = fopen(file2_name, "a+");
    
    char* record = (char*) calloc(record_length + 2, sizeof(char));

    
    fseek(file1, 0, SEEK_END);
    long int endOfFile = ftell(file1);
    fseek(file1, 0, SEEK_SET);
    
    
    while(ftell(file1) < endOfFile){

        fread(record, record_length + 2, 1, file1);
        fwrite(record, record_length + 2, 1, file2);
    }
    
    fclose(file1);
    fclose(file2);
}



void copy_sys(char * file1_name, char *file2_name, int record_length){
    
    int file1 = open(file1_name, O_RDWR);
    
    int file2 = open(file2_name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    
    char* record = (char*) calloc(record_length + 2, sizeof(char));
    
    
    lseek(file1, 0, SEEK_END);
    long int endOfFile = lseek(file1, 0, SEEK_CUR);
    lseek(file1, 0, SEEK_SET);
    
    
    while(lseek(file1, 0, SEEK_CUR) < endOfFile){
        
        read(file1, record,record_length+2);
        write(file2, record, record_length +2);
    }
    
    close(file1);
    close(file2);
}

