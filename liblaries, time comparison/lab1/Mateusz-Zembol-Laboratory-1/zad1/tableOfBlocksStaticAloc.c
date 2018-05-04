//
//  tableOfBlocksStaticAloc.c
//  lab1
//
//  Created by Mateusz Zembol on 07.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct StaticTable {
    
    int sizeofBlock;
    int numberOfElements;
    char **  table;
    
} StaticTable;




struct StaticTable createTableStatically(int numberOfElements, int sizeOfBlock){
    
    struct StaticTable table;
    
    table.numberOfElements = numberOfElements;
    table.sizeofBlock = sizeOfBlock;
    
    table.table = calloc(table.numberOfElements, sizeof(char*));
    
    for(int i = 0 ; i< numberOfElements; i++)
        table.table[i] = calloc(table.sizeofBlock, sizeof(char));
        
    return table;
    
}



void addBlockToStaticTable(struct StaticTable table, char block[]){
    
    if(strlen(block)>table.sizeofBlock){
        printf("\nCannot add  block \"%s\" to the table. Size of block is too big.", block);
        return;
    }
    
    
    for(int i = 0; i< table.numberOfElements; i++){
        
        if(strlen(table.table[i]) == 0){
            
            for(int j=0;j<strlen(block);j++) table.table[i][j] = block[j];
            return;
            
        }
    
}

     printf("the table is full, please free some space or create another one.");
}




void deleteBlockFromStaticTable(struct StaticTable table , int index){
    
    free(table.table[index]);
    table.table[index] = calloc(table.sizeofBlock, sizeof(char));
}


void deleteStaticTable(struct StaticTable table){
    
    for(int i=0;i<table.numberOfElements;i++) free(table.table[i]);
    
    free(table.table);
}


char * findClosestBlockInStaticTable(struct StaticTable table, int index){
    
    if(index > table.numberOfElements){
        printf("\nindex is out of bound!");
        return NULL;
    }
    
    char * closest = NULL;
    int sum = 0, temporarySum = 0, sumOfElement = 0;
    
    for(int j=0; table.table[index][j] >0;j++) sumOfElement += table.table[index][j];
    
    
    for(int i=0;i<table.numberOfElements;i++){
        if(table.table[i] != 0){
            
            temporarySum = 0;
            for(int j=0; table.table[i][j] > 0 ; j++) temporarySum += table.table[i][j];
            
            if( (closest == NULL && i != index) || (i != index && abs(sumOfElement - sum) > abs(sumOfElement - temporarySum))){
                
                sum = temporarySum;
                closest = table.table[i];
            }
        }
    }
    return closest;
}




