#include <stdlib.h>
#include <string.h>

#include "freelist.h"
#include "utils.h"
#include "bbm.h"

#define MAX_ORDER 10

// A singly linked list that represents the blocks of memory for the index and its size
struct Buddy {

    // Storing location pointer for a singly linked list
    struct Buddy * nextBuddy;
    
    //

} typedef Buddy;

// A freelist struct
struct freeListList {

    // Holding GLOBAL bitmap in the freelist
    void * BitMap;

    // Storing all the buddies up to a maximum of 10
    Buddy * buddies[MAX_ORDER];

    // Another item that will set the [0] l: lowest power, [1] u: highest power, [2] r: number of buddies (u - l + 1)
    // * maybe not necessary
    // int managementData[3];

} typedef List;

// Creates a freelist struct and returns the void *
// size = The size of the memory pool to work with
// l = Lower exponent bound
// u = Upper exponent bound
// Returns: FreeList, a struct containing the information on what blocks are free
FreeList freelistcreate(size_t size, int l, int u) {

    // Variables should be presumably safe as they are verified in bcreate()
    // Used for aliasing
    const size_t sizeRequested = size;
    const int lower = l, upper = u;
    const int range = lower - upper;

    // Creating the freelist struct
    List newFreelist;

    // Creating a bit map
    const BBM newBitMap = bbmcreate(size, upper);

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

    for (int i = lower; i <= upper; i++) {
        if (i == upper) {
            // highest value block

            // Set the pointers 
            Buddy * biggestBuddy = newFreelist.buddies[i];
            

        } else {
            // Lower levels do not start with a pointer
            Buddy * buddy = newFreelist.buddies[i];
            buddy->nextBuddy = NULL;
        }
    }

    // Saving an address to store the list
    void * freelistAddr = mmalloc(sizeof(List));

    // Copy the memory off the stack from newFreelist to an address (freeListAddr)
    memcpy(freelistAddr, &newFreelist, sizeof(List));

    // Returning the address of the created freelist
    return freelistAddr;

}

// Deletes a freelist
// f = A freelist
// l = Lower exponent bound
// u = Upper exponent bound
void freelistdelete(FreeList f, int l, int u) {

    // Validating the freelist
    if (!f) {
        
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }

    // Grabbing the free list
    const List * freelist = (List *)f;

    // Presuming l and u are correct

    // Freeing the freelist


    // mfree()

    // Freelist should be bon voyage!
    return;

}

// Allocates a block of memory from the freelist
// f = A freelist
// * base = The base address of the pool (I believe)
// e = Requested exponent 
// l = Lower exponent bound
// Returns: void *, an address of where the memory was allocated
void * freelistalloc(FreeList f, void * base, int e, int l) {

    // Validating the freelist
    if (!f) {
        
        // Outputting error message
        fprintf(stderr, "Freelist is not valid!");
        exit(1);
    }


}

// Frees a block of memory in the freelist
// f = A freelist
// * base = The base addess of the pool
// * mem = Address of the location where the allocated block is
// e = Requested exponent
// l = Lower exponent bound
void freelistfree(FreeList f, void * base, void * mem, int e, int l) {

}

// Grabs the size of a block in the freelist
// f = A freelist
// * base = The base address of the pool
// * mem = Address of the location where the block of memory is
// l = Lower exponent bound
// u = Upper exponent bound
// Returns: int, size of the block
int freelistsize(FreeList f, void * base, void * mem, int l, int u ) {

}

// Outputting tool of the freelist, useful for debugging
// f = A freelist
// l = Lower exponent bound
// u = Upper exponent bound
void freelistprint(FreeList f, int l, int u) {

}