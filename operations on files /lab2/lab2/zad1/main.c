//
//  main.c
//  lab2
//
//  Created by Mateusz Zembol on 16.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "main.h"
#include "generator.h"
#include "sort.h"
#include "copy.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>


int main(int argc, char *argv[]) {
    
    char * function = argv[1];
    
    if( strcmp(function, "generate") == 0) {
        
        char * file1_name = argv[2];
        int number_of_records = atoi(argv[3]);
        int record_length = atoi(argv[4]);

        if(record_length<=0){ printf("record length must be greater than 0!"); return 0;}
        if(number_of_records<=0){ printf("number of records must be greater than 0!"); return 0;}
        generate(file1_name, record_length, number_of_records);
    }
    else if (strcmp(function, "sort") == 0 ){
        
        char * file1_name = argv[2];
        int record_length = atoi(argv[4]);
        char * additional_tool = argv[5];
        
        if( additional_tool == NULL){
            if(record_length<=0){ printf("record length must be greater than 0!"); return 0;}
            if(access(file1_name, F_OK) == -1){ printf("file doesn't exist"); return 0;}
            sort(file1_name, record_length);
        }
        else if(strcmp(additional_tool, "sys") == 0){
            if(record_length<=0){ printf("record length must be greater than 0!"); return 0;}
            if(access(file1_name, F_OK) == -1){ printf("file doesn't exist"); return 0;}
            sort_sys(file1_name, record_length);
        }else{
            printf("You defined function, that doesn't exists!");
        }
    }
    else if (strcmp(function, "copy") == 0 ){
        
        char * file1_name = argv[2];
        char * file2_name = argv[3];
        int record_length = atoi(argv[5]);
        char * additional_tool = argv[6];
        
        
        if(additional_tool  == NULL){
            if(record_length<=0){ printf("record length must be greater than 0!"); return 0;}
            if(access(file1_name, F_OK) == -1){ printf("file doesn't exist"); return 0;}
            copy(file1_name,file2_name,record_length);
        }
        else if(strcmp(additional_tool, "sys") == 0){
            if(record_length<=0){ printf("record length must be greater than 0!"); return 0;}
            if(access(file1_name, F_OK) == -1){ printf("file doesn't exist"); return 0;}
            copy_sys(file1_name,file2_name,record_length);
        }else{
            printf("You defined function, that doesn't exists!");
        }
    }
    else{
        printf("You defined function, that doesn't exists!");
    }
return 0;

}
