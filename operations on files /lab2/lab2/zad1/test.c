//
//  test.c
//  lab2
//
//  Created by Mateusz Zembol on 16.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "test.h"


#include "main.h"
#include "generator.h"
#include "sort.h"
#include "copy.h"
#include "myTime.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/times.h>
#include <string.h>


void test( char *argv[]) {
    
    char * function = argv[1];
    
    if( strcmp(function, "generate") == 0) {
        
        char * file1_name = argv[2];
        int number_of_records = atoi(argv[3]);
        int record_length = atoi(argv[4]);
        
        if(record_length<=0){ printf("record length must be greater than 0!"); return ;}
        if(number_of_records<=0){ printf("number of records must be greater than 0!"); return ;}
        generate(file1_name, record_length, number_of_records);
    }
    else if (strcmp(function, "sort") == 0 ){
        
        char * file1_name = argv[2];
        int record_length = atoi(argv[4]);
        char * additional_tool = argv[5];
        
        if( additional_tool == NULL){
            if(record_length<=0){ printf("record length must be greater than 0!"); return ;}
            if(access(file1_name, F_OK) == -1){ printf("file doesn't exist"); return ;}
            sort(file1_name, record_length);
        }
        else if(strcmp(additional_tool, "sys") == 0){
            
            if(record_length<=0){ printf("record length must be greater than 0!"); return ;}
            if(access(file1_name, F_OK) == -1){ printf("file doesn't exist"); return ;}
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
            if(record_length<=0){ printf("record length must be greater than 0!"); return ;}
            if(access(file1_name, F_OK) == -1){ printf("file doesn't exist"); return ;}
            copy(file1_name,file2_name,record_length);
        }
        else if(strcmp(additional_tool, "sys") == 0){
            if(record_length<=0){ printf("record length must be greater than 0!"); return ;}
            if(access(file1_name, F_OK) == -1){ printf("file doesn't exist"); return ;}
            copy_sys(file1_name,file2_name,record_length);
        }else{
            printf("You defined function, that doesn't exists!");
        }
    }
    else{
        printf("You defined function, that doesn't exists!");
    }
    return ;
    
}


int main(int argc, char *argv[]){
    
    struct rusage usage;
    double systime;
    double usertime;
    
    FILE *file = fopen("wyniki.txt", "a+");
    
    char * array[] =  {"4","generate","file12","2000","4",NULL,NULL};
    printf("\nCreating 2000 4-byte records: ");
    test(array);
    array[1]="generate";array[2]="file15";array[3]="5000";array[4]="4";
    printf("\nCreating 5000 4-byte records: ");
    test(array);
    array[1]="generate";array[2]="file22";array[3]="2000";array[4]="512";
    printf("\nCreating 2000 512-byte records: ");
    test(array);
    array[1]="generate";array[2]="file25";array[3]="5000";array[4]="512";
    printf("\nCreating 5000 512-byte records: ");
    test(array);
    array[1]="generate";array[2]="file32";array[3]="2000";array[4]="4096";
    printf("\nCreating 2000 4096-byte records: ");
    test(array);
    array[1]="generate";array[2]="file35";array[3]="5000";array[4]="4096";
    printf("\nCreating 5000 4096-byte records: ");
    test(array);
    array[1]="generate";array[2]="file42";array[3]="2000";array[4]="8192";
    printf("\nCreating 2000 8192-byte records: ");
    test(array);
    array[1]="generate";array[2]="file45";array[3]="5000";array[4]="8192";
    printf("\nCreating 5000 8192-byte records: ");
    test(array);

    
    
    
    
    
    printf("\nCopying 2000 4-byte records: ");
    fprintf(file, "\nCopying 2000 4-byte records: ");
    array[1]="copy";array[2]="file12";array[3]="file122";array[4]="2000";array[5]="4";array[6]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf("\nCopying 5000 4-byte records: ");
    fprintf(file, "\nCopying 5000 4-byte records: ");
    array[1]="copy";array[2]="file15";array[3]="file155";array[4]="5000";array[5]="4";array[6]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
   
    printf("\nCopying 2000 4-byte records with sys functions: ");
    fprintf(file, "\nCopying 2000 4-byte records with sys functions: ");
    array[1]="copy";array[2]="file12";array[3]="file1222";array[4]="2000";array[5]="4";array[6]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf("\nCopying 5000 4-byte records with sys functions: ");
    fprintf(file, "\nCopying 5000 4-byte records with sys functions: ");
    array[1]="copy";array[2]="file15";array[3]="file1555";array[4]="5000";array[5]="4";array[6]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);

    printf( "\nCopying 2000 512-byte records: ");
    fprintf(file, "\nCopying 2000 512-byte records: ");
    array[1]="copy";array[2]="file22";array[3]="file222";array[4]="2000";array[5]="512";array[6]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nCopying 5000 512-byte records: ");
    fprintf(file, "\nCopying 5000 512-byte records: ");
    array[1]="copy";array[2]="file25";array[3]="file255";array[4]="5000";array[5]="512";array[6]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);

    
    printf( "\nCopying 2000 512-byte records with sys functions: ");
    fprintf(file, "\nCopying 2000 512-byte records with sys functions: ");
    array[1]="copy";array[2]="file22";array[3]="file2222";array[4]="2000";array[5]="512";array[6]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nCopying 5000 512-byte records with sys functions: ");
    fprintf(file, "\nCopying 5000 512-byte records with sys functions: ");
    array[1]="copy";array[2]="file25";array[3]="file2555";array[4]="5000";array[5]="512";array[6]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);

    printf( "\nCopying 2000 4096-byte records: ");
    fprintf(file, "\nCopying 2000 4096-byte records: ");
    array[1]="copy";array[2]="file32";array[3]="file322";array[4]="2000";array[5]="4096";array[6]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nCopying 5000 4096-byte records: ");
    fprintf(file, "\nCopying 5000 4096-byte records: ");
    array[1]="copy";array[2]="file35";array[3]="file355";array[4]="5000";array[5]="4096";array[6]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nCopying 2000 4096-byte records with sys functions: ");
    fprintf(file, "\nCopying 2000 4096-byte records with sys functions: ");
    array[1]="copy";array[2]="file32";array[3]="file3222";array[4]="2000";array[5]="4096";array[6]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nCopying 5000 4096-byte records with sys functions: ");
    fprintf(file, "\nCopying 5000 4096-byte records with sys functions: ");
    array[1]="copy";array[2]="file35";array[3]="file3555";array[4]="5000";array[5]="4096";array[6]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);


    printf( "\nCopying 2000 8192-byte records: ");
    fprintf(file, "\nCopying 2000 8192-byte records: ");
    array[1]="copy";array[2]="file42";array[3]="file422";array[4]="2000";array[5]="8192";array[6]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nCopying 5000 8192-byte records: ");
    fprintf(file, "\nCopying 5000 8192-byte records: ");
    array[1]="copy";array[2]="file45";array[3]="file455";array[4]="5000";array[5]="8192";array[6]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);


    printf( "\nCopying 2000 8192-byte records with sys functions: ");
    fprintf(file, "\nCopying 2000 8192-byte records with sys functions: ");
    array[1]="copy";array[2]="file42";array[3]="file4222";array[4]="2000";array[5]="8192";array[6]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nCopying 5000 8192-byte records with sys functions: ");
    fprintf(file, "\nCopying 5000 8192-byte records with sys functions: ");
    array[1]="copy";array[2]="file45";array[3]="file4555";array[4]="5000";array[5]="8192";array[6]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    
    
    
    
    printf( "\nSorting 2000 4-byte records: ");
    fprintf(file, "\nSorting 2000 4-byte records: ");
    array[1]="sort";array[2]="file122";array[3]="2000";array[4]="4";array[5]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 5000 4-byte records: ");
    fprintf(file, "\nSorting 5000 4-byte records: ");
    array[1]="sort";array[2]="file155";array[3]="5000";array[4]="4";array[5]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);

    
    printf( "\nSorting 2000 4-byte records with sys functions: ");
    fprintf(file, "\nSorting 2000 4-byte records: ");
    array[1]="sort";array[2]="file1222";array[3]="2000";array[4]="4";array[5]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 5000 4-byte records with sys functions: ");
    fprintf(file, "\nSorting 5000 4-byte records: ");
    array[1]="sort";array[2]="file1555";array[3]="5000";array[4]="4";array[5]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);

    
    printf( "\nSorting 2000 512-byte records: ");
    fprintf(file, "\nSorting 2000 512-byte records: ");
    array[1]="sort";array[2]="file222";array[3]="2000";array[4]="512";array[5]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 5000 512-byte records: ");
    fprintf(file, "\nSorting 5000 512-byte records: ");
    array[1]="sort";array[2]="file255";array[3]="5000";array[4]="512";array[5]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 2000 512-byte records with sys functions: ");
    fprintf(file, "\nSorting 2000 512-byte records with sys functions: ");
    array[1]="sort";array[2]="file2222";array[3]="2000";array[4]="512";array[5]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 5000 512-byte records with sys functions: ");
    fprintf(file, "\nSorting 5000 512-byte records with sys functions: ");
    array[1]="sort";array[2]="file2555";array[3]="5000";array[4]="512";array[5]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 2000 4096-byte records: ");
    fprintf(file, "\nSorting 2000 4096-byte records: ");
    array[1]="sort";array[2]="file322";array[3]="2000";array[4]="4096";array[5]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 5000 4096-byte records: ");
    fprintf(file, "\nSorting 5000 4096-byte records: ");
    array[1]="sort";array[2]="file355";array[3]="5000";array[4]="4096";array[5]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 2000 4096-byte records with sys functions: ");
    fprintf(file, "\nSorting 2000 4096-byte records with sys functions: ");
    array[1]="sort";array[2]="file3222";array[3]="2000";array[4]="4096";array[5]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 5000 4096-byte records with sys functions: ");
    fprintf(file, "\nSorting 5000 4096-byte records with sys functions: ");
    array[1]="sort";array[2]="file3555";array[3]="5000";array[4]="4096";array[5]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);

    printf( "\nSorting 2000 8192-byte records: ");
    fprintf(file, "\nSorting 2000 8192-byte records: ");
    array[1]="sort";array[2]="file422";array[3]="2000";array[4]="8192";array[5]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 5000 8192-byte records: ");
    fprintf(file, "\nSorting 5000 8192-byte records: ");
    array[1]="sort";array[2]="file455";array[3]="5000";array[4]="8192";array[5]=NULL;
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);
    
    printf( "\nSorting 2000 8192-byte records with sys functions: ");
    fprintf(file, "\nSorting 2000 8192-byte records with sys functions: ");
    array[1]="sort";array[2]="file4222";array[3]="2000";array[4]="8192";array[5]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);    
    countTime(&usage, &systime, &usertime, file);

    printf( "\nSorting 5000 8192-byte records with sys functions: ");
    fprintf(file, "\nSorting 5000 8192-byte records with sys functions: ");
    array[1]="sort";array[2]="file4555";array[3]="5000";array[4]="8192";array[5]="sys";
    setTime(&usage, &systime, &usertime);
    test(array);
    countTime(&usage, &systime, &usertime, file);

    
    fclose(file);
    printf("\n\n");
    return 0;
}

