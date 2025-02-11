#include <stdlib.h>
#include <string.h>

#include "freelist.h"

// ? A struct to store ______________
// * Should it be called this?
struct freeListList {

} typedef list;

//
// size =
// l =
// u = 
// Returns: FreeList,
FreeList freelistcreate(size_t size, int l, int u) {

}

//
// f =
// l =
// u =
void freelistdelete(FreeList f, int l, int u) {

}

//
// f
// * base =
// e = 
// l =
void * freelistalloc(FreeList f, void * base, int e, int l) {

}

//
// * base =
// * mem = 
// e =
// l =
void freelistfree(FreeList f, void * base, void * mem, int e, int l) {

}

//
// f =
// * base =
// * mem = 
// l =
// u =
// Returns: int,
int freelistsize(FreeList f, void * base, void * mem, int l, int u ) {

}

//
// f =
// l =
// u =
void freelistprint(FreeList f, int l, int u) {

}