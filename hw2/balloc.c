/**
 * A balloc memory allocator module that uses the buddy system
 *
 * @author Brian Wu
 * @version 1.0
 *
 */

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
    Rep *newBalloc = mmalloc(sizeof(Rep));

    // Mapping the memory in the address space to be used
    void *poolAddr = mmalloc(actualSize);

    // Save address into newBalloc for start of pool
    newBalloc->pool = poolAddr;

    // Getting number of buddies for the pool
    // The +1 is there for the [0:r-1] constraint
    // Ex: 6 - 3 = 3, but you need 4 buddies
    // (3, 4, 5, 6) ==> [0, 1, 2 (stops because r-1 = 2)]
    numberOfBuddies = upper - lower + 1;

    // Storing management data information into the newBalloc
    // Lower, Upper, and Number of Buddies
    newBalloc->managementData[0] = lower;
    newBalloc->managementData[1] = upper;
    newBalloc->managementData[2] = numberOfBuddies;

    // Storing size
    newBalloc->size = actualSize;

    // Adding the freelist to the newBalloc
    newBalloc->freeList = freelistcreate(actualSize, lower, upper, poolAddr);

    // Returning the address of the created Balloc
    return (void *)newBalloc;
}

// Deletes the pool of memory that was created
// pool = A Balloc struct that contains the memory map
void bdelete(Balloc pool)
{

    // Grabbing representation of pool
    Rep *ballocPool = (Rep *)pool;

    // Verify allocator
    if (ballocPool == NULL)
    {
        // Allocator does not exist

        // Ouputting error message
        fprintf(stderr, "Pool does not exist!\n");
        exit(1);
    }

    // 3 Things need to be deleted and NULLed
    // 1. Pool Address
    // 2. Freelist
    // 3. Management Data

    // 1. Pool Address
    if (ballocPool->pool != NULL)
    {
        // Base address is still valid

        // Unmap the memory pool
        mmfree(ballocPool->pool, ballocPool->size);

        // Make sure to set the address to NULL
        ballocPool->pool = NULL;
    }

    // 2. Freelist
    // Grabbing freelist
    const FreeList list = ballocPool->freeList;

    // Grabbing lower/upper bounds and the size
    const int lower = ballocPool->managementData[0];
    const int upper = ballocPool->managementData[1];

    // * Not needed
    // const int size = ballocPool->size;

    // Unmapping the freelist
    freelistdelete(list, lower, upper);

    // 3. Management Data
    // Setting all values to 0
    ballocPool->managementData[0] = 0;
    ballocPool->managementData[1] = 0;
    ballocPool->managementData[2] = 0;
    ballocPool->size = 0;

    // Unmapping the Allocator
    mmfree(ballocPool, sizeof(Rep));

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

    // Grabbing upper constrant (verification of allocation requested)
    const int upper = ballocPool->managementData[1];

    // Checking if allocation is a valid size in what is requested
    if (requestedSize > e2size(upper))
    {
        // Allocation is not within valid constraints

        // Outputting error message
        fprintf(stderr, "Cannot allocate this amount as it is above the allocators constraints (%d)!\n", requestedSize);
        exit(1);
    }

    // Checking if the allocation exponent needs to be set on lower constraint if value is too small
    if (actualSizeE < lower)
    {
        // Normalizing exponent to lower
        actualSizeE = lower;
    }

    // Calling the freelist to allocate the memory
    void *allocatedSpot = freelistalloc(list, poolAddr, actualSizeE, lower);

    // Return the address at the start of the allocation
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
    // ^ Might not need to check that
    // * What if cannot free?
    freelistfree(list, poolAddr, mem, size2e(sizeOfBlock), lower);

    // Block is freed!
    return;
}

// Grabs the size of the memory block
// pool = A Balloc struct that contains the memory map
// * mem = A void pointer that is pointing at the block of memory to grab its size
// Returns: int, the size of the block
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
    int blockSize = freelistsize(list, poolAddr, mem, lower, upper);

    // Returning size of the block!
    // Have to convert exponent value to block size
    return e2size(blockSize);
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
    fprintf(stdout, "Memory Allocator Pool: %p\n", poolAddr);
    fprintf(stdout, "--------------------------\n");

    // 2. Management Data
    // Grabbing data to be output
    int lower = ballocPool->managementData[0];
    int upper = ballocPool->managementData[1];
    int range = ballocPool->managementData[2];

    // Outputting management data
    fprintf(stdout, "Lower Bounds (2^N): %d\n", lower);
    fprintf(stdout, "Upper Bounds (2^N): %d\n", upper);
    fprintf(stdout, "Range (Lower-Upper): %d\n", range);

    // Whitespace
    fprintf(stdout, "--------------------------\n");

    // 3. The FreeList
    // Grab the freelist
    FreeList *list = ballocPool->freeList;

    // Outputting the freelist using the method for it
    freelistprint(list, lower, upper);

    // Whitespace
    fprintf(stdout, "--------------------------\n\n");

    return;
}
