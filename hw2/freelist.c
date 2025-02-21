#include <stdlib.h>
#include <string.h>

#include "freelist.h"
#include "utils.h"
#include "bbm.h"

#define MAX_ORDER 10

// A singly linked list that represents the blocks of memory for the index and its size
struct Buddy
{

    // Storing current address
    void *currentLocation;

    // Storing location pointer for a singly linked list
    struct Buddy *nextBuddy;

} typedef Buddy;

// A freelist struct
struct freeListList
{

    // Holding GLOBAL bitmap in the freelist
    void *BitMap;

    // Storing all the buddies up to a maximum of 10
    Buddy *buddies[MAX_ORDER];

    // Another item that will set the [0] l: lowest power, [1] u: highest power, [2] r: number of buddies (u - l + 1)
    // * maybe not necessary
    // int managementData[3];

} typedef List;

// Builds a list of free blocks from the root node
// rootNode = The start of the singly linked list
// numberOfBlocks = How many blocks should be appended to the list
// blockSize = The size of the block to create
// * base = The base address of the memory pool
void buildlist(Buddy *rootNode, size_t numberOfBlocks, size_t blockSize, void *base)
{

    // Using this for looping purposes to keep appending nodes
    Buddy *currentNode = rootNode;

    // Holding the current address
    void *currentAddress = base;

    // Creating all the blocks, the blockSize should aleady be in bytes
    for (int i = 0; i < numberOfBlocks; i++)
    {

        // Building list failed
        if (!currentNode)
        {
            // Outputting error message
            fprintf(stderr, "Building the initial buddy list failed!");
            exit(1);
        }

        // Creating new block
        Buddy *newBuddy = mmalloc(sizeof(Buddy));

        // Getting new address using pointer arithmetic
        void *newAddress = (void *)((char *)currentAddress + blockSize);

        // Store the new current address
        currentAddress = newAddress;

        // Setting the current address to the new block
        newBuddy->currentLocation = newAddress;

        // Appending new block
        currentNode->nextBuddy = newBuddy;

        // Getting new currentNode
        currentNode = currentNode->nextBuddy;
    }

    // Setting final block to point at NULL
    currentNode->nextBuddy = NULL;

    // List created!
    return;
}

// Creates a freelist struct and returns the void *
// size = The size of the memory pool to work with
// l = Lower exponent bound
// u = Upper exponent bound
// * base = The address of the memory pool base
// Returns: FreeList, a struct containing the information on what blocks are free
FreeList freelistcreate(size_t size, int l, int u, void *base)
{

    // Variables should be presumably safe as they are verified in bcreate()
    // Used for aliasing
    const size_t sizeRequested = size;
    const int lower = l, upper = u;

    // * Unsure if should exist as saving the management data is kinda pointless
    // const int range = lower - upper;

    // Creating the freelist struct
    List *newFreelist = mmalloc(sizeof(List));

    // Creating a bit map
    BBM newBitMap = bbmcreate(sizeRequested, upper);

    // Creating the pointers that will hold the lists
    // Ex: lower = 4, upper = 6
    // buddies[0] --> NULL
    // buddies[1] --> NULL
    // buddies[2] --> NULL
    // buddies[3] --> NULL
    // buddies[4] --> NULL (can be changed)
    // buddies[5] --> NULL (can be changed)
    // buddies[6] --> void *
    // buddies[7] --> NULL
    // buddies[8] --> NULL
    // buddies[9] --> NULL

    for (int i = 0; i <= MAX_ORDER; i++)
    {
        if (i == upper)
        {
            // Highest value block
            // This means you need to break down the size of the pool
            // and create as many blocks of the highest size as you can

            // Getting the size of the largest block
            size_t largestBlockSize = e2size(upper);

            // Getting the number of blocks based on the largest size Ex: (size / 2^U) = numberOfBlocks
            size_t numberOfBlocks = divup(size, largestBlockSize);

            // New list node
            Buddy *biggestBuddy;

            // Creating a new list node
            biggestBuddy = mmalloc(sizeof(Buddy));

            // Setting the address to the base of the pool
            biggestBuddy->currentLocation = base;

            // Initally setting the nextBuddy to null for now...
            biggestBuddy->nextBuddy = NULL;

            // Build the list!
            buildlist(biggestBuddy, numberOfBlocks, largestBlockSize, base);

            // Save into the list
            newFreelist->buddies[i] = biggestBuddy;
        }
        else
        {
            // Lower levels do not start with a pointer

            // New list node
            Buddy *buddy;

            // Creating a new list node
            buddy = mmalloc(sizeof(Buddy));

            // Setting the address to the base of the pool
            buddy->currentLocation = base;

            // Since this buddy will not have a list built initially, it is pointing at nothing
            buddy->nextBuddy = NULL;

            // Save into the list
            newFreelist->buddies[i] = buddy;
        }
    }

    // Save bitmap into the list
    newFreelist->BitMap = newBitMap;

    // Returning the freelist as a void pointer
    return (void *)newFreelist;
}

// Deletes a freelist
// f = A freelist
// l = Lower exponent bound
// u = Upper exponent bound
void freelistdelete(FreeList f, int l, int u)
{

    // Validating the freelist
    if (!f)
    {
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }

    // Grabbing the free list
    const List *freelist = (List *)f;

    // Presuming l and u are correct

    // Freeing each buddy and their list

    // Freeing the freelist

    // Freelist should be bon voyage!
    return;
}

// Allocates a block of memory from the freelist
// f = A freelist
// * base = The base address of the pool (I believe)
// e = Requested exponent
// l = Lower exponent bound
// Returns: void *, an address of where the memory was allocated
void *freelistalloc(FreeList f, void *base, int e, int l)
{

    // Validating the freelist
    if (!f)
    {
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }

    // Block has been allocated
    return 0;
}

// Frees a block of memory in the freelist
// f = A freelist
// * base = The base addess of the pool
// * mem = Address of the location where the allocated block is
// e = Requested exponent
// l = Lower exponent bound
void freelistfree(FreeList f, void *base, void *mem, int e, int l)
{

    // Block has been freed!
    return;
}

// Grabs the size of a block in the freelist
// f = A freelist
// * base = The base address of the pool
// * mem = Address of the location where the block of memory is
// l = Lower exponent bound
// u = Upper exponent bound
// Returns: int, size of the block
int freelistsize(FreeList f, void *base, void *mem, int l, int u)
{

    return 0;
}

// Outputting tool of the freelist, useful for debugging
// f = A freelist
// l = Lower exponent bound
// u = Upper exponent bound
void freelistprint(FreeList f, int l, int u)
{

    // Used for aliasing
    const int lower = l, upper = u;

    // Validating the freelist
    if (!f)
    {
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }

    // Grab the list representation of the freelist
    List *list = (List *)f;

    // 2 Big things need to be output
    // 1. Buddy Bitmap
    // 2. All lists and their nodes

    // 1. Buddy Bitmap
    // Grab the buddy bitmap
    BBM *buddyBitmap = list->BitMap;

    // Use the provided implementation and output the bitmap
    // * THROWING A SEGFAULT
    // ? Nothing printing out (Is this intentional?)
    bbmprt(buddyBitmap);

    // 2. All lists and their nodes
    // Grab the lists (array of pointers)
    Buddy **buddies = list->buddies;

    // Starting to print the lists
    fprintf(stdout, "Printing out the buddies!\n");

    // Outputting each list one by one
    for (int i = lower; i <= upper; i++)
    {
        // Get the current buddy
        Buddy *currentBuddy = buddies[i];

        // Printing out current index of buddy lists
        fprintf(stdout, "Freelist[%d] (Size: %ld): ", i, e2size(i));

        // Printing out the current buddy
        // fprintf(stdout, "Current Buddy Location: %p", currentBuddy->currentLocation);

        // Loop through all buddies in the singly linked list
        // Using index to note base
        int index = 0;
        while (currentBuddy)
        {
            // At base address (and increment)
            if (index++ == 0)
            {
                // Stating base at the list
                fprintf(stdout, "[BASE]-------->");
            }
            else
            {
                // Output current location
                fprintf(stdout, "[%p]-------->", currentBuddy->currentLocation);
            }

            // Update currentBuddy
            currentBuddy = currentBuddy->nextBuddy;
        }

        // Reached end of list (presumably)
        if (!currentBuddy)
        {
            // Append 'NULL' to the end
            // Since it is pointing at nothing
            fprintf(stdout, "NULL\n");
        }
    }

    // Outputting of freelist complete!
    return;
}