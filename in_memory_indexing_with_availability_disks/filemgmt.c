//
//  filemgmt.c
//  ads_assignment1
//
//  Created by Khantil Choksi on 2/8/18.
//  Copyright © 2018 Khantil Choksi. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Primary Key Struct
typedef struct {
    int key; /* Record's key */
    long off; /* Record's offset in file */
    
} index_S;

//Availibility Hole Struct
typedef struct {
    int siz; /* Hole's size */
    long off; /* Hole's offset in file */
    
} avail_S;

int compareKeys (const void *a, const void *b){
    index_S *indexA = (index_S *)a;
    index_S *indexB = (index_S *)b;
    //printf("\n INSIDE COMPARE: 1: %d 2: %d\n",indexA->key, indexB->key);
    return (indexA->key - indexB->key);
}

//Ascending order
int compareHoleAsc (const void *a, const void *b){
    avail_S *holeA = (avail_S *)a;
    avail_S *holeB = (avail_S *)b;
    //printf("\n INSIDE COMPARE HOLES: 1: %d 2: %d\n",holeA->siz, holeB->siz);
    //If multiple holes have the same size, the entries for these holes should be sorted in ascending order of hole offset.
    if(holeA->siz == holeB->siz)
        return (holeA->off - holeB->off);
    
    return (holeA->siz - holeB->siz);
}

//Desceding order
int compareHoleDesc (const void *a, const void *b){
    avail_S *holeA = (avail_S *)a;
    avail_S *holeB = (avail_S *)b;
    //printf("\n INSIDE COMPARE HOLES: 1: %d 2: %d\n",holeA->siz, holeB->siz);
    //If multiple holes have the same size, the entries for these holes should be sorted in ascending order of hole offset.
    if(holeA->siz == holeB->siz)
        return (holeB->off - holeA->off);
    
    return (holeB->siz - holeA->siz);
}

//Reading Holes - In-Memory
int readAvailabilityHoles(FILE *fp, avail_S *array){
    //Calculating Number of Records
    fseek(fp, 0L, SEEK_END);      //going to end of file
    long holesSize = ftell(fp);      //then storing the file size
    
    int numberOfHoles = holesSize/sizeof(avail_S);
    
    fseek(fp, 0L, SEEK_SET);
    //fread(array, sizeof( index_S ), numberOfIndexKeys, fp );
    rewind(fp);
    while(!feof(fp)){
        fread(array, sizeof(avail_S), 1, fp);
        array++;
    }
    
    //Testing
    //printf("\n Number of primary keys present: %d",numberOfHoles);
//    if(numberOfIndexKeys > 0){
//        printf("\n KEY: %d",array[0].key);
//        printf("\n OFFSET: %ld",array[0].off);
//    }
    return numberOfHoles;
}

//Writing to Holes File
void writeHolesToFile(FILE *fp, avail_S *array, int size){
    int i = 0;
    for(; i< size;i++){
        //printf("\n Writing HOLE SIZE: %d",array[i].siz);
        fwrite(array, sizeof( avail_S ), 1, fp );
        array++;
    }
    
}

//Add hole to availability array
void addHoleAvailability(avail_S *array, int siz, long off, int holesCount){
    array[holesCount].siz = siz;
    array[holesCount].off = off;
    //quickSort(array, ++holesCount);
}

//For deletion of a hole  from holes availability array
void removeHoleAvailability(avail_S *array, int index, int totalHoles)
{
    int i;
    for(i = index; i < totalHoles - 1; i++)
    array[i] = array[i + 1];
}
void quickSortHolesAsc(avail_S *array, int size){
    qsort (array, size, sizeof(avail_S), compareHoleAsc);
}

void quickSortHolesDesc(avail_S *array, int size){
    qsort (array, size, sizeof(avail_S), compareHoleDesc);
}

void sortHolesAccordingly(char* argument, avail_S *array, int size)
{
    //Options
    char *firstFitCommand = "--first-fit";
    char *bestFitCommand = "--best-fit";
    char *worstFitCommand = "--worst-fit";
    
    if( strcmp(argument,firstFitCommand) == 0 ){
        //printf("First Fit\n");
       //Do Nothing
    }else if( strcmp(argument,bestFitCommand) == 0){
        
        //printf("Best Fit\n");
        quickSortHolesAsc(array,size);
    }else if( strcmp(argument, worstFitCommand) == 0){
        //printf("Worst Fit \n");
        quickSortHolesDesc(array, size);
    }else{
        printf("Wrong Command for Fit argument!\n");
    }
}

//First Fit Logic return index of available hole
int findHole(int requiredHoleSize, avail_S *array, int size){
    //int requiredHoleSize = recordSize + sizeof(int);
    int i = 0;
    for(; i<size; i++){
        if( requiredHoleSize <= array[i].siz ){
            return i;
        }
    }
    return -1;
}




void quickSortKeys(index_S *array, int size){
    qsort (array, size, sizeof(index_S), compareKeys);
}

//For deletion of a key index from indices array
void removePrimaryKeyIndex(index_S *array, int index, int totalKeys)
{
    int i;
    for(i = index; i < totalKeys - 1; i++)
        array[i] = array[i + 1];
}

//Reading Primary Key-Indexes - In-Memory
int readPrimaryKeyIndices(FILE *fp, index_S *array){
    //Calculating Number of Records
    fseek(fp, 0L, SEEK_END);      //going to end of file
    long indexKeysSize = ftell(fp);      //then storing the file size
    
    int numberOfIndexKeys = indexKeysSize/sizeof(index_S);    //as it is mentioned, the file contains only index_S
    
    fseek(fp, 0L, SEEK_SET);
    //fread(array, sizeof( index_S ), numberOfIndexKeys, fp );
    rewind(fp);
    while(!feof(fp)){
        fread(array, sizeof(index_S), 1, fp);
        array++;
    }
    
    //Testing
//    printf("\n Number of primary keys present: %d",numberOfIndexKeys);
//    if(numberOfIndexKeys > 0){
//        printf("\n KEY: %d",array[0].key);
//        printf("\n OFFSET: %ld",array[0].off);
//    }
    return numberOfIndexKeys;
}

//Writing to Primary Key-Indexes File
void writePrimaryKeyIndices(FILE *fp, index_S *array, int size){
    int i = 0;
    for(; i< size;i++){
        //printf("\n Writing KEY: %d",array[i].key);
        fwrite(array, sizeof( index_S ), 1, fp );
        array++;
    }
    
}

//Add key index to array
void addPrimaryKeyIndex(index_S *array, int key, long off, int keysCount){
    array[keysCount].key = key;
    array[keysCount].off = off;
    quickSortKeys(array, ++keysCount);
}

//Writing Record at end of file
int addRecordAtEndOfFile(FILE *fp, char* record){
    fseek(fp, 0L, SEEK_END);
    int endOfFileOffset = ftell(fp);
    
    //buf = "712412912|Pord|Pob|Phi";
    int recordSize = strlen(record);
    fwrite(&recordSize, sizeof(int), 1, fp);
    fwrite(record, strlen(record), 1, fp);
    
    return endOfFileOffset;
    
}

//Writing record at offset
void addRecordAtOffset(FILE *fp, long offset, char* record){
    fseek( fp, offset, SEEK_SET );
    int recordLength = strlen(record);
    fwrite(&recordLength, sizeof(int), 1, fp);
    fwrite(record, sizeof(index_S), 1, fp);
}

int binarySerach(index_S *array, int findKey, int totalKeys){
    int start = 0, mid , end;
    end = totalKeys-1;
    int foundIndex = -1;
    while(start<=end){
        mid = (start + end) /2;
        if(array[mid].key == findKey){
            //printf("KEY FOUND OFFSET: %ld and at INDEX: %d\n",array[mid].off, mid);
            //*offset = array[mid].off;
            foundIndex = mid;
            break;
        }else if(array[mid].key > findKey){
            end = mid-1;
        }else{
            start = mid+1;
        }
    }
    //if(foundIndex == -1)
      //  printf("KEY NOT FOUND\n");
    return foundIndex;
}

//Return Record Size too
char* directAccessRecord(FILE *fp, long offset){
    int recordSize = 0;
    char* record = NULL;
    
    fseek( fp, offset, SEEK_SET );
    
    fread( &recordSize, sizeof( int ), 1, fp );
    //printf("\n REC SIZE: %d",recordSize);
    record = malloc( recordSize );
    fread( record, 1, recordSize, fp );
    //printf("\n RECORD FOUND : %s\n",record);
    
    return record;
}

//Direct Access Record Size for deletion
int directAccessRecordSize(FILE *fp, long offset){
    int recordSize = 0;
    char* record = NULL;
    
    fseek( fp, offset, SEEK_SET );
    
    fread( &recordSize, sizeof( int ), 1, fp );
    //printf("\n REC SIZE: %d",recordSize);
    //record = malloc( recordSize );
    //fread( record, 1, recordSize, fp );
    //printf("\n RECORD FOUND : %s\n",record);
    
    return recordSize;
}

void printPrimaryIndices(index_S *array, int totalKeys){
    printf("Index:\n");
    for(int i = 0 ; i < totalKeys; i++){
        //printf(" Index: %d , Offset: %ld\n", array[i].key, array[i].off);
        printf( "key=%d: offset=%ld\n", array[i].key, array[i].off );
    }
    //printf("------\n");
}

void printHoles(avail_S *array, int totalHoles){
    printf("Availability:\n");
    int totalHoleSizeAvailable = 0;
    for(int i = 0 ; i < totalHoles; i++){
        //printf(" Hole Size: %d , Offset: %ld\n", array[i].siz, array[i].off);
        printf( "size=%d: offset=%ld\n", array[i].siz, array[i].off );
        totalHoleSizeAvailable += array[i].siz;
    }
    printf( "Number of holes: %d\n", totalHoles );
    printf( "Hole space: %d\n", totalHoleSizeAvailable );
    //printf("------\n");
}

int main(int argc, char** argv)
{
    
    //char *buf, *buf2, *fields, *inputCommand; /* Buffer to hold student record */
    FILE *studentDbFP; /* Input/output stream for student DB*/
    long rec_off; /* Record offset */
    int rec_siz; /* Record size, in characters */
    
    //Argv[1] fit options
    
    //Options

    char *endCommand = "end\n";
    char *addCommand = "add";
    char *findCommand = "find";
    char *deleteCommand = "del";
    
    //char buff[] = "712412913|Ford|Rob|Phi";
    //printf("size of record is: %lu", sizeof(buff));
    
    //buf = "712412913|Ford|Rob|Phi";
    //int recordSize = strlen(buf);
    
    int numberOfIndexKeys = 0;
    int numberOfHoles = 0;
    
    //Key - Index File
    index_S primaryKeyIndices[ 10000 ];  //Primary Index
    FILE *indexFP = NULL;
    if( ( indexFP = fopen( "index.bin", "rb" )) != NULL){
        //In-Memory Read
        numberOfIndexKeys = readPrimaryKeyIndices(indexFP, primaryKeyIndices);
        fclose(indexFP);
        printPrimaryIndices(primaryKeyIndices, numberOfIndexKeys);
    }
    
    //Availabiltiy - Hole File
    avail_S availableHoles[ 10000 ];  //Holes
    FILE *holesFP = NULL;
    if( ( holesFP = fopen( "holes.bin", "rb" )) != NULL){
        //In-Memory Read
        numberOfHoles = readAvailabilityHoles(holesFP, availableHoles);
        fclose(holesFP);
        printHoles(availableHoles, numberOfHoles);
    }
    
    //qsort (list, 6, sizeof(order), compare);
     /* Output file stream for index file*/
    ///index_S prim[ 10000 ]; /* Primary key index */
    
    //take student db in argument
    if ( ( studentDbFP = fopen( "student.db", "r+b" ) ) == NULL ){
        studentDbFP = fopen( "student.db", "w+b" );
    }
    
    char *commandLine = NULL, *command, *delimiter = " ", *record;
    size_t length;
    ssize_t numberOfBytesRead;
    //printf("Insert Input:");
    while ((numberOfBytesRead = getline(&commandLine, &length, stdin)) != -1) {
        //printf("Retrieved line of length %zu:\n Line: %s\n", numberOfBytesRead, commandLine);
        command = strtok(commandLine, delimiter);
        if(strcmp(command,addCommand) == 0){
            //printf("ADD Command\n");
            //Key
            command = strtok(NULL, delimiter);
            int primaryKey = atoi(command);
            
            //Checking if primary key already exists or not
            
            int foundIndex = binarySerach(primaryKeyIndices, primaryKey, numberOfIndexKeys);
            if(foundIndex != -1){
                printf("Record with SID=%d exists\n",primaryKey);
            }else{
                //Record
                record = strtok(NULL, "\n");
                
                //Testing
                //printf("Received Input Key: %d and Record: %s \n",primaryKey,record);
                
                //Insert record into student.db file
                
                //Fit Logic
                int requiredHoleSize = strlen(record)+sizeof(int);
                int foundAvailbleHoleIndex = findHole(requiredHoleSize, availableHoles, numberOfHoles);
                int offset;
                if(foundAvailbleHoleIndex == -1) {
                    //add record at end of file
                    offset = addRecordAtEndOfFile(studentDbFP, record);
                }
                else {
                    //Found a hole space
                    offset = availableHoles[foundAvailbleHoleIndex].off;
                    addRecordAtOffset(studentDbFP, offset, record); //Add record to file
                    
                    if(availableHoles[foundAvailbleHoleIndex].siz > requiredHoleSize){
                        //new small hole will be created
                        
                        
                        //Add the new hole entry from the hole availability array
                        int newHoleSize = availableHoles[foundAvailbleHoleIndex].siz - requiredHoleSize;
                        long newHoleOffset = offset + requiredHoleSize - 1;     //check this???????
                        addHoleAvailability(availableHoles, newHoleSize, newHoleOffset, numberOfHoles);
                        numberOfHoles++;
                        

                        //printf("\n Small new hole entry has been created of size: %d and at offset: %ld",newHoleSize, newHoleOffset);
                        
                    }else{
                        //no new hole will be created
                        //printf("\n No new hole entry has been created!");
                        
                    }
                    
                    //Remove the old hole entry from hole availability array
                    removeHoleAvailability(availableHoles, foundAvailbleHoleIndex, numberOfHoles);
                    numberOfHoles--;
                    
                    //Now according to option sort the availabilityHoles List
                    sortHolesAccordingly(argv[1], availableHoles, numberOfHoles);
                    
                }
                
                
                //Insert record entry into primary key index array
                addPrimaryKeyIndex(primaryKeyIndices, primaryKey, offset, numberOfIndexKeys);
                //Incrementing total number of index keys
                numberOfIndexKeys++;
            }
            //printPrimaryIndices(primaryKeyIndices, numberOfIndexKeys);
            //printHoles(availableHoles, numberOfHoles);
            
        }else if(strcmp(command, findCommand) == 0){
            //printf("FIND Command\n");
            
            //Key
            command = strtok(NULL, delimiter);
            int findKey = atoi(command);
            int foundIndex = binarySerach(primaryKeyIndices, findKey, numberOfIndexKeys);
            
            if(foundIndex == -1){
                printf("No record with SID=%d exists\n",findKey);
            }else{
                //Direct Access
                char* accessedRecord = directAccessRecord(studentDbFP, primaryKeyIndices[foundIndex].off);
                printf("%s\n",accessedRecord);
            }
            printPrimaryIndices(primaryKeyIndices, numberOfIndexKeys);
            
        } else if( strcmp(command, deleteCommand) == 0){
            //printf("DELETE Command\n");
            //Key
            command = strtok(NULL, delimiter);
            int deleteKey = atoi(command);
            int foundIndex = binarySerach(primaryKeyIndices, deleteKey, numberOfIndexKeys);
            if(foundIndex == -1){
                printf("No record with SID=%d exists\n",deleteKey);
            }else{
                //First create the hole
                int recordSize = directAccessRecordSize(studentDbFP, primaryKeyIndices[foundIndex].off);
                int holeSize = recordSize + sizeof(int);
                //printf(" RecordSize: %d, holeSize: %d\n",recordSize , holeSize);
                addHoleAvailability(availableHoles, holeSize, primaryKeyIndices[foundIndex].off, numberOfHoles);
                numberOfHoles++;
                
                //Now according to option sort the availabilityHoles List
                sortHolesAccordingly(argv[1], availableHoles, numberOfHoles);
                //printf(" Holes Sorted\n");
                
                //Remove from the primary key indices
                removePrimaryKeyIndex(primaryKeyIndices, foundIndex, numberOfIndexKeys);
                numberOfIndexKeys--;
            }
            //printPrimaryIndices(primaryKeyIndices, numberOfIndexKeys);
            //printHoles(availableHoles, numberOfHoles);
        } else if(strcmp(command, endCommand) == 0){
            printPrimaryIndices(primaryKeyIndices, numberOfIndexKeys);
            printHoles(availableHoles, numberOfHoles);
            indexFP = fopen( "index.bin", "w+b" );    /* Input-Output Stream for key index file*/
            writePrimaryKeyIndices(indexFP, primaryKeyIndices, numberOfIndexKeys);
            
            holesFP = fopen( "holes.bin", "w+b" );    /* Input-Output Stream for holes file*/
            writeHolesToFile(holesFP, availableHoles, numberOfHoles);
            
            //printf("\nEND Command BYE\n");
            break;
        }else{
            printf("\n Wrong Command TRY add / find / del / end!\n");
        }
        
            
    }
        
        
        
        //printf("STring = %s", line);

    
    //inputCommand = malloc(sizeof(char)*100);
    //scanf ("%[^\n]%*c", inputCommand);
    //scanf ("%a", inputCommand);
    //printf("\n Input command: %s",inputCommand);
    
    /* If student.db doesn't exist, create it, otherwise read its first record */
    //Uncomment following
    /*if ( ( fp = fopen( "student.db", "r+b" ) ) == NULL ){
        fp = fopen( "student.db", "w+b" );

        
    } else {
        rec_off = 0;
        fseek( fp, rec_off, SEEK_SET );
        fread( &rec_siz, sizeof( int ), 1, fp );
        printf("\n REC SIZE: %d",rec_siz);
        buf = malloc( rec_siz );
        fread( buf, 1, rec_siz, fp );
        printf("\n REC: %s",buf);
    }*/

    
    /*while( (fields = strsep(&buf, "|")) != NULL){
     printf("\n KEY Fields: %s",fields);
     prim[numberOfRecords].key = atoi(fields);
     prim[numberOfRecords].off = 0;
     break;
     }*/
    

    
    /*while(1){
        scanf ("%[^\n]%*c", inputCommand);
        
        if(strcmp(inputCommand, end) == 0){
            //write index files and availability list
            break;
        }
    }*/

    //Close all file pointers
    fclose( indexFP );
    fclose(studentDbFP);
    
    return 0;
    
}
