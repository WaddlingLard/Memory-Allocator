#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "balloc.h"
#include "bbm.h"
#include "freelist.h"
#include "utils.h"

// Memory is allocated by powers of two so this is the initial start
static const int startingByteSize = 2;

// Used to detect if calling mmalloc failed (mmap returns (void *) -1 if failed mapping)
static const void * failedMap = (void *) -1;

// The representation of a Balloc
// * I do not believe you create an actual struct of Balloc, that is supposed to be a void pointer (Could be wrong)
typedef struct Rep {
    
    // A pointer that will be the pool
    void * pool;

    // Another item that will set the [0] l: lowest power, [1] u: highest power, [2] r: number of buddies (u - l + 1)
    int managementData[3];

    // Something to hold the buddies that will be ranging from 0:r-1
    // ^ This comment makes sense as r will be on basis of 0-index

    // Size should probably be saved? likely in managementData
} *Rep;

// ? Likely need to create some sort of struct to store the buddies
// ? The buddy is used to store an address to a section on the "BitMap"
// ? and also will have a location stored for a "FreeList"
typedef struct Buddy {


    // * UNSURE ON THIS SO FAR
    // ? A pointer to store location for the "BitMap"


    // ? A pointer to store a "Free List"

    
} *Buddy;

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

    // Keeping track of 'r', how many buddies will exist in the allocator
    int numberOfBuddies;
    
    // Used for aliasing
    int lower, upper;

    requestedSize = size;
    lower = l;
    upper = u;

    // Checking for valid size call
    // * MIGHT CHANGE
    if (requestedSize < 0) {
        // Size is not a positive number

        // ? Output an error message, maybe use ERROR.h?
        // * For now, outputting an exit code along the stderr message is the solution
        fprintf(stderr, "Invalid size call. Requires a nonzero positive number!");
        exit(1);
    }

    // Checking size of l (lowest) and u (highest) are valid
    if (lower < 0 || upper < 0) {
        // Lower or upper bounds is not a positive number

        // ? Output error message, maybe use ERROR.h?
        // * For now, outputting an exit code along the stderr message is the solution
        fprintf(stderr, "Invalid size constraints provided. l and u should be nonzero positive numbers!");
        exit(1);
    } else if (lower > upper) {
        // Lower bounds is greater than upper bounds (should not be possible)
        
        // ? Output error message, maybe use ERROR.h?
        // * For now, outputting an exit code along the stderr message is the solution
        fprintf(stderr, "Upper bound is less than lower bounds. That makes no sense...");
        exit(1);
    }

    // Highest block size that can be created in this pool
    highestAllocationSize = pow(startingByteSize, upper);
    actualSize = pow(startingByteSize, ceil(log2(size)));

    // Checking if size will conflict with the lowest or highest bounds
    if (actualSize < highestAllocationSize) {

        // ? Output error message, maybe use ERROR.h?
        // * For now, outputting an exit code along the stderr message is the solution
        fprintf(stderr, "Will not be able to allocate blocks at the highest level provided. AKA, %d is too small for 2^%d (%d)", size, u, highestAllocationSize);
        exit(1);
    }

    // ? Creating Balloc struct to have the pool initialized (I believe this is the right way going around this)
    struct Rep newBalloc;

    // Mapping the memory in the address space to be used
    void * poolAddr = mmalloc(actualSize);

    // Verifying that the mapping worked, -1 if it failed
    if (poolAddr == failedMap) {

        // ? Output error message, maybe use ERROR.h?
        // * For now, outputting an exit code along the stderr message is the solution
        fprintf(stderr, "Memory mapping failed!");
        exit(1);
    } else {
        
        // Save address into newBalloc for start of pool
        newBalloc.pool = poolAddr;
    }

    // Getting number of buddies for the pool
    // The +1 is there for the [0:r-1] constraint
    // Ex: 6 - 3 = 3, but you need 4 buddies 
    // (3, 4, 5, 6) ==> [0, 1, 2 (stops because r-1 = 2)]
    numberOfBuddies = upper - lower + 1;
    
    // Storing management data information into the newBalloc

    // Lower, Upper, and Number of Buddies
    newBalloc.managementData[0] = lower;
    newBalloc.managementData[1] = upper;
    newBalloc.managementData[2] = numberOfBuddies;






    // Returning the address of the created Balloc
    // * NOTE: THE CURRENT SYSTEM NEEDS TO BE REDONE IN SOME WAY
    // * BY FIGURING OUT HOW TO STORE THE REPRESENATION OF THE
    // * BALLOC STRUCT
    // * Is this valid?
    void * ballocAddr = &newBalloc;
    return ballocAddr;

    // * For now
    // return NULL;

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

