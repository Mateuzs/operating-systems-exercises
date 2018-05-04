//
//  main.c
//  lab1
//
//  Created by Mateusz Zembol on 07.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Table {
    
    int size;
    char **  table;
    
} Table;



 struct Table createTableDynamically(int memorySize){
    
    struct Table table;

    table.size = memorySize;
    table.table = calloc(table.size, sizeof(char*));
    
    return table;
    
}

void addBlockToDynamicTable(struct Table table, char block[]){
    
    for(int i = 0; i< table.size; i++){
    
        if(table.table[i] == NULL){
            
            table.table[i] = calloc(strlen(block), sizeof(char));
            for(int j=0;j<strlen(block);j++) table.table[i][j] = block[j];
            return;
        
        }
    }

    printf("the table is full, please free some space or create another one.");
}



void deleteBlockFromDynamicTable(struct Table table , int index){
    
    free(table.table[index]);
    table.table[index] = 0;
}


void deleteDynamicTable(struct Table table){
    
    for(int i=0;i<table.size;i++) free(table.table[i]);
    
    free(table.table);
}

char * findClosestBlockInDynamicTable(struct Table table, int index){
    
    if(index > table.size){
        printf("\nindex is out of bound!");
        return NULL;
    }
    
    char * closest = NULL;
    int sum = 0, temporarySum = 0, sumOfElement = 0;
   
    for(int j=0;j<strlen(table.table[index]);j++) sumOfElement += table.table[index][j];
    
    
    for(int i=0;i<table.size;i++){
        if(table.table[i] != NULL){
            
            temporarySum = 0;
            for(int j=0;j<strlen(table.table[i]) ; j++) temporarySum += table.table[i][j];
        
                if( (closest == NULL && i != index) || (i != index && abs(sumOfElement - sum) > abs(sumOfElement - temporarySum))){
                    
                    sum = temporarySum;
                    closest = table.table[i];
                }
        }
    }
    return closest;
}





