//
//  assn_3.c
//  ads_assignment3
//
//  Created by Khantil Choksi on 3/12/18.
//  Copyright © 2018 Khantil Choksi. All rights reserved.
//

//assn_3 mergesort-method index-file sorted-index-file
/*
 Your program must support three different mergesort methods.
 
 1.    --basic    Split the index file into sorted runs stored on disk, then merge the runs to produce a sorted index file.
 2.    --multistep    Split the index file into sorted runs. Merge subsets of runs to create super-runs, then merge the super-runs to produce a sorted index file.
 3.    --replacement    Split the index file into sorted runs created using replacement selection, then merge the runs to produce a sorted index file.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>

//Read Keys
void readKeyBlocks(FILE *fp, int *buffer, int count){
    fread(buffer, sizeof(int), count, fp);
}

//Write Runs to disk
void writeKeys(FILE *fp, int *buffer, int count){
    fwrite( buffer, sizeof( int ), count, fp );
}

//Read next block from disk and returns number of integers actually read
int readBlock(FILE *fp, int *buffer, int start, int count){
    buffer = buffer+start;
    return fread(buffer, sizeof(int), count, fp);
}

//Print Buffer
void printBuffer(int* buffer, int start, int end){
    
    for(; start<end;start++){
        printf(" %d ::  %d\n",start,buffer[start]);
    }
}

//Comparator function for integer to be used in quick sort
int compare_int( const void* a, const void* b )
{
    if( *(int*)a == *(int*)b ) return 0;
    return *(int*)a < *(int*)b ? -1 : 1;
}

//Creating Runs and return total number of runs created i.e. runs = runs or runs = runs + 1
int create_runs(FILE *inputFP, int *buffer, long input_keys, char* inputBinFileName){
    //printf("\n Called CREATE_RUNS");
    int i = 0, remaining_keys;
    int runs = input_keys/1000;
    
    //Run File name e.g. input.bin.012
    char runFileName[1000];
    //To store e.g. "012"
    char currentRunName[4];
    
    FILE *runFP = NULL;
    
    for(i = 0; i<runs; i++){
        readKeyBlocks(inputFP, buffer, 1000);
        qsort(buffer, 1000, sizeof(int), compare_int);
        //printBuffer(input_buffer, 0, 1000);
        strcpy(runFileName,inputBinFileName);
        strcat(runFileName,".");
        snprintf(currentRunName, 4*sizeof(char), "%03d", i);
        strcat(runFileName,currentRunName);
        
        runFP = fopen( runFileName, "wb" );
        writeKeys(runFP, buffer, 1000 );
        fclose(runFP);
        
    }
    
    remaining_keys = input_keys - (runs*1000);
    if(remaining_keys > 0){
        //Create one more last run file which will have keys < 1000
        runs++;     //Total runs will be runs+1 for further use
        
        readKeyBlocks(inputFP, buffer, remaining_keys);
        qsort(buffer, remaining_keys, sizeof(int), compare_int);
        
        strcpy(runFileName,inputBinFileName);
        strcat(runFileName,".");
        snprintf(currentRunName, 4*sizeof(char), "%03d", i);
        strcat(runFileName,currentRunName);
        runFP = fopen( runFileName, "wb" );
        writeKeys(runFP, buffer,remaining_keys );
        fclose(runFP);
    }
    //printf("\n END OF Called CREATE_RUNS");
    return runs;
}

//Initial Load into input_buffer for all runs
void initial_load_runs(int runStart, int runEnd, FILE **runsFP, int *buffer, int blockSize, int *currentBufferRunPointers, int *runBufferReadCount, int *isRunExhausted, char* baseFileName)
{
    //printf("\n Called initial_load_runs");
    //Run File name e.g. input.bin.012
    char runFileName[1000];
    //To store e.g. "012"
    char currentRunName[4];
    
    int i = 0;
    int numberOfRuns = runEnd - runStart;
    for(i = 0; i<numberOfRuns; i++){
        strcpy(runFileName,baseFileName);
        strcat(runFileName,".");
        snprintf(currentRunName, 4*sizeof(char), "%03d", (runStart+i));
        strcat(runFileName,currentRunName);
        
        
        runsFP[i] = fopen( runFileName, "rb" );

        
        runBufferReadCount[i] = readBlock(runsFP[i], buffer, i*blockSize, blockSize);
        currentBufferRunPointers[i] = i*blockSize;
        isRunExhausted[i] = 0; //Default initially isRunExhausted will false bcz the run will have at-least one value
        
        //printf("\n LOADED FILENAME: %s",runFileName);
    }
    //printf("\n END OF Called initial_load_runs");
    
}

//Merge Runs
void merge_runs(int *input_buffer, int* output_buffer, FILE *sortedFP, FILE **runsFP, int runsToBeMerged, int blockSize, int *currentBufferRunPointers, int* runBufferReadCount, int* isRunExhausted){
    
    int minKey;
    int minRunNumber;
    int actualKeysRead;
    int outputBufferIndex = 0;
    
    
    //Testing Counter
    int testCounter = 0;
    
    int i = 0;
    
    while(1){       //Until all the runs are not exhausted
        
        testCounter++;
        //Find MIN so initializing with INT_MAX
        minKey = INT_MAX;
        
        for(i = 0; i <runsToBeMerged; i++){
            if(isRunExhausted[i] == 0){//If run is not exhausted
                
                if(currentBufferRunPointers[i] >= ( i*blockSize + runBufferReadCount[i] ) ){
                    //ith run's buffer has been used, so load next block for this run
                    if(runBufferReadCount[i] < blockSize){
                        //printf("\n RUN I: %d got exhausted",i);
                        isRunExhausted[i] = 1;
                        continue;
                    }else{
                        
                        actualKeysRead = readBlock(runsFP[i], input_buffer, i*blockSize, blockSize); //Run next block size keys
                        
                        if(actualKeysRead < blockSize){ //This means that end of file occured and this run has been exhausted
                            
                            runBufferReadCount[i] = actualKeysRead;
                            fclose(runsFP[i]);  //Close this sorted file
                            
                            //This mean that, we don't have any more keys left for ready from that sorted file run i.e. situation like file having 250 keys divided by 4
                            if(actualKeysRead == 0){
                                //No more keys
                                //printf("\n RUN I: %d got exhausted",i);
                                isRunExhausted[i] = 1;  //Run i exhausted set TRUE
                                continue;
                            }
                        }
                        
                        //Reset pointer
                        currentBufferRunPointers[i] = i*blockSize;
                    }
                    
                }
                
                //Condition  runBufferReadCount[i]!=0  added bcz if the case like 1000 divided by blocksize 4, at 250th readBlock it will read last 4 keys but won't set the isRunExhausted flag
                //But this condition is already handeled on top if(actualKeysRead == 0) so no need to worry about that here
                
                if (input_buffer[currentBufferRunPointers[i]] < minKey){
                    //printf("\n MINKEY : %d",minKey);
                    minKey = input_buffer[currentBufferRunPointers[i]];
                    //indexOfMinKey = currentBufferRunPointers[i];
                    minRunNumber = i;
                }
                
                
            }
        }
        
        
        //minKey will remain INT_MAX is all the runs are exhausted
        //This indicates we are done!
        if(minKey == INT_MAX){
            //printf("\n COMPLETED EXECUTION");
            break;
        }
        
        
        currentBufferRunPointers[minRunNumber] += 1;    //minRunNumber run buffer got used i.e. advancing pointer
        
        //Output buffer is exhausted so write it to output file
        if(outputBufferIndex >= (blockSize*runsToBeMerged)){
            //FOR BASIC MERGE SORT blocksize*runs = 1000
            //printf("\n WRITING : %d KEYS TO OUTPUT FILE",(blockSize*runsToBeMerged));
            writeKeys(sortedFP, output_buffer, (blockSize*runsToBeMerged));
            outputBufferIndex = 0;  //Indicates that output_buffer is empty
        }
        //printf("\n WROTE MINKEY %d to output buffer.",minKey);
        output_buffer[outputBufferIndex] = minKey;
        outputBufferIndex++;
        //printf("\n TEST COUNTER: %d",testCounter);
    }
    
    //Write the remaining output buffer
    if(outputBufferIndex > 0){
        //printf(" \n WRITING REMAINING : %d KEYS TO OUTPUT FILE",outputBufferIndex);
        writeKeys(sortedFP, output_buffer, outputBufferIndex);
    }
    
    //Testing
    
    //printf("\n TEST COUNTER: %d\n",testCounter);
}

void basic_merge_sort(int runs, FILE* inputFP, int* input_buffer, long input_keys, char* outputBinFileName, char* inputBinFileName){
    int blockSize;  //The amount of each run we can buffer
    
    FILE *sortedFP = NULL;
    
    //You must record how much time it takes to complete the basic mergesort. This includes run creation, merging, and writing the results to sort.bin.
    //Time computing structs
    struct timeval startTime;
    struct timeval endTime;
    struct  timeval result;
    
    //Output Buffer
    int output_buffer[1000];
    int i = 0;
    
    gettimeofday(&startTime, NULL);
    //Basic MergeSort
    //1. Create Runs
    runs = create_runs(inputFP, input_buffer, input_keys,inputBinFileName);
    
    
    //No need of input file now
    fclose( inputFP );
    
    //Calculating how many keys we can accomodate in input_buffer from each
    blockSize = 1000/runs;
    //printf("\n BLOCK SIZE: %d",blockSize);
    
    //Total Numbers of files created =  runs
    
    //Reference pointers to handle runs : All files pointers, each sub buffer pointer, pointer for checking run exhausted,
    FILE *runsFP[runs];
    int currentBufferRunPointers[runs];
    int isRunExhausted[runs];
    int runBufferReadCount[runs];
    
    
    //Initial load from all runs file into input buffer
    initial_load_runs(0,runs, runsFP, input_buffer, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted, inputBinFileName);
    
    /*printf("\n ----- INITIALLY LOADED BUFFER ---------- ");
     printBuffer(input_buffer, 0, 1000);*/
    
    //SORTED FILE
    sortedFP = fopen(outputBinFileName, "wb");
    
    //Merge Runs
    merge_runs(input_buffer, output_buffer, sortedFP, runsFP, runs, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted);
    
    //sortedFP = fopen("sorted.bin", "wb");
    fclose(sortedFP);
    
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &result);
    printf( "Time: %ld.%06ld\n", result.tv_sec, result.tv_usec );
    
    
    
//    //Print output file for testing
//    sortedFP = fopen("sorted.bin", "rb");
//    fseek(sortedFP, 0L, SEEK_END);      //Setting file pointer at the end of the file
//    long file_size = ftell(sortedFP);
//    input_keys = file_size/sizeof(int);
//    printf("\n Total OUTPUT Keys: %ld\n",input_keys);
//    fseek(sortedFP, 0L, SEEK_SET);    //Set input file pointer back to the beginning
//
//    runs = input_keys/1000;
//    printf("\n\n FINAL SORTED FILE ---------------------- \n\n");
//    //Basic MergeSort
//    for(i = 0; i<1; i++){
//        readKeyBlocks(sortedFP, input_buffer, 1000);
//        printf("\n RUN I: %d\n",i);
//        printBuffer(input_buffer, 0, 1000);
//    }
}

void multistep_merge_sort(int runs, FILE* inputFP, int* input_buffer, long input_keys, char* outputBinFileName, char* inputBinFileName){
    int blockSize;  //The amount of each run we can buffer
    
    FILE *sortedFP = NULL;
    
    //Run File name e.g. input.bin.012
    char runFileName[1000];
    char baseFileName[1000];
    strcpy(baseFileName, inputBinFileName);
    //To store e.g. "012"
    char currentRunName[4];
    
    //Output Buffer
    int output_buffer[1000];
    
    //Time computing structs
    struct timeval startTime;
    struct timeval endTime;
    struct  timeval result;
    
    gettimeofday(&startTime, NULL);
    //Multistep MergeSort
    //1. Create Runs
    runs = create_runs(inputFP, input_buffer, input_keys,inputBinFileName);
    //So this value will be 250000/1000
    
    //No need of input file now
    fclose( inputFP );
    
    //Total Numbers of files created =  runs
    
    int i = 0, lastRun = 0, j = 0;
    int runsToBeMerged = 15;    //This is the hard coded value given in the assignment
    //Calculating how many keys we can accomodate in input_buffer from each
    blockSize = 1000/runsToBeMerged;
    //printf("\n BLOCK SIZE: %d",blockSize);
    
    //Reference pointers to handle runs : All files pointers, each sub buffer pointer, pointer for checking run exhausted,
    FILE *runsFP[100];
    int currentBufferRunPointers[1000], isRunExhausted[1000] , runBufferReadCount[1000];
    //Here I am taking 1000 pointers so no need to worry about final merge e.g. in our case 17 way merge, but this is not gonna increase 1000
    

    if(runs>15){    //If generated runs are greater than 15
        for(i = 0; i<(runs/15);i++){
            
            //Implementations of multistep merge
            initial_load_runs((i*15), (i+1)*15, runsFP, input_buffer, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted, baseFileName);
            
            
            strcpy(runFileName,inputBinFileName);
            strcat(runFileName,".super.");
            snprintf(currentRunName, 4*sizeof(char), "%03d", i);
            strcat(runFileName,currentRunName);
            
            //SUPER RUN FILE
            sortedFP = fopen(runFileName, "wb");

            //So merging 15 runs to build one super run
            merge_runs(input_buffer, output_buffer, sortedFP, runsFP, runsToBeMerged, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted);
            
            fclose(sortedFP);
            
        }
        
        runsToBeMerged = (runs - (runs/15)*15);     //in our example 250 - 240 = 10
        
        if(runsToBeMerged > 0){
            
            //int startingRun =(runs/15)*15;
            
            blockSize = 1000/runsToBeMerged;     //So new block size will be 100
            //printf("\n NOW NEW blockSize IS : %d",blockSize);
            //(runs/15)*15 = 240 in the case for runs = 250
            initial_load_runs((runs/15)*15, runs, runsFP, input_buffer, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted, baseFileName);
            
            strcpy(runFileName,inputBinFileName);
            strcat(runFileName,".super.");
            snprintf(currentRunName, 4*sizeof(char), "%03d", i);
            strcat(runFileName,currentRunName);
            
            //SUPER RUN FILE
            sortedFP = fopen(runFileName, "wb");
            
            //Merge Runs
            merge_runs(input_buffer, output_buffer, sortedFP, runsFP, runsToBeMerged, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted);
            
            fclose(sortedFP);
            
        }
        
        
        runs = runs/15;
        if(runsToBeMerged>0)
            runs++;
        
        strcpy(baseFileName, inputBinFileName); //To handle
        strcat(baseFileName,".super");
    }
    
    //Final n-way merge which will have n << runs initially created e.g. in our case it became 17 instead of 250
    // If initially there are less than 15 files created input.bin.--- so the following will directly merge
    if(runs >0){
        //printf("\n Building final SORTED.bin RUNS: %d",runs);
        
        runsToBeMerged = runs;
        blockSize = 1000/runsToBeMerged;     //So new block size will be 1000/17 = 58.82
        //printf("\n OUT NOW blockSize IS : %d",blockSize);
        //(runs/15)*15 = 240 in the case for runs = 250
        
        //baseFileName will have value"input.bin.super." if runs > 15 otherwise it will have value "input.bin." bcz for runs<=15 we don't require to create super run file
        
        initial_load_runs(0, runs,runsFP, input_buffer, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted, baseFileName);

        //SUPER RUN FILE
        sortedFP = fopen(outputBinFileName, "wb");

        //Merge Runs
        merge_runs(input_buffer, output_buffer, sortedFP, runsFP, runsToBeMerged, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted);

        //sortedFP = fopen("sorted.bin", "wb");
        fclose(sortedFP);
    }
    
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &result);
    printf( "Time: %ld.%06ld\n", result.tv_sec, result.tv_usec );
    
//        //Print output file for testing
//        sortedFP = fopen("sorted.bin", "rb");
//        fseek(sortedFP, 0L, SEEK_END);      //Setting file pointer at the end of the file
//        long file_size = ftell(sortedFP);
//        input_keys = file_size/sizeof(int);
//        printf("\n Total OUTPUT Keys: %ld\n",input_keys);
//        fseek(sortedFP, 0L, SEEK_SET);    //Set input file pointer back to the beginning
//
//        runs = input_keys/1000;
//        printf("\n\n FINAL SORTED FILE ---------------------- \n\n");
//        //Basic MergeSort
//        for(i = 0; i<250; i++){
//            readKeyBlocks(sortedFP, input_buffer, 1000);
//            printf("\n RUN I: %d\n",i);
//            printBuffer(input_buffer, 0, 1000);
//        }
    
//End of Multistep Mergesort
}

void swap(int *array, int x, int y){
    int temp = array[x];
    array[x] = array[y];
    array[y] = temp;
}

void sift(int *array, int i, int n){
    int j, k, small;
    while(i < (n/2)){
        j = (i *2)+1;       //left child
        k = j+1;            //right child
        //small = i;
        if( k < n && array[k] <= array[j]){
            small = k;
        }else{
            small = j;
        }
        if( array[i] <= array[small] ){
            return;
        }
        swap(array, i, small);
        i = small;
    }
}

void heapify(int *array, int n){
    int i = (n/2);
    while (i>=0){
        sift(array, i, n);
        i--;
    }
}


void replacement_merge_sort(FILE* inputFP, int* input_buffer, char* outputBinFileName, char *inputBinFileName){
    FILE *runFP = NULL;
    //Output Buffer
    int output_buffer[1000], output_index = 0;
    int i = 0;
    
    int isInputFileExhausted = 0; //initially not exhausted
    
    //Run File name e.g. input.bin.012
    char runFileName[1000];
    //To store e.g. "012"
    char currentRunName[4];
    
    //Time computing structs
    struct timeval startTime;
    struct timeval endTime;
    struct timeval result;
    
    int indexH = 0, indexP=-1, indexB=750;
    //indexH is the index for the main heap -> won't be using anywhere
    //indexP is the index for the secondary heap
    //indexB is the index for the buffer
    int sizeH = 0 , sizeP = 0 , sizeB = 0 ;
    
    //2.1. Read the first 750 keys from input.bin into H, and the next 250 keys into B.
    
    gettimeofday(&startTime, NULL);
    //Initialize pointers
    
    
    sizeH = readBlock(inputFP, input_buffer, 0, 750);
    
    sizeB = readBlock(inputFP, input_buffer, 750, 250);
    
    //int testCounter = 0,testCounter2 = 0, reloadTestCounter = 3, eachRunCount = 0;
    int j;
    
    while(1){
        //printf("\n\n\n------------- STARTING NEW RUN I : %d \n",i);
        if(sizeH == 0 && sizeP == 0 & sizeB == 0){
            //If input.bin is empty, program will exit from here
            //Or all the heap, secondary heap and buffer have been exhausted
            //printf("\n Execution of Creating Runs have been completed succeessfully!");
            break;
        }
        indexH = 0;
        //indexB = 750;
        indexP = -1;
        sizeP = 0;
        output_index = 0;
        //eachRunCount = 0;
        
        strcpy(runFileName,inputBinFileName);
        strcat(runFileName,".");
        snprintf(currentRunName, 4*sizeof(char), "%03d", i);
        strcat(runFileName,currentRunName);
        runFP = fopen( runFileName, "wb" );
        heapify(input_buffer, sizeH);
        
        while(sizeH>0){ //Till primary heap won't be empty
            
            //2.2 Rearrange H so it forms an ascending heap.
            //heapify(input_buffer, sizeH);
            sift(input_buffer, 0, sizeH);
            //testCounter2++;
            //3.1 Append H1 (the smallest value in the heap) to the current run, managed through the output buffer.
            if(output_index >= 1000){
                //printf("\n Writing %d keys to run file for RUN: %d \n",1000,i);
                writeKeys(runFP, output_buffer, 1000 );
                //printBuffer(input_buffer, 0, 1000);
//                testCounter += 1000;
//                eachRunCount+= 1000;
                output_index = 0;
            }
            output_buffer[output_index] = input_buffer[0];
            output_index++;
            
            if(sizeB == 0 && isInputFileExhausted == 0){
                //reload buffer from the input.bin
                
                sizeB = readBlock(inputFP, input_buffer, 750, 250);
                //reloadTestCounter++;
                //printf("\n Re-Loading Buffer: %d succeessfully loaded for RUN: %d",sizeB,i);
                indexB = 750;
                
                if (sizeB < 250){
                    isInputFileExhausted = 1;
                    //printf("\n Input File has been exhausted! At Test Counter: %d",testCounter);
                    fclose(inputFP);
                }
            }
            
            //So if the input file has been exhausted, sizeB will remain 0; which indicates everytime H[sizeH] will be relaced to H[1].
            
            //3.2 Use replacement selection to determine where to place B1
            
            
            if( sizeB > 0 && input_buffer[0] <= input_buffer[indexB]){   //If H1 ≤ B1, replace H1 with B1.
                
                input_buffer[0] = input_buffer[indexB];
                
                
            }else{
                //If H1 > B1, replace H1 with H750, reducing the size of the heap by one.
                input_buffer[0] = input_buffer[sizeH-1];
                sizeH--;
                
                //Replace H750(P1) with B1, increasing the size of the secondary heap by one.
                if(sizeB > 0){
                    indexP = sizeH;   //i.e. P1
                    input_buffer[indexP] = input_buffer[indexB];
                    sizeP++;
                }
                
                
            }
            //Every time one element will be removed from the buffer that's for sure
            if(sizeB > 0){
                indexB++;
                sizeB--;
            }
            
        }
        
        
        
        //If the current program is here, it means that H is empty and current run is over
        if(output_index > 0){
            //printf("\n Writing Remaining %d keys to run file!",(output_index));
            writeKeys(runFP, output_buffer, (output_index) );
            //printBuffer(input_buffer, 0, (output_index+1));
            //testCounter += (output_index);
            //eachRunCount+= (output_index);
            output_index = 0;
        }
        //This run has been completed
        //printf("\n RUN I : %d has been completed! Total keys for run: %d\n-------------------\n",i,eachRunCount);
        fclose(runFP);
        i++;
        
        //The secondary heap will be full, so it replaces H, and a new run is started
        //If sizeH == 0 and sizeB ==0 and isInputFileExhauseted == 1 then you have to copy P1 to sizeP to H1 to sizeH.
//        if(sizeH == 0 && sizeB == 0 && isInputFileExhausted == 1){
//            indexH = 0;
//            if(sizeP == 0){
//                //Execution of Creating Runs have been completed succeessfully!
//                printf("\n Execution of Creating Runs have been completed succeessfully!");
//                break;
//            }else{
//                if(indexP != -1){ //or sizeP != 0
//                    for(;indexP<sizeP;indexP++,indexH++){
//                        input_buffer[indexH] = input_buffer[indexP];
//                    }
//                }
//
//            }
//
//        }
        
        if(indexP != 0){
            //printf("\n Secondary heap is not automatically replaced with primary heap");
            indexH = 0;
            
            for(j = 0;j<sizeP;j++,indexP++,indexH++){
                input_buffer[indexH] = input_buffer[indexP];
            }
        }
        //printf("\n RUN: %d => indexP: %d => sizeP: %d",i, indexP, sizeP);
        sizeH = sizeP;  //For last run, sizeP < 750! Otherwise it will always be 750.
    }
    
    //printf("\n\n --------- TOTAL RUNS CREATED: %d",i);
    //printf("\n\n --------- TOTAL KEYS Scanned %d TEST COUNTER2: %d, reloadTestCounter: %d",testCounter,testCounter2,reloadTestCounter);

    int runs = i;

    //Calculating how many keys we can accomodate in input_buffer from each
    int blockSize = 1000/runs;
    //printf("\n BLOCK SIZE: %d",blockSize);

    //Total Numbers of files created =  runs

    //Reference pointers to handle runs : All files pointers, each sub buffer pointer, pointer for checking run exhausted,
    FILE *runsFP[runs];
    int currentBufferRunPointers[runs], isRunExhausted[runs] , runBufferReadCount[runs];


    //Initial load from all runs file into input buffer
    initial_load_runs(0,runs, runsFP, input_buffer, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted, inputBinFileName);

    /*printf("\n ----- INITIALLY LOADED BUFFER ---------- ");
     printBuffer(input_buffer, 0, 1000);*/

    //SORTED FILE
    FILE *sortedFP = fopen(outputBinFileName, "wb");

    //Merge Runs
    merge_runs(input_buffer, output_buffer, sortedFP, runsFP, runs, blockSize, currentBufferRunPointers, runBufferReadCount, isRunExhausted);

    //sortedFP = fopen("sorted.bin", "wb");
    fclose(sortedFP);
    
    gettimeofday(&endTime, NULL);
    timersub(&endTime, &startTime, &result);
    printf( "Time: %ld.%06ld\n", result.tv_sec, result.tv_usec );

    
    

//    //    //Print output file for testing
//    sortedFP = fopen("sorted.bin", "rb");
//    fseek(sortedFP, 0L, SEEK_END);      //Setting file pointer at the end of the file
//    long file_size = ftell(sortedFP);
//    long input_keys = file_size/sizeof(int);
//    printf("\n------$$$$$$$---------- Total OUTPUT Keys: %ld\n",input_keys);
//    fseek(sortedFP, 0L, SEEK_SET);    //Set input file pointer back to the beginning
////
////    runs = input_keys/1000;
////    printf("\n\n FINAL SORTED FILE ---------------------- \n\n");
////    //Basic MergeSort
////    for(i = 0; i<runs; i++){
////        readKeyBlocks(sortedFP, input_buffer, 1000);
////        printf("\n RUN I: %d\n",i);
////        printBuffer(input_buffer, 0, 1000);
////    }
//    fclose(sortedFP);
//
//    //    //Print output file for testing
//    strcpy(runFileName,"input.bin.");
//    snprintf(currentRunName, 4*sizeof(char), "%03d", (runs-1));
//    strcat(runFileName,currentRunName);
//    runFP = fopen( runFileName, "rb" );
//    fseek(runFP, 0L, SEEK_END);      //Setting file pointer at the end of the file
//    file_size = ftell(runFP);
//    input_keys = file_size/sizeof(int);
//    printf("\n RUN FILE: %s",runFileName);
//    fseek(runFP, 0L, SEEK_SET);    //Set input file pointer back to the beginning
//
//    runs = input_keys/1000;
//
//    //Basic MergeSort
//    for(i = 0; i<runs; i++){
//        readKeyBlocks(sortedFP, input_buffer, 1000);
//        printf("\n RUN I: %d\n",i);
//        printBuffer(input_buffer, 0, 1000);
//    }
//    printf("\n input_keys: %d",input_keys);
//    printf("\n (input_keys/1000)*runs: %d",(input_keys/1000)*runs);
//
//    if(input_keys > (input_keys/1000)*runs){
//        int remaining_keys = input_keys - 1000*runs;
//        readKeyBlocks(runFP, input_buffer, remaining_keys);
//        printf("\n remaining_keys: %d",remaining_keys);
//        printf("\n Last RUN: %d\n",runs);
//
//        printBuffer(input_buffer, 0, remaining_keys);
//    }
//
//    fclose(runFP);
    
    
    
}

int main(int argc, char** argv)
{
    //Input Buffer
    int input_buffer[1000];
    //Input Keys
    long input_keys, file_size;

    int i;
    
    FILE *inputFP = NULL;
    
    //Number of RUNS
    int runs;
    
    char* inputBinFileName = argv[2];
    char* outputBinFileName = argv[3];
    //char inputFile[1000] =
    
    if( ( inputFP = fopen( inputBinFileName, "rb" )) != NULL){
        //Read 1000 indexes
        //readKeyBlocks(inputFP, input_buffer, 1000);
        
        fseek(inputFP, 0L, SEEK_END);      //Setting file pointer at the end of the file
        file_size = ftell(inputFP);
        input_keys = file_size/sizeof(int);   //Total input keys present in the input.bin file
        //printf(" Total Input Keys: %ld\n",input_keys);
        fseek(inputFP, 0L, SEEK_SET);    //Set input file pointer back to the beginning
        
        //Options
        char *basicCommand = "--basic";
        char *multiStepCommand = "--multistep";
        char *replacementCommand = "--replacement";
        
        if( strcmp(argv[1],basicCommand) == 0 ){
            //printf("Basic Merge Sort\n");
            basic_merge_sort(runs, inputFP, input_buffer, input_keys,outputBinFileName,argv[2]);
        }else if( strcmp(argv[1],multiStepCommand) == 0){
            
            //printf("Multistep Merge Sort\n");
            multistep_merge_sort(runs, inputFP, input_buffer, input_keys,outputBinFileName,inputBinFileName);
            
        }else if( strcmp(argv[1],replacementCommand) == 0){
//            //printf("Replacement Fit \n");
//            int a[7] = {8,6,5,7,9,1,2};
//            heapify(a, 7);
//            printf("\n HEAP: ");
//            for(i = 0; i< 7; i++){
//                printf(" :: %d",a[i]);
//            }
            replacement_merge_sort(inputFP, input_buffer,outputBinFileName,inputBinFileName);
            
        }else{
            printf("Wrong Command for Merge Sort!\n");
            
        }
    
        
    }
  
    
    //printf( "Time: %ld.%06ld", exec_tm.tv_sec, exec_tm.tv_usec );
    
    return 0;
    
}


