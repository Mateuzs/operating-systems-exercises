//
//  tableOfBlocksStaticAloc.h
//  lab1
//
//  Created by Mateusz Zembol on 07.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#ifndef tableOfBlocksStaticAloc_h
#define tableOfBlocksStaticAloc_h

#include <stdio.h>
#include <stdlib.h>

typedef struct StaticTable {
    
    int sizeofBlock;
    int numberOfElements;
    char **  table;
    
} StaticTable;


struct StaticTable createTableStatically(int numberOfElements, int sizeOfBlock);
void addBlockToStaticTable(struct StaticTable table, char block[]);
void deleteBlockFromStaticTable(struct StaticTable table , int index);
void deleteStaticTable(struct StaticTable table);
char * findClosestBlockInStaticTable(struct StaticTable, int index);




#endif /* tableOfBlocksStaticAloc_h */
