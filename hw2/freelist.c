#include <stdlib.h>
#include <string.h>

#include "freelist.h"
#include "utils.h"
#include "bbm.h"

#define MAX_ORDER 10

// A singly linked list that represents the blocks of memory for the index and its size
struct Buddy
{

    // Holding levels bitmap in the freelist
    BBM bitmap;

    // Storing current address
    void *currentLocation;

    // Storing location pointer for a singly linked list
    struct Buddy *nextBuddy;

} typedef Buddy;

// A freelist struct
struct freeListList
{

    // Pool of the allocator
    void *baseAddress;

    // Storing all the buddies up to a maximum of 10
    Buddy *buddies[MAX_ORDER];

    // Another item that will set the [0] l: lowest power, [1] u: highest power, [2] r: number of buddies (u - l + 1)
    int managementData[3];

} typedef List;

// Builds a list of free blocks from the root node
// rootNode = The start of the singly linked list
// numberOfBlocks = How many blocks should be appended to the list
// blockSize = The size of the block to create
// * base = The base address of the memory pool
void buildlist(Buddy *rootNode, size_t numberOfBlocks, int blockExponent, void *base, BBM bitmap)
{

    // Using this for looping purposes to keep appending nodes
    Buddy *currentNode = rootNode;

    // Holding the base address of the bitmap
    // void *baseBBMAddress = buddyBitmap;

    // Hodling the current address to the bit map
    // void *currentBBMAddress = buddyBitmap;

    // Holding the current address (initally starting with base)
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
        void *newAddress = (void *)((char *)currentAddress + e2size(blockExponent));

        // Getting new bit map address
        // i represents the number of buddies (kind of)
        // void *newBBMAddress = (void *)((char *)baseBBMAddress + i);

        // Store the new current address
        currentAddress = newAddress;

        // Setting the current address to the new block
        newBuddy->currentLocation = currentAddress;

        // Adding the bitmap to the new buddy
        newBuddy->bitmap = bitmap;

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

    // Calculating range
    const int range = lower - upper;

    // Creating the freelist struct
    List *newFreelist = mmalloc(sizeof(List));

    // Creating a bit map (void *)
    // Ex: Bitmap(64, 5)
    // BBM newBitMap = bbmcreate(sizeRequested, upper);

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

        // Build a bitmap for that level
        BBM newBuddyBitmap = bbmcreate(sizeRequested, i);

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

            // Saving bitmap into the buddy
            biggestBuddy->bitmap = newBuddyBitmap;

            // Build the list!
            buildlist(biggestBuddy, numberOfBlocks - 1, upper, base, newBuddyBitmap);

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

            // Setting the address to the base of the pool (NULL because it isn't valid)
            buddy->currentLocation = NULL;

            // Since this buddy will not have a list built initially, it is pointing at nothing
            buddy->nextBuddy = NULL;

            // Saving bitmap into the buddy
            buddy->bitmap = newBuddyBitmap;

            // Save into the list
            newFreelist->buddies[i] = buddy;
        }
    }

    // Saving management data in the free list
    newFreelist->managementData[0] = lower;
    newFreelist->managementData[1] = upper;
    newFreelist->managementData[2] = range;

    // Saving base address into the free list
    newFreelist->baseAddress = base;

    // Save bitmap into the list
    // newFreelist->BitMap = newBitMap;

    // Returning the freelist as a void pointer
    return (void *)newFreelist;
}

// Deletes a freelist
// f = A freelist
// l = Lower exponent bound
// u = Upper exponent bound
void freelistdelete(FreeList f, int l, int u)
{

    // Grabbing the free list
    List *list = (List *)f;

    // Validing the List
    if (list == NULL)
    {
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }

    // 2 Big things to delete
    // 1. Bitmap
    // 2. All of the buddies and their singly linked lists

    // Grab the lists (array of pointers)
    Buddy **buddies = list->buddies;

    // Freeing each buddy and their list
    for (int i = 0; i < MAX_ORDER; i++)
    {
        // Get the current buddy
        Buddy *currentBuddy = buddies[i];

        // Grab the bitmap
        BBM currentBitmap = currentBuddy->bitmap;

        // Delete the bitmap
        bbmdelete(currentBitmap);

        // NULL the bitmap
        currentBitmap = NULL;

        // Loop through the whole singly linked list
        while (currentBuddy)
        {
            // Save next buddy before freeing the current one
            Buddy *nextBuddy = currentBuddy->nextBuddy;

            // Free the 'current' buddy
            mmfree(currentBuddy, sizeof(Buddy));

            // NULL out the buddy
            currentBuddy = NULL;

            // Update the buddy to be freed
            currentBuddy = nextBuddy;
        }
    }

    // Freeing the freelist
    mmfree(f, sizeof(List));

    // WHEN DELETING YOU NEED TO DEAL WITH THE POINTERS
    f = NULL;

    // Freelist should be bon voyage!
    return;
}

// Helper method that does the dirty work for allocation
// bitmap = A buddy bit map
// * base = The base of the memory address in the pool
// exponent = Exponent of the block size
// singleNodeFlag = Flag to indicate if the list is a single node
// ** buddies = Array of pointers to the buddies
// Returns: void *, Where the allocated block starts
void *allocation(Buddy *currentBuddy, Buddy **buddies, BBM bitmap, void *base, int exponent, int singleNodeFlag)
{

    // Address that is at the start of the to be allocated block
    void *location;

    // Condition where there is a single node in the list
    if (singleNodeFlag)
    {
        // Grab the address of the block the buddy is pointing too (so it can be returned)
        location = currentBuddy->currentLocation;

        // Update the current block (to point at nothing)
        currentBuddy->currentLocation = NULL;

        // Update the bitmap to mark the block
        // Using the base of the bitmap and the offset
        bbmset(bitmap, base, location, exponent);
    }
    else
    {
        // There are at least 2 blocks in the list

        // Grab the address of the block the buddy is pointing too (so it can be returned)
        location = currentBuddy->currentLocation;

        // Get the next buddy so it will cover in the spot of the previous buddy
        Buddy *nextBuddy = currentBuddy->nextBuddy;

        // Free the block
        mmfree(currentBuddy, sizeof(Buddy));

        // Save that block into the head of the list
        buddies[exponent] = nextBuddy;

        // Using the base of the bitmap and the offset
        bbmset(bitmap, base, location, exponent);
    }

    // Return the location of the allocated block
    return location;
}

// Checks if there is another node in the list
// * currentBuddy = The buddy being checked
// Returns: int, 1 for single, 0 for no
int issingle(Buddy *currentBuddy)
{
    // Is there a friend?
    if (currentBuddy->nextBuddy != NULL)
    {
        // Yay, friend
        return 0;
    }
    else
    {
        // No friend
        return 1;
    }
}

// Splits a block recursively down to the requested exponent
// * currentBuddy = The current buddy
// ** buddies = An array of pointers to all of the buddies
// bitmap = A buddy bitmap
// * base = The base address of the memory pool
// lower = Lower bound exponent
// requestedExponent = Requested size allocation in exponent form
// exponent = Current block size exponent
// Returns: Buddy *, A pointer to the location of the currentBuddy (after all the splits and stuff)
Buddy *splitblock(Buddy *currentBuddy, Buddy **buddies, BBM bitmap, void *base, int lower, int requestedExponent, int exponent)
{

    // BASE CASE: Have we finally reached what was requested
    if (requestedExponent == exponent)
    {
        // This means the splitting is complete and the lists and bitmaps should reflect that
        return buddies[requestedExponent];
    }
    else
    {
        // Split the block

        // Get the list at the current level (AKA the one below the currentBuddy)
        Buddy *currentBuddyLevel = buddies[exponent - 1];

        // Grab the bitmap at that level
        BBM currentBitMap = currentBuddyLevel->bitmap;

        // Do a typical allocation, but there will be more work
        // Getting the starting address from the bigger block
        // 1st new block: startAddr
        // 2nd new block: secondStartAddr
        void *startAddr = allocation(currentBuddy, buddies, bitmap, base, exponent, issingle(currentBuddy));

        // Finding the middle address between the startAddr and the ending address
        void *secondStartAddr = ((char *)(startAddr) + (e2size(exponent) / 2));

        // Build the list at the lower level
        // New list node
        // May seem strange that it is "secondNode", but it is the one that must be added first
        Buddy *secondNode = mmalloc(sizeof(Buddy));

        // Setting the address to the secondStartAddr
        secondNode->currentLocation = secondStartAddr;

        // ? Setting to what it is looking at NULL.. for now
        // * This logic might be wrong
        // * Im unsure about this becuase if you are splitting does
        // that not imply the level has no nodes? (Might even throw a segfault)
        secondNode->nextBuddy = currentBuddyLevel->nextBuddy;

        // Saving bitmap into the buddy
        secondNode->bitmap = currentBitMap;

        // Add to the start of the list
        buddies[exponent - 1] = secondNode;

        // Now repeat the process for the 'first' node and add it to the list
        Buddy *firstNode = mmalloc(sizeof(Buddy));

        // Using the startAddr this time
        firstNode->currentLocation = startAddr;

        // firstNode --> Next --> secondNode
        // Ehh? Makes sense, huh?
        firstNode->nextBuddy = secondNode;

        // Saving bitmap into the buddy
        firstNode->bitmap = currentBitMap;

        // Add to the start of the list
        buddies[exponent - 1] = firstNode;

        // I believe the splitting is done now
        // Recursion time now! Yay! :')

        // Set the current buddy to the firstNode
        currentBuddy = firstNode;

        // Same call but with the exponent dropping
        return splitblock(currentBuddy, buddies, currentBitMap, base, lower, requestedExponent, exponent - 1);
    }
}

// Makes sure the exponent that is being searched is not greater than upper
// because that implies there are no valid free blocks that will satify the allocation
//
void checkexponent(int exponent, int upper)
{
    if (exponent > upper)
    {
        // Cannot go higher, there is not a block that will satisfy this allocation

        // Output error message
        // ? Should it be an error message
        fprintf(stdout, "Cannot allocate memory: No free block in system that is of adequate size!\n");
        exit(1);
    }
    else
    {
        // Still ok
        return;
    }
}

// Allocates a block of memory from the freelist
// f = A freelist
// * base = The base address of the pool (I believe)
// e = Requested exponent
// l = Lower exponent bound
// Returns: void *, an address of where the memory was allocated
void *freelistalloc(FreeList f, void *base, int e, int l)
{

    // Aliasing
    int exponent = e, lower = l;

    // Used for later case when you need a base exponent
    int requestedExponent = e;

    // Validating the freelist
    if (!f)
    {
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }

    // Grab the list representation of the freelist
    List *list = (List *)f;

    // Validating the base pool address
    if (!base)
    {
        // Outputting error messsage
        fprintf(stderr, "Base pool address is not valid!");
        exit(1);
    }

    // Allocation begins
    // First, check to see if there is a free block in the correlate list to e

    // Grab the lists (array of pointers)
    Buddy **buddies = list->buddies;

    // Grab the buddy at the exponent requested level
    Buddy *currentBuddy = buddies[exponent];

    // Grab upper exponent
    const int upper = list->managementData[1];

    // Memory return address
    void *startOfFreeMem;

    // Check if there is a valid block
    if (currentBuddy->currentLocation != NULL)
    {
        // There is a free block!
        // Processing the block

        // Grabbing bitmap from the respective level
        BBM bitmap = currentBuddy->bitmap;

        // Casting the bitmap for indexing later
        char *bytes = (char *)bitmap;

        // Run a allocation with checking the case if the list is single noded or not
        startOfFreeMem = allocation(currentBuddy, buddies, bitmap, base, exponent, issingle(currentBuddy));
    }
    else
    {
        // Need to go to a higher level and split
        // Go up level (Block Size)
        exponent++;

        // Flag for when a valid block is found
        int found = 0;

        // This while loop is going to keep climbing levels until it finds a free block that can be split
        while (!found)
        {

            // Make sure things are not getting out of scope
            checkexponent(exponent, upper);

            // Looking for the block size above
            currentBuddy = buddies[exponent];

            // Is there something free?
            if (currentBuddy->currentLocation != NULL)
            {
                // Free block has been located
                // Time to split the block

                // Free block has been found
                found = 1;
            }
            else
            {
                // Go to the next level
                exponent++;
            }
        }

        // Grabbing bitmap from the respective level
        BBM bitmap = currentBuddy->bitmap;

        // Split the blocks recursively
        currentBuddy = splitblock(currentBuddy, buddies, bitmap, base, lower, requestedExponent, exponent);

        // Get bitmap after the splitting
        bitmap = currentBuddy->bitmap;

        // Run that allocation
        startOfFreeMem = allocation(currentBuddy, buddies, bitmap, base, e, issingle(currentBuddy));
    }

    // Block has been allocated
    return startOfFreeMem;
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

    // Grab the lists (array of pointers)
    Buddy **buddies = list->buddies;

    // 2 Big things need to be output
    // 1. Buddy Bitmap
    // 2. All lists and their nodes

    // 1. Buddy Bitmap
    // These will be printed off in by there blocks
    fprintf(stdout, "Printout the bitmaps by level!\n");

    // Outputting each bitmap one by one
    for (int i = lower; i <= upper; i++)
    {
        // Get the current buddy
        Buddy *currentBuddy = buddies[i];

        // Grab the bitmap for that buddies level
        BBM bitmap = currentBuddy->bitmap;

        // Print out the bitmap
        fprintf(stdout, "Freelist[%d]: ", i);
        bbmprt(bitmap);
    }

    // Whitespace
    fprintf(stdout, "\n");

    // 2. All lists and their nodes

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
        // int index = 0;
        while (currentBuddy)
        {
            // At base address (and increment)
            // if (index++ == 0)
            // {
            //     // Stating base at the list
            //     fprintf(stdout, "[BASE]-------->");
            // }
            // else
            // {
            //     // Output current location
            //     fprintf(stdout, "[%p]-------->", currentBuddy->currentLocation);
            // }

            fprintf(stdout, "[%p]-------->", currentBuddy->currentLocation);

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