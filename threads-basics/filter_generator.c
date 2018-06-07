//
//  filter_generator.c
//  threads-basics
//
//  Created by Mateusz Zembol on 22.05.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "filter_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
        if (argc != 3)
        {
            printf("give the Constant and path!!"); exit(1);
        }
        
        int     constant = atoi(argv[1]);;
        double  **values;
        double  sum = 0;
        FILE    *filter;
        char    *path = argv[2];
    
        values = malloc(constant * sizeof(float *));
    
    for(int i = 0; i < constant ; i ++){
        values[i] = malloc(constant * sizeof(float *));
    }
    
    
    for (int i = 0; i < constant; ++i)
        {
            for (int j = 0; j < constant; ++j)
            {
                values[i][j] = rand() / (double) RAND_MAX;
                sum += values[i][j];
            }
        }
    
        filter = fopen(path, "w");
        fprintf(filter, "%d\n", constant);
        
        for (int i = 0; i < constant; ++i)
        {
            for (int j = 0; j < constant; ++j)
            {
                values[i][j] /= sum;
                fprintf(filter, "%lf ", values[i][j]);
            }
            fprintf(filter, "\n");
        }
        
        free(values);
        fclose(filter);
        
        return 0;
    }
