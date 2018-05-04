//
//  main.c
//  lab2
//
//  Created by Mateusz Zembol on 14.03.2018.
//  Copyright © 2018 Mateusz Zembol. All rights reserved.
//

#include <math.h>
#include <time.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

void showFileInfo(int);

void findFiles(char* fullPath, char* comparison, char* date2){
   
    
    char *date = date2;
    date[4] = date[7] = date[10] = date[13] = date[16] = '\0';
    struct tm tmdate = {0};
    tmdate.tm_year = atoi(&date[0]) - 1900;
    tmdate.tm_mon = atoi(&date[5]) - 1;
    tmdate.tm_mday = atoi(&date[8]);
    time_t t = timelocal( &tmdate );
    
    
    char path[1000];
    long int definedTime = t + atoi(&date[11])*3600 + atoi(&date[14])*60 + atoi(&date[17]);
    
    
    DIR* directoryPointer = opendir(fullPath); //otwieramy strumien
    if(directoryPointer==NULL){
        return;
    }
    
    struct dirent* dirEntity = readdir(directoryPointer);
    struct stat fileStat;
    
    
    while(dirEntity!=NULL){

        if(strcmp(dirEntity->d_name,".")==0 || strcmp(dirEntity->d_name,"..")==0){
            dirEntity = readdir(directoryPointer);
            continue;
        }
        
        strcpy(path,fullPath);
        strcat(path,"/");
        strcat(path,dirEntity->d_name); //aktualizujemy sciezke

        lstat(path, &fileStat);
        
        
        
        if(S_ISDIR(fileStat.st_mode)){

            findFiles(path, comparison, date); // wywolujemy sie rekrunencyjnie na nastepnej sciezce (glebiej)
            dirEntity = readdir(directoryPointer);
            continue;
        }
       
        //jak nieregularny, ma dowiazania symboliczne lub urządzenia blokowe.
       
        if( !S_ISREG(fileStat.st_mode)){
            dirEntity = readdir(directoryPointer);
            continue;
        }
        if(S_ISBLK(fileStat.st_mode)){
            dirEntity = readdir(directoryPointer);
            continue;
        }
        if(S_ISLNK(fileStat.st_mode)){
            dirEntity = readdir(directoryPointer);
            continue;
        }

        
    
         double time = difftime((double)trunc(fileStat.st_mtime), (double)trunc( definedTime));

        //sprawdzamy zgodnosc z wymaganiami uzytkowanika
        if(*comparison == '>' && time<=0 ){
          
                dirEntity = readdir(directoryPointer);
                continue;
        }
        else if (*comparison == '<' && time >= 0){
            
                    dirEntity = readdir(directoryPointer);
                    continue;
        }
        else if(*comparison == '=' && time != 0){
            
                    dirEntity = readdir(directoryPointer);
                    continue;
        }
        else{
        
        //wypisujemy interesujace nas informacje
        printf("%s\n",path);
        
        printf("File size: %lli\n",fileStat.st_size);
        
        printf("File Permissions: \t");
        printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
        printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
        printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
        printf("\nLast file modification:   %s\n", ctime(&fileStat.st_mtime));
       
            dirEntity = readdir(directoryPointer);

        }
    }
}

            
            
int main(int argc,char *argv[]){
    if(argc!=4){
        printf("Podaj 3 argumenty: sciezke, znak porownawczy i date\n");
        return 1;
    }
    char* directory = (argv[1]);
    char* comparison = (argv[2]);
    char* date  = (argv[3]);
    
    findFiles(realpath(directory, NULL),comparison, date);
    
    return 0;
}
