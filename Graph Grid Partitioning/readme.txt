// AMAN BILAIYA 2018CSB1069 //
// Grid partitioning of network graph //

###### HOW TO RUN ######
On g++ compiler use below commands.
$ g++ partitionGraph.cpp
$ ./a

NOTE : 
1. One can change the value of 'cellSize' and 'B' [These variables are defined at the top of the code under macro statements as shown below]
#define cellSize 10000 // cellsize=k for each k * k cell
#define entries 10000 // B
2. In the grid, cell ids are taken as (i,j) pair like we have in 2d array. [row, column]
3. Disk blocks are names as "Cell(id)_Block.txt" and overflow files as "Cell(id)_OverflowBlock1.txt", "Cell(id)_OverflowBlock2.txt" etc...
4. No external library is used in the code, only primitive data types are used like vector array, string, file I/O.

STEPS INVOVED :
1. Computing X_min, X_max, Y_min, Y_max from the dataset
2. Finding cell id given X, Y coordinates
3. Creating disk blocks for each cell and handling overflow blocks

IMPLEMENTED :
1. A map from node id → x and y coordinate
2. Function to determine the appropriate cell id given an x and y coordinates
3. A map between the cell ids and the file names corresponding to their disk blocks
4. Visualizer function which would print the disk block (and its associated overflow block) contents of any given node id in "visualizer.txt"
