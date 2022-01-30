// AMAN BILAIYA 2018CSB1069 //
// External sorting with the N-way merge algorithm readme //

###### HOW TO RUN ######
On g++ compiler use below commands.
$ g++ externalSort.cpp
$ ./a

NOTE : 
1. One can change the value of 'records', 'B' and 'M' [These variables are defined at the top of the code under macro statements as shown below]
#define records 200
#define B 45
#define M 3
#define N 2
2. One can change the order of merge (merging 2, 3, 4..M-1 runs) based on value of M using the 'N' variable.  
3. Intermediate runs .txt files are not deleted in code so that one can test the correctness of the code.
4. Delete the .txt files that were created if you want to test the code again with different 'records', 'B', 'M' and order of merge values. 
5. No external library is used in the code, only primitive data types are used like vector array, string, file I/O.

###### STEPS INVOLVED ######
1. Large synthetic table containing '$records' sales records of department stores is generated as "synTable.txt"
2. Then disk blocks are created as .txt file which cannot atmost '$B' records from the synthetic table [naming convention "1.txt", "2.txt"... soon]
3. Then follows the main exteranl sort algorithm :- 
    (i) All the intermediate runs (created during the sort process) are managed/stored as “simulated disk blocks” as .txt files [naming convention "runs1.txt", "runs2.txt",...soon]
    (ii) Finally,the output is produced in a .txt file which contains the records in the sorted order ["Output.txt"]


###### RESULT ######