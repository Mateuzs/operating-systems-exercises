//
//  nftw.c
//  lab2
//
//  Created by Mateusz Zembol on 14.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include "nftw.h"
#include <math.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char **largs;


static int displayInformation(const char *fullPath, const struct stat *fileStat,
                              int tflag, struct FTW *ftwbuf){
   
    char* comparison = (largs[2]);
    char *date = largs[3];

    date[4] = date[7] = date[10] = date[13] = date[16] = '\0';
    struct tm tmdate = {0};
    tmdate.tm_year = atoi(&date[0]) - 1900;
    tmdate.tm_mon = atoi(&date[5]) - 1;
    tmdate.tm_mday = atoi(&date[8]);
    time_t t = timelocal( &tmdate );
    
    long int definedTime = t + atoi(&date[11])*3600 + atoi(&date[14])*60 + atoi(&date[17]);

    
    long double time = difftime(trunc(fileStat->st_mtime), trunc(definedTime));

    
    
    
    //sprawdzamy zgodnosc z wymaganiami uzytkowanika
    if(tflag != FTW_F){//jak nie regularny
        return 0;
    }
    if(tflag == FTW_D){//jak folder
        return 0;
    }
    if(tflag == FTW_SL){//jak nie regular
        return 0;
    }
    
    
    
    if(*comparison == '>' && time<=0 ){
        
        return 0;
        
    }
    else if (*comparison == '<' && time >= 0){
        
        return 0;
        
    }
    else if(*comparison == '=' && time != 0){
        
        return 0;
        
    }
    else{
        
        //wypisujemy interesujace nas informacje
        printf("%s\n",fullPath);
        
        printf("File size: %lli\n",fileStat->st_size);
        
        printf("File Permissions: \t");
        printf( (S_ISDIR(fileStat->st_mode)) ? "d" : "-");
        printf( (fileStat->st_mode & S_IRUSR) ? "r" : "-");
        printf( (fileStat->st_mode & S_IWUSR) ? "w" : "-");
        printf( (fileStat->st_mode & S_IXUSR) ? "x" : "-");
        printf( (fileStat->st_mode & S_IRGRP) ? "r" : "-");
        printf( (fileStat->st_mode & S_IWGRP) ? "w" : "-");
        printf( (fileStat->st_mode & S_IXGRP) ? "x" : "-");
        printf( (fileStat->st_mode & S_IROTH) ? "r" : "-");
        printf( (fileStat->st_mode & S_IWOTH) ? "w" : "-");
        printf( (fileStat->st_mode & S_IXOTH) ? "x" : "-");
        printf("\nLast file modification:   %s\n", ctime(&fileStat->st_mtime));
        
        
        return 0;
    }
    
}


int
main(int argc, char *argv[])
{
    
    largs = argv;
    if(argc!=4){
        printf("Podaj 3 argumenty: sciezke, znak porownawczy i date\n");
        return 1;
    }
    char* directory = (argv[1]);
    int flags = 0;
    flags |= FTW_DEPTH;
    flags |= FTW_PHYS;
   
    
    if (nftw(directory, displayInformation, 30, flags) == -1) {
        perror("nftw");
        return 1;
    }
    
    return 0;
}

