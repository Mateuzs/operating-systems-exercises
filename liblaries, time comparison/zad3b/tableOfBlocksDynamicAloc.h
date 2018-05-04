//
//  tableOfBlocks.h
//  lab1
//
//  Created by Mateusz Zembol on 07.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#ifndef tableOfBlocksDynamicAloc_h
#define tableOfBlocksDynamicAloc_h

#include<stdlib.h>
#include<stdio.h>

typedef struct Table {
    
    int size;
    char **  table;
    
} Table;

struct Table createTableDynamically(int memorySize);
void addBlockToDynamicTable(struct Table table, char block[]);
void deleteBlockFromDynamicTable(struct Table table , int index);
void deleteDynamicTable(struct Table table);
char * findClosestBlockInDynamicTable(struct Table table, int index);





#endif /* tableOfBlocks_h */
