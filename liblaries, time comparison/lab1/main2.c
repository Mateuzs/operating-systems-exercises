
#include<stdio.h>
#include<stdlib.h>
#include "tableOfBlocksDynamicAloc.h"
#include "tableOfBlocksStaticAloc.h"
#include "dataBase.h"
#include <sys/time.h>
#include <time.h>
#include <sys/resource.h>
#include <errno.h>
#include <dlfcn.h>


void showTime(double systime, double usertime, double realtime, FILE *file){
   
    fprintf(file, "\n\nTime taken to perform this task:\nsystem time: %f us\nuser time: %f us\nreal time: %f us", systime,usertime,realtime);
    printf("\n\nTime taken to perform this task:\nsystem time: %f us\nuser time: %f us\nreal time: %f us", systime,usertime,realtime);
    
}

void setTime(struct rusage * usage,struct timeval *real_time,double *systime,double *usertime,double *realtime){
    
    getrusage(RUSAGE_SELF, usage);
    gettimeofday(real_time,NULL);
            
    *systime = - (usage->ru_stime.tv_sec*1000000.0 + usage->ru_stime.tv_usec);
    *usertime = -(usage->ru_utime.tv_sec*1000000.0 + usage->ru_utime.tv_usec);
    *realtime = -(real_time->tv_sec*1000000.0 + real_time->tv_usec);

}

void countTime(struct rusage * usage,struct timeval *real_time,double *systime,double *usertime,double *realtime){
    
    getrusage(RUSAGE_SELF, usage);
    gettimeofday(real_time,NULL);
    
    *systime +=  (usage->ru_stime.tv_sec*1000000.0 + usage->ru_stime.tv_usec);
    *usertime += (usage->ru_utime.tv_sec*1000000.0 + usage->ru_utime.tv_usec);
    *realtime += (real_time->tv_sec*1000000.0 + real_time->tv_usec);
    
}

int main(int argc, const char * argv[]) {
    
    
    //Declare variable pointer
    void *handle;
    
    struct Table (*createTableDynamically)(int);
    void (*addBlockToDynamicTable)(struct Table , char []);
    void (*deleteBlockFromDynamicTable)(struct Table , int );
    void (*deleteDynamicTable)(struct Table );
    char * (*findClosestBlockInDynamicTable)(struct Table , int);
    
    struct StaticTable (*createTableStatically)(int,int);
    void (*addBlockToStaticTable)(struct StaticTable , char []);
    void (*deleteBlockFromStaticTable)(struct StaticTable  , int );
    void (*deleteStaticTable)(struct StaticTable );
    char * (*findClosestBlockInStaticTable)(struct StaticTable, int );

    
    char *error;
    // Open the library
    handle = dlopen("./libdyncontacts.so", RTLD_LAZY);
    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return 1;
    }

    createTableDynamically = (struct Table (*)(int))dlsym(handle, "createTableDynamically");
    addBlockToDynamicTable = (void (*)(struct Table, char[]))dlsym(handle,"addBlockToDynamicTable");
    deleteBlockFromDynamicTable = (void (*)(struct Table, int))dlsym(handle, "deleteBlockFromDynamicTable");
    deleteDynamicTable = (void (*)(struct Table))dlsym(handle,"deleteDynamicTable");
    findClosestBlockInDynamicTable = (char * (*)(struct Table, int))dlsym(handle, "findClosestBlockInDynamicTable");
    
    createTableStatically = (struct StaticTable (*)(int,int))dlsym(handle, "createTableStatically");
    addBlockToStaticTable = (void (*)(struct StaticTable, char[]))dlsym(handle,"addBlockToDynamicTable");
    deleteBlockFromStaticTable = (void (*)(struct StaticTable, int))dlsym(handle, "deleteBlockFromDynamicTable");
    deleteStaticTable = (void (*)(struct StaticTable))dlsym(handle,"deleteDynamicTable");
    findClosestBlockInStaticTable = (char * (*)(struct StaticTable, int))dlsym(handle, "findClosestBlockInDynamicTable");
    
    
    
    int N = 1000;
    int sizeOfBlock = 1000;
    struct rusage usage;
    struct timeval real_time;
    
    FILE *file =  fopen("/Users/mateuszzembol/Projekty-studia/SysOpy/lab1/lab1/raport2.txt", "a+");
    
    printf("Hello, World!\n");
    
    
    printf("\nWe are going to measure time taken to perform below instructions, let's start!");
    
    printf("\n\nTABLE WITH VARIABLE LEGTH OF BLOCKS:");
    fprintf(file, "\n\nTABLE WITH VARIABLE LEGTH OF BLOCKS:");
    printf("\n\n1. Creating a table with %d elements and adding  %d strings with variable length: ", N,N);
    fprintf(file, "\n\n1. Creating a table with %d elements and adding  %d strings with variable length: ", N,N);

    double systime;
    double usertime;
    double realtime;
    
    setTime(&usage, &real_time, &systime, &usertime, &realtime);
    
    struct Table dynamicTable = createTableDynamically(N);
    addDataToDynamicTable(dynamicTable);

    countTime(&usage, &real_time, &systime, &usertime, &realtime);
    showTime(systime, usertime, realtime, file);
   
    printf("\n\n2. Perform about 100 searchings of string with closest sum to defined one: ");
    fprintf(file, "\n\n2. Perform about 100 searchings of string with closest sum to defined one: ");

    setTime(&usage, &real_time, &systime, &usertime, &realtime);

    for(int i=0; i< N-10; i += 10) findClosestBlockInDynamicTable(dynamicTable, i);
    
    countTime(&usage, &real_time, &systime, &usertime, &realtime);
    showTime(systime, usertime, realtime, file);
    
    printf("\n\n3. Deleting 500 records and then adding 500 new records: ");
    fprintf(file, "\n\n3. Deleting 500 records and then adding 500 new records: ");

    setTime(&usage, &real_time, &systime, &usertime, &realtime);

    deleteAndAddRecords(dynamicTable);
    
    countTime(&usage, &real_time, &systime, &usertime, &realtime);
    showTime(systime, usertime, realtime, file);
    printf("\n\n4. Deleting and adding 1000 records one by one: ");
    fprintf(file, "\n\n4. Deleting and adding 1000 records one by one: ");

    setTime(&usage, &real_time, &systime, &usertime, &realtime);

    deleteThenAddOneByOne(dynamicTable);
    
    countTime(&usage, &real_time, &systime, &usertime, &realtime);
    showTime(systime, usertime, realtime, file);

    printf("\n\nTABLE WITH FIXED LEGTH OF BLOCKS:");
    fprintf(file, "\n\nTABLE WITH FIXED LEGTH OF BLOCKS:");
    fprintf(file, "\n\n1. Creating a table with %d elements and adding  %d strings with variable length: ", N,N);
    printf("\n\n1. Creating a table with %d elements and adding  %d strings with variable length: ", N,N);
    
    setTime(&usage, &real_time, &systime, &usertime, &realtime);

    struct StaticTable staticTable = createTableStatically(N, sizeOfBlock);
    addDataToStaticTable(staticTable);
    
    countTime(&usage, &real_time, &systime, &usertime, &realtime);
    showTime(systime, usertime, realtime, file);
    
    printf("\n\n2. Perform about 100 searchings of string with closest sum to defined one: ");
    fprintf(file, "\n\n2. Perform about 100 searchings of string with closest sum to defined one: ");

    setTime(&usage, &real_time, &systime, &usertime, &realtime);

    for(int i=0; i< N-10; i += 10) findClosestBlockInStaticTable(staticTable, i);
    
    countTime(&usage, &real_time, &systime, &usertime, &realtime);
    showTime(systime, usertime, realtime, file);
    
    printf("\n\n3. Deleting 500 records and then adding 500 new records: ");
    fprintf(file, "\n\n3. Deleting 500 records and then adding 500 new records: ");

    setTime(&usage, &real_time, &systime, &usertime, &realtime);
    
    deleteAndAddRecordsInStaticTable(staticTable);
    
    countTime(&usage, &real_time, &systime, &usertime, &realtime);
    showTime(systime, usertime, realtime, file);
    
    printf("\n\n4. Deleting and adding 1000 records one by one: ");
    fprintf(file, "\n\n4. Deleting and adding 1000 records one by one: ");

    setTime(&usage, &real_time, &systime, &usertime, &realtime);
    
    deleteThenAddOneByOneInStaticTable(staticTable);
    
    countTime(&usage, &real_time, &systime, &usertime, &realtime);
    showTime(systime, usertime, realtime, file);

    printf("\n\nTHANK YOU FOR YOUR ATTENTION :)\n\n");
    fprintf(file, "\n\nTHANK YOU FOR YOUR ATTENTION :)\n\n");

    
    fclose(file);
    
    
    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "%s\n", error);
        return 1;
    }
    
    dlclose(handle);

    return 0;
}


