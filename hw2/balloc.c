#include <stdio.h>
#include <math.h>

#include "balloc.h"
#include "utils.h"

// Memory is allocated by powers of two so this is the initial start
static const int startingByteSize = 2;

// The representation of a Balloc
// * I do not believe you create an actual struct of Balloc, that is supposed to be a void pointer (Could be wrong)
typedef struct Rep {
    // a pointer that will be the pool
    void * pool;
    // another item that will set the [0] l: lowest power, [1] u: highest power, [2] r: ?
    int managementData[3];
    // something to hold the buddies that will be ranging from 0:r-1
    // size should probably be saved? likely in managementData
} *Rep;

// Creates a pool of memory that can be chunked off into
// blocks that are powers of 2 (Ex: 2^n, [0: 1, 1: 2, 2: 4, etc.]) and can
// then have parts of the memory segmented
// size = Given number of bytes to create the pool
// l = Determines the lowest possible allocation (Ex: 2^1 = 2 is the lowest)
// u = Determines the highest possible allocation ()
Balloc bcreate(unsigned int size, int l, int u) {

    // Used for keeping track of the size for the memory pool
    unsigned int requestedSize, actualSize;

    // Highest block size that memory can be allocated from the pool
    int highestAllocationSize;

    requestedSize = size;

    // Checking for valid size call
    // * MIGHT CHANGE
    if (requestedSize < 0) {
        // Size is not a positive number

        // ? Output an error message, maybe use ERROR.h?
        // * Seems to be okay for now
        fprintf(stderr, "Invalid size call. Requires a nonzero positive number!");
    }

    // Checking size of l (lowest) and u (highest) are valid
    if (l < 0 || u < 0) {
        // Lower or upper bounds is not a positive number

        // ? Output error message, maybe use ERROR.h?
        // * Seems alright to have for now
        fprintf(stderr, "Invalid size constraints provided. l and u should be nonzero positive numbers!");
    } else if (l > u) {
        // Lower bounds is greater than upper bounds (should not be possible)
        
        // ? Output error message, maybe use ERROR.h?
        // * Seems ok for now
        fprintf(stderr, "Upper bound is less than lower bounds. That makes no sense...");
    }

    // Highest block size that can be created in this pool
    highestAllocationSize = pow(startingByteSize, u);
    actualSize = pow(startingByteSize, ceil(log2(size)));

    // Checking if size will conflict with the lowest or highest bounds
    if (actualSize < highestAllocationSize) {

        // ? Output error message, maybe use ERROR.h?
        // * Seems ok for now
        fprintf(stderr, "Will not be able to allocate blocks at the highest level provided. AKA, %d is too small for the power 2^%d(%d)", size, u, highestAllocationSize);
    }

    // Mapping the memory in the address space to be used
    mmalloc(actualSize);
    

    

    // Returning the created Balloc
    //return newBalloc;

    // * For now
    return NULL;
}

// Deletes the pool of memory that was created
// pool = A Balloc struct that contains the memory map
void bdelete(Balloc pool) {

    return;
}

// Allocates memory from a pool using the Buddy System Algorithm 
// pool = A Balloc struct that contains the memory map
// size = A number of bytes that is requested to be allocated in the pool
// Returns: void *, the address where the allocation was initiated
void * balloc(Balloc pool, unsigned int size) {

    return 0;
}

// Frees the block of memory that is allocated 
// pool = A Balloc struct that contains the memory map
// mem = A void pointer that is pointing at the block of memory to be deallocated
void bfree(Balloc pool, void *mem) {

    return;
}

// Grabs the size of the allocated memory block 
// pool = A Balloc struct that contains the memory map
// mem = A void pointer that is pointing at the block of memory to grab its size
unsigned int bsize(Balloc pool, void *mem) {

    return 0;
}

// A tool to output a text representation of the memory pool to stdout
// which is primarily a tool for debugging
// pool = A Balloc struct that contains the memory map
void bprint(Balloc pool) {

    return;
}

