#include <stdlib.h>
#include <string.h>

#include "freelist.h"
#include "utils.h"

#define MAX_ORDER 10


// * maybe buddies should be in here...
struct Buddy {

    // Bit map?

    // Storing location pointer
    void * nextNode;

} typedef Buddy;

// ? A struct to store ______________
// * Should it be called this?
struct freeListList {

    // Storing all buddies in the freelist
    

} typedef List;

// Creates a freelist struct and returns the void *
// size = The size of the memory pool to work with
// l = Lower exponent bound
// u = Upper exponent bound
// Returns: FreeList, a struct containing the information on what blocks are free
FreeList freelistcreate(size_t size, int l, int u) {

    // ? Creating the freelist struct
    List newFreelist;

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

}

// Allocates a block of memory from the freelist
// f = A freelist
// * base = The base address of the pool (I believe)
// e = Requested exponent 
// l = Lower exponent bound
// Returns: void *, an address of where the memory was allocated
void * freelistalloc(FreeList f, void * base, int e, int l) {

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
// Returns: int,
int freelistsize(FreeList f, void * base, void * mem, int l, int u ) {

}

// Outputting tool of the freelist, useful for debugging
// f = A freelist
// l = Lower exponenet bound
// u = Upper exponent bound
void freelistprint(FreeList f, int l, int u) {

}