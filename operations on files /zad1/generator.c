//
//  zad1.c
//  lab2
//
//  Created by Mateusz Zembol on 16.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>



char getRandomChar() {
    char a = 'A' + (rand() % 26);
    return a;
}

void getRandomRecord(char* record, int record_length) {
    int i;
    for(i=0; i<record_length; i++) {
        record[i] = getRandomChar();
    }
}


void generate(char * file_name, int record_length, int record_number) {
   
    srand(time(NULL));
    FILE *file = fopen(file_name, "wb+");
    
    int length = record_length + 2;     // 2 more for new line chars
    
    char** records = (char**) calloc(record_number, sizeof(char*));
    
    int i;
    for (i=0; i<record_number; i++) {
      
        records[i] = (char*) calloc(length, sizeof(char));
        getRandomRecord(records[i], record_length);
        records[i][length-2] = '\r';
        records[i][length-1] = '\n';
        fwrite(records[i], length, sizeof(char), file);
    }
    
    fclose(file);
}





