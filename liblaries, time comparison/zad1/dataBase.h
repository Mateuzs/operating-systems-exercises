//
//  dataBase.h
//  lab1
//
//  Created by Mateusz Zembol on 09.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#ifndef dataBase_h
#define dataBase_h

#include <stdio.h>
#include "main.h"
#include "tableOfBlocksDynamicAloc.h"
#include "tableOfBlocksStaticAloc.h"


void addDataToDynamicTable(struct Table table);
void deleteAndAddRecords(struct Table table);
void deleteThenAddOneByOne(struct Table table);
void addDataToStaticTable(struct StaticTable table);
void deleteAndAddRecordsInStaticTable(struct StaticTable table);
void deleteThenAddOneByOneInStaticTable(struct StaticTable table);


#endif /* dataBase_h */
