#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "balloc.h"
#include "freelist.h"
#include "utils.h"

// The representation of a Balloc
struct Rep
{

    // A pointer that will be the pool
    void *pool;

    // Another item that will set the [0] l: lowest power, [1] u: highest power, [2] r: number of buddies (u - l + 1)
    int managementData[3];

    // Something to hold the buddies that will be ranging from 0:r-1
    FreeList freeList;

    // Size is saved as well
    int size;

} typedef Rep;

// Creates a pool of memory that can be chunked off into
// blocks that are powers of 2 (Ex: 2^n, [0: 1, 1: 2, 2: 4, etc.]) and can
// then have parts of the memory segmented
// size = Given number of bytes to create the pool
// l = Determines the lowest possible allocation
// u = Determines the highest possible allocation
// Returns: Balloc, a void pointer to the struct
Balloc bcreate(unsigned int size, int l, int u)
{

    // Used for keeping track of the size for the memory pool
    const unsigned int requestedSize = size;
    unsigned int actualSize;

    // Highest block size that memory can be allocated from the pool
    int highestAllocationSize;

    // Keeping track of 'r', how many buddies will exist in the allocator
    int numberOfBuddies;

    // Used for aliasing
    const int lower = l, upper = u;

    // Checking for valid size call
    if (requestedSize <= 0)
    {
        // Size is not a positive number

        // Output error message
        fprintf(stderr, "Invalid size call. Requires a nonzero positive number!");
        exit(1);
    }

    // Checking size of l (lowest) and u (highest) are valid
    if (lower < 0 || upper < 0)
    {
        // Lower or upper bounds is not a positive number

        // Output error message
        fprintf(stderr, "Invalid size constraints provided. l and u should be nonzero positive numbers!");
        exit(1);
    }
    else if (lower > upper)
    {
        // Lower bounds is greater than upper bounds (should not be possible)

        // Output error message
        fprintf(stderr, "Upper bound is less than lower bounds. That makes no sense...");
        exit(1);
    }

    // Highest block size that can be created in this pool
    // by calling the utils e2size() method
    highestAllocationSize = e2size(upper);
    actualSize = e2size(size2e(size));

    // Checking if size will conflict with the lowest or highest bounds
    if (actualSize < highestAllocationSize)
    {

        // Output error message
        fprintf(stderr, "Will not be able to allocate blocks at the highest level provided. AKA, %d is too small for 2^%d (%d)", size, u, highestAllocationSize);
        exit(1);
    }

    // Creating Balloc struct to have the pool initialized
    Rep newBalloc;

    // Mapping the memory in the address space to be used
    void *poolAddr = mmalloc(actualSize);

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

    // Storing size
    newBalloc.size = actualSize;

    // Adding the freelist to the newBalloc
    newBalloc.freeList = freelistcreate(actualSize, lower, upper, poolAddr);

    // Saving an address to store the allocator
    // mmalloc() is very useful, it is very much like malloc() but you just need to keep track of the address
    void *ballocAddr = mmalloc(sizeof(Rep));

    // Copy the memory off the stack from newBalloc to an address (ballocAddr)
    memcpy(ballocAddr, &newBalloc, sizeof(Rep));

    // Returning the address of the created Balloc
    return ballocAddr;
}

// Deletes the pool of memory that was created
// pool = A Balloc struct that contains the memory map
void bdelete(Balloc pool)
{

    // Verify pool
    if (!pool)
    {
        // Pool does not exist

        // Ouputting error message
        fprintf(stderr, "Pool does not exist!");
        exit(1);
    }

    // Grabbing representation of pool
    const Rep *ballocPool = (Rep *)pool;

    // Grabbing freelist
    const FreeList list = ballocPool->freeList;

    // Grabbing mapped pool
    void *poolAddr = ballocPool->pool;

    // Grabbing lower/upper bounds and the size
    const int lower = ballocPool->managementData[0];
    const int upper = ballocPool->managementData[1];
    const int size = ballocPool->size;

    // Unmapping the freelist
    freelistdelete(list, lower, upper);

    // Unmapping the pool
    mmfree(poolAddr, size);

    // Undoing the memcpy from creation of allocator
    // * maybe don't actually need this
    // free(pool);

    // Allocator has been deleted!
    return;
}

// Allocates memory from a pool using the Buddy System Algorithm
// pool = A Balloc struct that contains the memory map
// size = A number of bytes that is requested to be allocated in the pool
// Returns: void *, the address where the allocation was initiated
void *balloc(Balloc pool, unsigned int size)
{

    // Used for keeping track of the size
    const int requestedSize = size;
    int actualSizeE;

    // Check parameters
    if (size < 1)
    {
        // Size requested is nothing

        // Outputting error message
        fprintf(stderr, "Size requested is not valid (must be positive integer)");
        exit(1);
    }
    else
    {
        // Normalize size exponent
        actualSizeE = size2e(requestedSize);
    }

    // Verify pool
    if (!pool)
    {
        // Pool does not exist

        // Ouputting error message
        fprintf(stderr, "Pool does not exist!");
        exit(1);
    }

    // Grabbing representation of pool
    const Rep *ballocPool = (Rep *)pool;

    // Accessing the freelist to allocate
    const FreeList list = ballocPool->freeList;

    // Grabbing base address of pool to allocate memory
    void *poolAddr = ballocPool->pool;

    // Grabbing lower constraint
    const int lower = ballocPool->managementData[0];

    // Calling the freelist to allocate the memory
    // * what if cannot allocate?
    void *allocatedSpot = freelistalloc(list, poolAddr, actualSizeE, lower);

    return allocatedSpot;
}

// Frees the block of memory that is allocated
// pool = A Balloc struct that contains the memory map
// * mem = A void pointer that is pointing at the block of memory to be deallocated
void bfree(Balloc pool, void *mem)
{

    // Verify pool
    if (!pool)
    {
        // Pool does not exist

        // Outputting error message
        fprintf(stderr, "Pool does not exist!");
        exit(1);
    }

    // Verify memory address to free
    if (!mem)
    {
        // Memory is not at a valid location

        // Outputting error message
        fprintf(stderr, "Memory is not a valid address");
        exit(1);
    }

    // Grab the representation of the pool
    const Rep *ballocPool = (Rep *)pool;

    // Accessing the freelist to allocate
    const FreeList list = ballocPool->freeList;

    // Grabbing base address of pool to allocate memory
    void *poolAddr = ballocPool->pool;

    // Grabbing lower constraint
    const int lower = ballocPool->managementData[0];

    // Getting size of the block
    unsigned int sizeOfBlock = bsize(pool, mem);

    // Freeing the block
    // * MAKE SURE TO VERIFY IT IS ALLOCATED ALREADY
    // * What if cannot free?
    freelistfree(list, poolAddr, mem, size2e(sizeOfBlock), lower);

    // Block is freed!
    return;
}

// Grabs the size of the memory block
// pool = A Balloc struct that contains the memory map
// * mem = A void pointer that is pointing at the block of memory to grab its size
unsigned int bsize(Balloc pool, void *mem)
{

    // Verify pool
    if (!pool)
    {
        // Pool does not exist

        // Outputting error message
        fprintf(stderr, "Pool does not exist!");
        exit(1);
    }

    // Verify memory address
    if (!mem)
    {
        // Memory is not at a valid location

        // Outputting error message
        fprintf(stderr, "Memory is not a valid address");
        exit(1);
    }

    // Grab the representation of the pool
    const Rep *ballocPool = (Rep *)pool;

    // Accessing the freelist to allocate
    const FreeList list = ballocPool->freeList;

    // Grabbing base address of pool to allocate memory
    void *poolAddr = ballocPool->pool;

    // Grabbing lower constraint
    const int lower = ballocPool->managementData[0];

    // Grabbing upper constraint
    const int upper = ballocPool->managementData[1];

    // Get the size of the block from the freelist
    // * What if cannot get size?
    int blockSize = freelistsize(list, poolAddr, mem, lower, upper);

    // Returning size of the block!
    return blockSize;
}

// A tool to output a text representation of the memory pool to stdout
// which is primarily a tool for debugging
// pool = A Balloc struct that contains the memory map
void bprint(Balloc pool)
{

    // Verify pool
    if (!pool)
    {
        // Pool does not exist

        // Outputting error message
        fprintf(stderr, "Pool does not exist!");
        exit(1);
    }
    else
    {
        // Starting to output the allocator
        fprintf(stdout, "Outputting the Memory Allocator!\n");
    }

    // Grab the representation of the pool
    // Note: The data in this balloc should be already good to go
    Rep *ballocPool = (Rep *)pool;

    // 3 Big things need to be output
    // 1. Base Address
    // 2. Management Data (Bounds, Range, Size)
    // 3. The FreeList (and location?)

    // 1. Base Address
    // Grab the pool address
    void *poolAddr = ballocPool->pool;

    // Outputting Pool Address into stdout
    fprintf(stdout, "The base address of the memory pool: %p\n", poolAddr);

    // 2. Management Data
    // Grabbing data to be output
    int lower = ballocPool->managementData[0];
    int upper = ballocPool->managementData[1];
    int range = ballocPool->managementData[2];

    // Outputting management data
    fprintf(stdout, "Lower Bounds (2^N): %d\n", lower);
    fprintf(stdout, "Upper Bounds (2^N): %d\n", upper);
    fprintf(stdout, "Range (Lower-Upper): %d\n", range);

    // 3. The FreeList
    // Grab the freelist
    FreeList *list = ballocPool->freeList;

    // Outputting the freelist using the method for it
    freelistprint(list, lower, upper);

    return;
}
