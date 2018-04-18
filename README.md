# Searching-and-Sorting-Data-Structures-Implementations

## 1. In-Memory vs. Disk-Based Searching
  * **Linear search:** A sequential search of K for a key that matches the current seek value.
  * **Binary search:** A binary search through a sorted list of keys K for a key that matches the current seek value si. The fact that the keys are sorted allows approximately half the remaining keys to be ignored from consideration during each step of the search.  
  * Each of the two searches (linear and binary) will be performed in two different environments.  
      * In the first, the key list K will be held completely in memory.   
      * In the second, individual elements ki ∈ K will read from disk as they are needed.  
        
  * **Program Execution:**  
  
  ```assn_1 search-mode keyfile-name seekfile-name```  
  There are four different search modes supported:  
* `--mem-lin`    Read the key file into memory. Perform a lin­ear search for each seek element si in the seek file.    
* `--mem-bin`    Read the key file into memory. Perform a binary search for each seek element si in the seek file.  
* `--disk-lin`   Read each ki from the key file as it is needed. Per­form a lin­ear search for each seek element si in the seek file.  
* `--disk-bin`   Read each ki from the key file as it is needed. Per­form a bi­nary search for each seek element si in the seek file.  

------------------
## 2.  In-Memory Indexing with Availability Lists





----------------
## 3. Disk-Based Mergesort


--------------
## 4.  B-Trees

----------
Resources:
* CSC NCSU Course CSC 541 - Advanced Data Structures - https://www.csc2.ncsu.edu/faculty/healey/csc541/
