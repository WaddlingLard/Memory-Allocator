/**
 * A freelist for the balloc module.
 *
 * @author Brian Wu
 * @version 1.0
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "freelist.h"
#include "utils.h"
#include "bbm.h"

// You can request up to 2^50
#define MAX_ORDER 50

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
// @param rootNode = The start of the singly linked list
// @param numberOfBlocks = How many blocks should be appended to the list
// @param blockSize = The size of the block to create
// @param *base = The base address of the memory pool
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
// @param size = The size of the memory pool to work with
// @param l = Lower exponent bound
// @param u = Upper exponent bound
// @param *base = The address of the memory pool base
// @return Returns: FreeList, a struct containing the information on what blocks are free
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
// @param f = A freelist
// @param l = Lower exponent bound
// @param u = Upper exponent bound
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

// Checks if there is another node in the list
// @param *currentBuddy = The buddy being checked
// @return Returns: int, 1 for single, 0 for no
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

// Acts very similar to allocaion but is purely used by buildup() to remove a pair of nodes
// @param *currentBuddy = The left buddy
// @param **buddies = The array of buddy head nodes
// @param *base = The base address of the pool
// @param exponent = The block size exponent
// @param atHead = flag for case if currentBuddy is at head of list
void removenodepair(Buddy *currentBuddy, Buddy **buddies, void *base, int exponent, int atHead)
{
    // This method is called when there is at least two nodes
    // Address to look for
    void *targetAddress = currentBuddy->currentLocation;

    // Is the pair at the head
    if (atHead)
    {
        // Deal with removing two nodes at the start of the list

        // Get the next buddy so it will be deleted as well
        Buddy *nextBuddy = currentBuddy->nextBuddy;

        // Free the block
        mmfree(currentBuddy, sizeof(Buddy));

        // Save that block into the head of the list
        buddies[exponent] = nextBuddy;

        // Will the nextBuddy be the last node in the list
        if (issingle(nextBuddy))
        {
            // Set that buddy to NULL
            nextBuddy->currentLocation = NULL;
        }
        else
        {
            // Can free next buddy as well, but need to grab the nextBuddy's nextBuddy (a little confusing I know)
            // Ex: [currentBuddy (TO BE FREED)]-->[nextBuddy (TO BE FREED)]-->[nextBuddy's nextBuddy]
            Buddy *nextnextBuddy = nextBuddy->nextBuddy;

            // Free the block
            mmfree(nextBuddy, sizeof(Buddy));

            // Save that block into the head of the list
            buddies[exponent] = nextnextBuddy;
        }
    }
    else
    {
        // Find the node before the pair to remove
        Buddy *startOfList = buddies[exponent];

        // Loop through the whole list of buddies
        while (1)
        {
            // Get the next buddy to check if the pair is there
            Buddy *nextBuddy = startOfList->nextBuddy;

            // Does the next node == the node pair we want to delete?
            if (nextBuddy->currentLocation == targetAddress)
            {
                // It is what we want to delete
                // Get the nextnextBuddy to delete
                Buddy *nextnextBuddy = nextBuddy;

                // Is the right buddy have something after
                if (issingle(nextnextBuddy))
                {
                    // There no node after

                    // Adjust pointers
                    startOfList->nextBuddy = NULL;

                    // Free left and right nodes
                    mmfree(nextBuddy, sizeof(Buddy));
                    mmfree(nextnextBuddy, sizeof(Buddy));
                }
                else
                {
                    // There is a node after

                    // Adjust pointers
                    startOfList->nextBuddy = nextnextBuddy->nextBuddy;

                    // Free left and right nodes
                    mmfree(nextBuddy, sizeof(Buddy));
                    mmfree(nextnextBuddy, sizeof(Buddy));
                }

                // No more looping
                break;
            }
            else
            {
                // Iterate the list
                startOfList = nextBuddy;
            }
        }
    }

    // It is removed!
    return;
}

// Helper method that does the dirty work for allocation
// @param *currentBuddy = The current buddy in pointer form
// @param **buddies = Array of pointers to the buddies
// @param bitmap = A buddy bit map
// @param *base = The base of the memory address in the pool
// @param exponent = Exponent of the block size
// @param singleNodeFlag = Flag to indicate if the list is a single node
// @return Returns: void *, Where the allocated block starts
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

// Splits a block recursively down to the requested exponent
// @param *currentBuddy = The current buddy
// @param **buddies = An array of pointers to all of the buddies
// @param bitmap = A buddy bitmap
// @param *base = The base address of the memory pool
// @param lower = Lower bound exponent
// @param requestedExponent = Requested size allocation in exponent form
// @param exponent = Current block size exponent
// @return Returns: Buddy *, A pointer to the location of the currentBuddy (after all the splits and stuff)
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
// @param exponent = Current exponent
// @param upper = Upper exponent bound
void checkexponent(int exponent, int upper)
{
    if (exponent > upper)
    {
        // Cannot go higher, there is not a block that will satisfy this allocation

        // Output error message
        fprintf(stderr, "Cannot allocate memory: No free block in system that is of adequate size!\n");
        exit(1);
    }
    else
    {
        // Still ok
        return;
    }
}

// Allocates a block of memory from the freelist
// @param f = A freelist
// @param *base = The base address of the pool (I believe)
// @param e = Requested exponent
// @param l = Lower exponent bound
// @return Returns: void *, an address of where the memory was allocated
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

// Helper method that gives you an idea what buddy of the two you are looking a
// This is achieved using modular division as the block to the 'right' will always
// return a value that is non zero when you apply % sizeofLargerBlock as opposed
// to the block on the left
// @param *currentBuddy = The current buddy being checked
// @param *base = The base address of the memory pool
// @param sizeOfLargerBlock = The size of a block that is a level highert
// @return Returns: int, 1 for left, 0 for right
int leftorrightbuddy(Buddy *currentBuddy, void *base, int sizeOfLargerBlock)
{
    // Getting location of the currentBuddy
    void *currentBuddyLocation = currentBuddy->currentLocation;

    // Calculating the offset from the base by using a pointer diff
    ptrdiff_t offset = (char *)base - (char *)currentBuddyLocation;

    // Is it left or right
    if (offset % sizeOfLargerBlock == 0)
    {
        // Left block return value
        return 1;
    }
    else
    {
        // Right block return value
        return 0;
    }
}

// Loops through a singly linked list to find an address
// @param *headOfList = A buddy that starts at the front of the list
// @param *address = Address that is being looked for
// @return Returns: int, 1 for found, 0 for no
int lookforaddress(Buddy *headOfList, void *address)
{

    // Current buddy holder
    Buddy *currentBuddy = headOfList;

    // Loops through all buddies in the list
    while (currentBuddy != NULL)
    {
        // Is the address in the list?
        if (currentBuddy->currentLocation == address)
        {
            // Found it.
            return 1;
        }
        // Increment currentBuddy pointer
        currentBuddy = currentBuddy->nextBuddy;
    }

    // Did not find it :(
    return 0;
}

// Checks if the list is empty
// @param *currentBuddy = A pointer to the buddy that's list is being checked
// @return Returns: int, 1 for is empty and 0 for no
int isemptylist(Buddy *currentBuddy)
{
    // Is list empty?
    if (currentBuddy->currentLocation == NULL)
    {
        // Return it is empty value
        return 1;
    }
    else
    {
        // List is not empty

        // Not empty return value
        return 0;
    }
}

// Helper method that does the dirty work for unallocation
// @param ** buddies = Array of pointers to the buddies
// @param bitmap = A buddy bitmap
// @param *base = The base of the memory address in the pool
// @param *mem = The offset of where the allocation occured
// @param exponent = Exponent of the block that needs to be unallocated
// @param lower = Lower Exponent Bounds
// @return Returns: Buddy *, a pointer to the resurrected buddy!
Buddy *unallocation(Buddy **buddies, BBM bitmap, void *base, void *mem, int exponent, int lower, int emptyListFlag)
{
    // Get the head of the list for the buddy level
    Buddy *currentBuddyLevel = buddies[exponent];

    // Saving buddy location
    Buddy *freedBuddy;

    // Add back into the freelist, but must account if the list is empty or not
    if (emptyListFlag)
    {
        // List is empty
        // Simple, just modify the front of the list

        // Setting the address to the location of where the allocation occured
        currentBuddyLevel->currentLocation = mem;

        // Saving the location into freedBuddy
        freedBuddy = currentBuddyLevel;
    }
    else
    {
        // List is not empty
        // Not as simple, must figure out where to appropriately put in the list

        // Rebuild a new node
        Buddy *resurrectedBuddy = mmalloc(sizeof(Buddy));

        // Set the bitmap on the new node with the currentBuddyLevel's bitmap (they are on the same level)
        resurrectedBuddy->bitmap = currentBuddyLevel->bitmap;

        // Setting the address to the location of where the allocation occured
        resurrectedBuddy->currentLocation = mem;

        // Find where to insert in list

        // Check if allocation already exists
        if (mem == currentBuddyLevel->currentLocation)
        {
            // Outputting error message
            fprintf(stderr, "Should not be possible to do this operation!\n");
            exit(1);
        }

        // Base Case: Is it at the start?
        if (mem < currentBuddyLevel->currentLocation)
        {
            // At the start of the list
            // Set the new buddy's next value to the currentBuddyLevel (Head of list)
            resurrectedBuddy->nextBuddy = currentBuddyLevel;

            // Set new head of list to the resurrectedBuddy
            buddies[exponent] = resurrectedBuddy;

            // Saving the location into freedBuddy
            freedBuddy = resurrectedBuddy;
        }
        else
        {
            // Somewhere not at the start

            // Aliasing for the current buddy
            Buddy *currentBuddy = currentBuddyLevel;

            // Grabbing the next value of the list
            Buddy *nextBuddy = currentBuddy->nextBuddy;

            // If this is false that means we are at the end of the list
            while (nextBuddy != NULL)
            {
                // Check if allocation already exists
                if (mem == currentBuddyLevel->currentLocation)
                {
                    // Outputting error message
                    fprintf(stderr, "Should not be possible to do this operation!\n");
                    exit(1);
                }

                // Grabbing freed buddy location
                void *ressurectedBuddyLocation = resurrectedBuddy->currentLocation;

                // Grabbing next buddy location
                void *nextBuddyLocation = nextBuddy->currentLocation;

                // Comparing to see if nextBuddy is ahead of ressurectedBuddy
                if (ressurectedBuddyLocation < nextBuddyLocation)
                {
                    // Behind, we have found the spot to nestle it in
                    // Set the resurrectedBuddy's next to be the nextBuddy
                    // Ex: [currentBuddy]-->[resurrectedBuddy]-->[nextBuddy]-->....
                    resurrectedBuddy->nextBuddy = nextBuddy;

                    // Set the currentBuddy's next to be the resurrectedBuddy
                    currentBuddy->nextBuddy = resurrectedBuddy;

                    // Saving the location into freedBuddy
                    freedBuddy = resurrectedBuddy;

                    // No more of this as the list is now properly aligned
                    break;
                }
                else
                {
                    // Have not found spot yet, just continue down the list
                    // Iterate both buddies
                    currentBuddy = nextBuddy;
                    nextBuddy = currentBuddy->nextBuddy;
                }
            }

            // Checking if we reached the end of the list
            if (nextBuddy == NULL)
            {
                // That means just add resurrectedBuddy to the end of the currentBuddy
                // which in principle should be the last node
                // Ex: ...->[currentBuddy]-->[resurrectedBuddy]-->
                currentBuddy->nextBuddy = resurrectedBuddy;

                // Setting the resurrectedBuddy's next buddy to be NULL because its at the tail
                resurrectedBuddy->nextBuddy = NULL;

                // Saving the location into freedBuddy
                freedBuddy = resurrectedBuddy;
            }
        }
    }

    // Returning the freed buddy
    return freedBuddy;
}

// Recursively builds up the freelist until it cannot
// @param *resurrectedBuddy = The buddy that was unallocated
// @param **buddies = The array of all the buddy list heads
// @param bitmap = A bitmap at the exponent level
// @param *base = The base address of the pool
// @param *mem = The location of where the buddy was freed
// @param exponent = Block size exponent
// @param upper = Upper exponent bounds
// @param lower = Lower exponent bounds
void buildup(Buddy *resurrectedBuddy, Buddy **buddies, BBM bitmap, void *base, void *mem, int exponent, int upper, int lower)
{
    // Get the startOfList
    Buddy *startOfList = buddies[exponent];

    // Upper exponent that searches for the upper level
    int upperExponentLevel = exponent + 1;

    // Check if we are at the top
    if (upperExponentLevel > upper)
    {
        // Change the bitmap as this is the last free possible
        bbmclr(bitmap, base, mem, exponent);

        // Building complete
        return;
    }

    // Got to find out the buddy I just brought back
    int leftorright = leftorrightbuddy(resurrectedBuddy, base, e2size(exponent + 1));

    // Result from the lookfor()
    int lookfor = -1;

    // The base address of the left and right buddy
    // Ex: ...-->[Left]-->[Right]-->...
    // ----------^Base^----------------------------
    void *baseAddressOfBoth;

    // Now we know if the freedbuddy is the left or right of the pair
    // Check the other one
    if (leftorright)
    {
        // Check the right one
        // Calculate the size to look for
        void *rightBuddy = (char *)resurrectedBuddy->currentLocation + e2size(exponent);

        // Set the baseAddressOfBoth to the resurrectedBuddy
        baseAddressOfBoth = resurrectedBuddy->currentLocation;

        // Look to see if the address is in the free blocks
        lookfor = lookforaddress(startOfList, rightBuddy);
    }
    else
    {
        // Check the left one
        void *leftBuddy = (char *)resurrectedBuddy->currentLocation - e2size(exponent);

        // Set the baseAddressOfBoth to
        baseAddressOfBoth = leftBuddy;

        // Look to see if the address is in the free blocks
        lookfor = lookforaddress(startOfList, leftBuddy);
    }

    // Is the buddy found?
    if (lookfor)
    {
        // Grab the bitmap level of the buddy that was brought back
        BBM currentBitmap = resurrectedBuddy->bitmap;

        // A pair of buddies are found, which means they will be built up so the bitmap should reflect that change
        bbmclr(currentBitmap, base, mem, exponent);

        // Remove the two buddies from the lower level so they can be tranferred to a higher level
        Buddy *currentBuddy = startOfList;

        // Loop through whole list of buddies
        while (1)
        {

            // Found the base address to the pair of buddies
            if (currentBuddy->currentLocation == baseAddressOfBoth)
            {
                // Left buddy found!
                // Remove the pair
                removenodepair(currentBuddy, buddies, base, exponent, currentBuddy->currentLocation == startOfList->currentLocation);

                // No more looping
                break;
            }
            else
            {
                // Keep iterating
                currentBuddy = currentBuddy->nextBuddy;
            }
        }

        // Get the buddy head at the next level exponent
        Buddy *currentBuddyLevel = buddies[upperExponentLevel];

        // Update the bitmap
        currentBitmap = currentBuddyLevel->bitmap;

        // Call another unallocation on the higher level
        Buddy *freedBuddy = unallocation(buddies, currentBitmap, base, baseAddressOfBoth, upperExponentLevel, lower, isemptylist(currentBuddyLevel));

        // Possibly could use some recursion...
        // RECURSION TIME
        return buildup(freedBuddy, buddies, currentBitmap, base, baseAddressOfBoth, upperExponentLevel, upper, lower);
    }
    else
    {
        // Buddy not found :(
        return;
    }
}

// Frees a block of memory in the freelist
// @param f = A freelist
// @param *base = The base addess of the pool
// @param *mem = Address of the location where the allocated block is
// @param e = Requested exponent
// @param l = Lower exponent bound
void freelistfree(FreeList f, void *base, void *mem, int e, int l)
{
    // Aliasing
    int exponent = e, lower = l;

    // Validating the freelist
    if (!f)
    {
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }

    // Grab the list representation of the freelist
    List *list = (List *)f;

    // Grabbing upper exponent bounds
    int upper = list->managementData[1];

    // Validating the base pool and mem offset address
    if (!base || !mem)
    {
        // Outputting error messsage
        fprintf(stderr, "Base pool or mem offset address is not valid!");
        exit(1);
    }

    // Grab the lists (array of pointers)
    Buddy **buddies = list->buddies;

    // Get the head of the list for the buddy level
    Buddy *currentBuddyLevel = buddies[exponent];

    // Get the bitmap at the current level
    BBM currentBitmap = currentBuddyLevel->bitmap;

    // When you are freeing something, you practially have the
    // Buddy, just not in an explicit struct.
    // When you free a node you have to check its buddy since
    // a buddy contains two nodes...
    // That is because a bitmap represents two nodes at one bit location

    // Need to check if it should be built up, if not just return from the dead
    // This will require some understanding of how to restructure the list

    // First bring back the buddy via unallocation
    Buddy *resurrectedBuddy = unallocation(buddies, currentBitmap, base, mem, exponent, lower, isemptylist(currentBuddyLevel));

    // Recursion to build it back up
    buildup(resurrectedBuddy, buddies, currentBitmap, base, mem, exponent, upper, lower);

    // Block has been freed!
    return;
}

// Recursively goes down bitmaps to find the respective exponent
// @param **buddies = An array of buddies via pointers
// @param *base = The base address of the pool
// @param *mem = The offset address of where the bit is
// @param upper = Upper exponent bound
// @param lower = Lower exponent bound
// @param currentExponent = The current exponent level in the buddy list being checked
// @return Returns: size_t, The size of the exponent
size_t exponentsearcher(Buddy **buddies, void *base, void *mem, int upper, int lower, int currentExponent)
{

    // BASE CASE: Have we reached the end of the buddies?
    if (currentExponent < lower)
    {
        // This is the end of the line, son
        // That means the lowest level of the buddies is where the allocation occured

        // The lowest exponent it is then!
        return currentExponent + 1;
    }

    // Need to grab a bit map
    // ? Maybe start at the top?
    Buddy *currentBuddy = buddies[currentExponent];

    // Grab bitmap at current level
    BBM currentBitmap = currentBuddy->bitmap;

    // Remember, if we are looking for allocated nodes that implys the bitmap
    // will have that location marked with a '1'

    // Test to see if at the currentExponents level the bit is considered allocated
    if (bbmtst(currentBitmap, base, mem, currentExponent))
    {
        // Location is marked allocated
        // Get new level to search

        // This means we need to go to a lower level
        // WOOP WOOP THIS IS RECURSION
        return exponentsearcher(buddies, base, mem, upper, lower, currentExponent - 1);
    }
    else
    {
        // Location is marked free
        // This means the level we are looking at is not where the alloction occured
        // and that implys any levels below are also not where the allocation is

        // Must be on the level above!
        return currentExponent + 1;
    }
}

// Grabs the size of an allocated block in the freelist
// (It is presumed you can only get the size of allocated blocks)
// @param f = A freelist
// @param *base = The base address of the pool
// @param *mem = Address of the location where the block of memory is
// @param l = Lower exponent bound
// @param u = Upper exponent bound
// @return Returns: int, size of the block
int freelistsize(FreeList f, void *base, void *mem, int l, int u)
{

    // Aliasing
    const int lower = l, upper = u;

    // Used to track current level of exponent on buddies
    int currentExponent = upper;

    // Validating the freelist
    if (!f)
    {
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }

    // Grab the list representation of the freelist
    List *list = (List *)f;

    // Validating the base pool and mem offset address
    if (!base || !mem)
    {
        // Outputting error messsage
        fprintf(stderr, "Base pool or mem offset address is not valid!");
        exit(1);
    }

    // Grab the list of buddies (array of pointers)
    Buddy **buddies = list->buddies;

    // Remember, if we are looking for allocated nodes that implys the bitmap
    // will have that location marked with a '1'
    // Example:
    // Freelist[3] = 0000001
    // Freelist[4] = 0000001
    // Freelist[5] = 0000001
    // Since all buddies in the first position are marked allocated that implies
    // the allocation occured in the Freelist[3] layer as the mem offset accounts for it
    // due to bitmaps work on offsets its nice because you dont have to worry about
    // a stray allocated block caught in this algorithm.
    //
    // For example, If you had a double allocation occur in Freelist[5] on the same
    // buddy bitmap value, it doesn't matter because that second allocation would reflect
    // on the second bitmap value in Freelist[4] which is marked as free. That implies
    // the size you are looking for must be on Freelist[5] because that is the only one
    // where the bitmap is marked 1 when you get the off set of the base and mem

    // Maybe use recursion and save the exponent
    int exponent = exponentsearcher(buddies, base, mem, upper, lower, currentExponent);

    // Returning the exponent!
    return exponent;
}

// Outputting tool of the freelist, useful for debugging
// @param f = A freelist
// @param l = Lower exponent bound
// @param u = Upper exponent bound
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