#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "balloc.h"
#include "bbm.h"
#include "freelist.h"
#include "bbm.h"
#include "freelist.h"
#include "utils.h"

// Setting most amount of buddies that will exist
#define MAX_ORDER 10

// ? Likely need to create some sort of struct to store the buddies
// ? The buddy is used to store an address to a section on the "BitMap"
// ? and also will have a location stored for a "FreeList"
struct Buddy {

    // * UNSURE ON THIS SO FAR
    // ? A pointer to store location for the "BitMap"

    // ? A pointer to store a "Free List"
    
} typedef Buddy;

// The representation of a Balloc
// * I do not believe you create an actual struct of Balloc, that is supposed to be a void pointer (Could be wrong)
struct Rep {
    
    // A pointer that will be the pool
    void * pool;

    // Another item that will set the [0] l: lowest power, [1] u: highest power, [2] r: number of buddies (u - l + 1)

    // Another item that will set the [0] l: lowest power, [1] u: highest power, [2] r: number of buddies (u - l + 1)
    int managementData[3];

    // Something to hold the buddies that will be ranging from 0:r-1
    Buddy buddies[MAX_ORDER];

    // Size should probably be saved? likely in managementData
} typedef Rep;

// Creates a pool of memory that can be chunked off into
// blocks that are powers of 2 (Ex: 2^n, [0: 1, 1: 2, 2: 4, etc.]) and can
// then have parts of the memory segmented
// size = Given number of bytes to create the pool
// l = Determines the lowest possible allocation
// u = Determines the highest possible allocation
// Returns: Balloc, a void pointer to the struct
Balloc bcreate(unsigned int size, int l, int u) {

    // Used for keeping track of the size for the memory pool
    unsigned int requestedSize = 0, actualSize = 0;

    // Highest block size that memory can be allocated from the pool
    int highestAllocationSize = 0;

    // Keeping track of 'r', how many buddies will exist in the allocator
    int numberOfBuddies = 0;
    
    // Used for aliasing
    int lower = 0, upper = 0;

    requestedSize = size;
    lower = l;
    upper = u;

    // Checking for valid size call
    if (requestedSize < 0) {
        // Size is not a positive number

        // Output error message
        fprintf(stderr, "Invalid size call. Requires a nonzero positive number!");
        exit(1);
    }

    // Checking size of l (lowest) and u (highest) are valid
    if (lower < 0 || upper < 0) {
    if (lower < 0 || upper < 0) {
        // Lower or upper bounds is not a positive number

        // Output error message
        fprintf(stderr, "Invalid size constraints provided. l and u should be nonzero positive numbers!");
        exit(1);
    } else if (lower > upper) {
        exit(1);
    } else if (lower > upper) {
        // Lower bounds is greater than upper bounds (should not be possible)
        
        // Output error message
        fprintf(stderr, "Upper bound is less than lower bounds. That makes no sense...");
        exit(1);
        exit(1);
    }

    // Highest block size that can be created in this pool
    // by calling the utils e2size() method
    highestAllocationSize = e2size(upper);
    actualSize = e2size(size2e(size));

    // Checking if size will conflict with the lowest or highest bounds
    if (actualSize < highestAllocationSize) {

        // Output error message
        fprintf(stderr, "Will not be able to allocate blocks at the highest level provided. AKA, %d is too small for 2^%d (%d)", size, u, highestAllocationSize);
        exit(1);
    }

    // ? Creating Balloc struct to have the pool initialized (I believe this is the right way going around this)
    Rep newBalloc;

    // Mapping the memory in the address space to be used
    void * poolAddr = mmalloc(actualSize);

    // Save address into newBalloc for start of pool
    newBalloc.pool = poolAddr;

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
    
    // Creating the buddies and adding into newBalloc
    for (int i = lower; i <= upper; i++) {

    }

    // Saving an address to store the allocator
    // mmalloc() is very useful, it is very much like malloc() but you just need to keep track of the address
    void * ballocAddr = mmalloc(sizeof(Rep));

    // Copy the memory off the stack from newBalloc to an address (ballocAddr)
    memcpy(ballocAddr, &newBalloc, sizeof(Rep));

    // Returning the address of the created Balloc
    return ballocAddr;

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

// Grabs the size of the free memory block 
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

