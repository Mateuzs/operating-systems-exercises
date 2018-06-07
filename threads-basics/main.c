//
//  main.c
//  threads-basics
//
//  Created by Mateusz Zembol on 22.05.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include <fcntl.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zconf.h>

int **raw_image; //considered as a matrix of numbers
int **filtered_image;
float **filter_matrix;
int image_width;
int image_height;
int pixel_max_value;
int constant; //constant defines number of rows and columns in filter matrix
int number_of_threads;
char title[2];

// SMALL COUNTING FUNCTIONS

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

double real_time(struct timeval start, struct timeval end) {
    
    return (((double) end.tv_sec * 1000000 + end.tv_usec) - ((double) start.tv_sec * 1000000 + start.tv_usec)) / 1000000;
}

double user_time(struct rusage *r_usage) {
   
    return (((double) (r_usage->ru_utime.tv_sec) * 1000000) + r_usage->ru_utime.tv_usec) / 1000000;
}

double system_time(struct rusage *r_usage) {
    
    return (((double) (r_usage->ru_stime.tv_sec) * 1000000) + r_usage->ru_stime.tv_usec) / 1000000;
}



// ADDITIONAL FUNCTIONS

void get_data(char **argv){
    
    FILE * image;
    image = fopen(argv[2],"r");
    
    if(image == NULL){
        printf("\nError during opening the image\n");
        exit(1);
    }

    fscanf(image, "%s %d %d %d", title, &image_width, &image_height, &pixel_max_value);
    
    
    raw_image = malloc(image_width * sizeof(int *));
    filtered_image = malloc(image_width * sizeof(int *));
    
    if(raw_image == NULL || filtered_image == NULL){
        printf("\nError during alocating memory\n");
        exit(1);
    }
    
    
    for(int i = 0;  i < image_width; i++){
        
        raw_image[i] = malloc(image_height * sizeof(int));
        filtered_image[i] = malloc(image_height * sizeof(int));
        
        if(raw_image[i] == NULL || filtered_image[i] == NULL){
            printf("\nError during alocating memory\n");
            exit(1);
        }
        
        for(int j = 0 ; j< image_height ; j++){
            
            fscanf(image, "%d", &raw_image[i][j]);
            
        }
    }
    
    fclose(image);

    FILE * filter = fopen(argv[3], "r");
    
    if(filter == NULL){
        printf("\nError while opening the filter file\n");
        exit(1);
    }
    
    fscanf(filter, "%d", &constant);
    
    filter_matrix = malloc(constant * sizeof(float *));
    
    if(filter_matrix == NULL){
        printf("\nError during alocating memory\n");
        exit(1);
    }
    
    for( int i = 0; i < constant ; i ++){
        
        filter_matrix[i] = malloc(constant * sizeof(float));
        if(filter_matrix[i] == NULL){
            printf("\nError during alocating memory\n");
            exit(1);
        }
        
        for(int j = 0 ; j < constant ; j++){
            
            fscanf(filter, "%f", &filter_matrix[i][j]);
        
        }
    }
    
    fclose(filter);
}



void *compute_filtered_image(void *thread_index){
    
    int x,y,a,b;
    double sum;
    int thread_number = *(int *)thread_index;
    
    int start_point = thread_number * ((image_width * image_height)/number_of_threads);
    int end_point =  min(start_point + ((image_width * image_height)/number_of_threads), image_width*image_height);
    
    for(int i = start_point; i < end_point ; ++i){
        
        x = i / image_width; // row
        y = i % image_width; // column
        sum = 0;
        
        for(int j = 0 ; j < constant; j ++){
            for(int k = 0 ; k < constant ; k ++){
                
                a = min(image_height - 1, max(1, x - (int) ceil(constant / 2) + k));
                b = min(image_width - 1, max(1, y - (int) ceil(constant / 2) + j));
                
                sum += raw_image[a][b] * filter_matrix[j][k];
            }
        }
        filtered_image[x][y] = (int) round(sum);
    }
    
    return NULL; // PTHREAD_JOIN WAITS FOR IT
}



void save_image(char *path) {
    
    FILE *destination;
    
    if ((destination = fopen(path, "w")) == NULL) {
        printf("Cannot open the result file\n");
        exit(1);
    }
    
    fprintf(destination, "%s\n%d %d\n%d\n", title, image_width, image_height, pixel_max_value);
    
    for (int i = 0; i < image_height; ++i) {
        for (int j = 0; j < image_width; ++j) {
            fprintf(destination, "%d ", filtered_image[i][j]);
        }
        fprintf(destination, "\n");
    }
    
    fclose(destination);
}



int main(int argc,  char * argv[]) {
   
    if (argc != 5){
        printf("\nWrong number of arguments! Required: number of threads, input file, filter file, output file!\n");
        exit(1);
    }

    get_data(argv);

    
    struct rusage *r_usage = malloc(sizeof(struct rusage));
    struct timeval start, end;
    
    number_of_threads = (int) strtol(argv[1], NULL, 10);
    
    if (number_of_threads > sysconf(_SC_NPROCESSORS_ONLN)) {
        printf("\n!!! Warning: this system has only %ld processors available !!!\n",
               sysconf(_SC_NPROCESSORS_ONLN));
    }
    
    // start counting the time
    gettimeofday(&start, NULL);
    
    // allocate threads
    pthread_t threads[number_of_threads];
   
   
    // compute the filtered image!
    
    for(int i = 0 ; i < number_of_threads ; i ++){
        
        int *thread_index = malloc(sizeof(int));
        *thread_index = i;
        pthread_create(threads + i, NULL, compute_filtered_image, thread_index);
        }
    
    //wait till the last thread
    for(int i = 0 ; i < number_of_threads ; i ++){
        pthread_join(threads[i], NULL);
    }
    
        
    // Point the measure
    gettimeofday(&end, NULL);
    getrusage(RUSAGE_SELF, r_usage);
    
    save_image(argv[4]);
    
    printf("Results: number of threads: %d\tReal time: %fs\t User time: %fs\t System time %fs\n", number_of_threads, real_time(start, end), user_time(r_usage), system_time(r_usage));
    
    
    FILE * report = fopen("Times.txt","a");
    
    
    fprintf(report, "Results: number of threads: %d\tReal time: %fs\t User time: %fs\t System time %fs\n", number_of_threads, real_time(start, end), user_time(r_usage), system_time(r_usage));
    
    fclose(report);
    free(raw_image);
    free(filtered_image);
    free(filter_matrix);
    return 0;
}

