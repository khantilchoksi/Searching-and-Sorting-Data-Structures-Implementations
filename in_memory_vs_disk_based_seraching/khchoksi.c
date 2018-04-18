//
//  readbinary.c
//  ads_assignment1
//
//  Created by Khantil Choksi on 1/21/18.
//  Copyright © 2018 Khantil Choksi. All rights reserved.
//

#include <stdio.h>
#include <sys/time.h>
#include <string.h>

void inMemoryFileRead(FILE* filePointer, int* array);

void inMemorySequential(char *keysFileName, int *S, int numSeeks);
void inMemoryBinary(char *keysFileName, int *S, int numSeeks);
void diskSequential(char *keysFileName, int *S, int numSeeks);
void diskBinary(char *keysFileName, int *S, int numSeeks);

int main(int argc, char** argv)
{
    //assn_1 search-mode keyfile-name seekfile-name
    //assn_1 --disk-lin key.db seek.db
    if(argc!=4){
        printf("\n Not enough number of arguments! Arguments should include the search mode, key.db file, and seek.db file.");
        return 0;
    }
    //Options
    char *inMemSeq = "--mem-lin";
    char *inMemBin = "--mem-bin";
    char *diskSeq = "--disk-lin";
    char *diskBin = "--disk-bin";
    
    //File pointers
    FILE *seekFile;
    //size of files
    long seekFileSize;

    //Seek File open, load
    //Open seek file
    //seekFile = fopen("/Users/khantil/Developer/MCSStudy/ADS/assn_1/seek.db" , "rb"); //argv[3]
    seekFile = fopen(argv[3] , "rb");
    
    //Checking whether seek file exists or not
    if(seekFile == NULL){
        printf("Seek.db File not found");
        return 0;
    }
    
    //Calculating the number of the seeks in file
    fseek(seekFile, 0L, SEEK_END);      //going to end of file
    seekFileSize = ftell(seekFile);      //then storing the file size
    const int numSeeks = seekFileSize/sizeof(int);    //as it is mentioned, the file contains only integers
    //printf("\n Seek File Size: %ld ",seekFileSize);
    //printf("\n Number of Integers in seekFile: %d ",numSeeks);
    
    //Seeks array - in memory sequential read
    int S[numSeeks];
    
    inMemoryFileRead(seekFile, S);
    /*for(int i = 0 ;i<numSeeks;i++){
     printf(" ::  %d",S[i]);
     }*/
   
    //According to search mode option
    if( strcmp(argv[1],inMemSeq) == 0 ){
        //In Memory Sequential Search
        //printf("In memory sequential\n");
        inMemorySequential(argv[2], S, numSeeks);
    }else if( strcmp(argv[1], inMemBin) == 0){
        //In Memory Binary Search
        //printf("In memory binary");
        inMemoryBinary(argv[2], S, numSeeks);
    }else if( strcmp(argv[1], diskSeq) == 0){
        //Disk Sequential Search
        //printf("Disk Sequential");
        diskSequential(argv[2], S, numSeeks);
    }else if( strcmp(argv[1], diskBin) == 0){
        //Disk Binary Search
        //printf("Disk binary");
        diskBinary(argv[2], S, numSeeks);
    }else{
        printf("Wrong Command!");
    }
    //Closing seek file pointer
    fclose(seekFile);
    
    return 0;
}

//IN MEMORY SEQUENTIAL SEARCH
void inMemorySequential(char *keysFileName, int *S, int numSeeks){
    //Key File Pointer
    FILE *keysFile;
    long keyFileSize;
    
    //Time computing structs
    struct timeval startTime;
    struct timeval endTime;
    
    //3. Create a third array of integers called hit of the same size as S. This array to record whether each seek value S[i] exists in K or not
    int hit[numSeeks];
    
    //Start Calculatin time
    // "You must record how much time it takes to open and load key.db, and to then determine the presence or absence of each S[i]."
    gettimeofday(&startTime, NULL);
    //Open the keys file
    keysFile = fopen(keysFileName, "rb");
    
    if(keysFile == NULL){
        printf("Key.db File not found");
        return;
    }

    
    //Calculating the number of the keys in file
    fseek(keysFile, 0L, SEEK_END);      //going to end of file
    keyFileSize = ftell(keysFile);      //then storing the file size
    const int numKeys = keyFileSize/sizeof(int);    //as it is mentioned, the file contains only integers
   
    //keys array
    int K[numKeys], i,j;
    
    //In memory file read
    inMemoryFileRead(keysFile, K);
    //printf("\n Key File Size: %ld ",keyFileSize);
    //printf("\n Number of Integers in keysfile: %d ",numKeys);
    
    
    //In-Memory Sequential Search
    //4. For each S[i], search K sequentially from beginning to end for a matching key value. If S[i] is found in K, set hit[i]=1. If S[i] is not found in K, set hit[i]=0.
    for(i = 0 ; i<numSeeks; i++){
        hit[i] = 0;
        for(j = 0; j<numKeys; j++){
            if(S[i] == K[j]){
                hit[i] = 1;
                printf("%12d: Yes\n",S[i]);
                break;
            }
        }
        if(hit[i] == 0){
            printf("%12d: No\n",S[i]);
        }
        
    }
    gettimeofday(&endTime, NULL);
    struct  timeval result;
    timersub(&endTime, &startTime, &result);
    printf( "Time: %ld.%06ld\n", result.tv_sec, result.tv_usec );
    fclose(keysFile);
}

//IN MEMORY BINARY SEARCH
void inMemoryBinary(char *keysFileName, int *S, int numSeeks){
    //Key File Pointer
    FILE *keysFile;
    long keyFileSize;
    
    //Time computing structs
    struct timeval startTime;
    struct timeval endTime;
    
    //3. Create a third array of integers called hit of the same size as S. This array to record whether each seek value S[i] exists in K or not
    int hit[numSeeks];
    
    //Start Time
    // "You must record how much time it takes to open and load key.db, and to then determine the presence or absence of each S[i]."
    gettimeofday(&startTime, NULL);
    //Open the keys file
    //keysFile = fopen("/Users/khantil/Developer/MCSStudy/ADS/assn_1/key.db" , "rb");
    keysFile = fopen(keysFileName , "rb");
    
    if(keysFile == NULL){
        printf("Key.db File not found");
        return;
    }
    
    //Calculating the number of the keys in file
    fseek(keysFile, 0L, SEEK_END);      //going to end of file
    keyFileSize = ftell(keysFile);      //then storing the file size
    const int numKeys = keyFileSize/sizeof(int);    //as it is mentioned, the file contains only integers
    
    //keys array
    int K[numKeys], i;
    
    //In memory file read
    inMemoryFileRead(keysFile, K);
    //printf("\n Key File Size: %ld ",keyFileSize);
    //printf("\n Number of Integers in keysfile: %d ",numKeys);
    
    
    //Binary Search
    int start, mid , end;
    for(i = 0 ; i<numSeeks; i++){
        hit[i] = 0;
        start = 0;
        end = numKeys-1;
        while(start<=end){
            mid = (start + end) /2;
            if(S[i] == K[mid]){
                hit[i] = 1;
                printf("%12d: Yes\n",S[i]);
                break;
            }else if(S[i] < K[mid]){
                end = mid-1;
            }else{
                start = mid+1;
            }
        }
        if(hit[i] == 0)
            printf("%12d: No\n",S[i]);
    }
    gettimeofday(&endTime, NULL);
    struct  timeval result;
    timersub(&endTime, &startTime, &result);
    printf( "Time: %ld.%06ld\n", result.tv_sec, result.tv_usec );
    fclose(keysFile);
}


//DISK SEQUENTIAL
void diskSequential(char *keysFileName, int *S, int numSeeks){
    //Key File Pointer
    FILE *keysFile;
    //long keyFileSize;
    
    //Time computing structs
    struct timeval startTime;
    struct timeval endTime;
    
    //3. Create a third array of integers called hit of the same size as S. This array to record whether each seek value S[i] exists in K or not
    int hit[numSeeks], i;
    
    //Open the keys file
    keysFile = fopen(keysFileName , "rb");
    
    if(keysFile == NULL){
        printf("Key.db File not found");
        return;
    }
    
    //Start Time
    //You must record how much time it takes to to determine the presence or absence of each S[i] in key.db.
    gettimeofday(&startTime, NULL);
    
    //Sequential Search
    for(i = 0 ; i<numSeeks; i++){
        //rewind(keysFile);
        hit[i] = 0;
        fseek(keysFile, 0, SEEK_SET);
        int current;
        while(!feof(keysFile)){
            fread(&current, sizeof(int), 1, keysFile);
            if(current == S[i]){
                hit[i] = 1;
                printf("%12d: Yes\n",S[i]);
                break;
            }
        }
        if(hit[i] == 0)
            printf("%12d: No\n",S[i]);
    }
    
    gettimeofday(&endTime, NULL);
    struct  timeval result;
    timersub(&endTime, &startTime, &result);
    printf( "Time: %ld.%06ld\n", result.tv_sec, result.tv_usec );
    fclose(keysFile);
}

//DISK BINARY
void diskBinary(char *keysFileName, int *S, int numSeeks){
    //Key File Pointer
    FILE *keysFile;
    long keyFileSize;
    
    //Time computing structs
    struct timeval startTime;
    struct timeval endTime;
    
    //3. Create a third array of integers called hit of the same size as S. This array to record whether each seek value S[i] exists in K or not
    int hit[numSeeks];
    
    //Open the keys file
    keysFile = fopen(keysFileName , "rb");
    
    if(keysFile == NULL){
        printf("Key.db File not found");
        return;
    }
    
    //Calculating the number of the keys in file
    fseek(keysFile, 0L, SEEK_END);      //going to end of file
    keyFileSize = ftell(keysFile);      //then storing the file size
    const int numKeys = keyFileSize/sizeof(int);    //as it is mentioned, the file contains only integers
    
    //On-Disk Binary Search
    int start, mid , end, i = 0;
    
    //Start time
    //"You must record how much time it takes to to determine the presence or absence of each S[i] in key.db."
    gettimeofday(&startTime, NULL);
    
    for(i = 0 ; i<numSeeks; i++){
        hit[i] = 0;
        start = 0;
        end = numKeys-1;
        int current;
        while(start<=end){
            mid = (start + end) /2;
            fseek(keysFile, sizeof(int) * mid, SEEK_SET);
            fread(&current, sizeof(int), 1, keysFile);
            if(S[i] == current){
                hit[i] = 1;
                printf("%12d: Yes\n",S[i]);
                break;
            }else if(S[i] < current){
                end = mid-1;
            }else{
                start = mid+1;
            }
        }
        if(hit[i] == 0)
            printf("%12d: No\n",S[i]);
    }
    gettimeofday(&endTime, NULL);
    struct  timeval result;
    timersub(&endTime, &startTime, &result);
    printf( "Time: %ld.%06ld\n", result.tv_sec, result.tv_usec);
    fclose(keysFile);
}

//Helper function to read binary file integers and store them to array
void inMemoryFileRead(FILE* filePointer, int* array)
{
    //Setting the file pointer to the start of the file
    rewind(filePointer);       //or fseek(fp, 0L, SEEK_SET)
    
    //feof will return '0' till the end of the file
    while(!feof(filePointer)){
        fread(array, sizeof(int), 1, filePointer);
        array++;
    }
}
