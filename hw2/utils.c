#include <stdio.h>

#include <sys/mmap.h>

// * Unsure if this is needed, ideally it is used 
// static int baseAddress = 0;

// Calls mmap which with a provided size, will map the address space
// of the given range, effectively
// size = size of how much bytes should be mapped
// Returns: void *, the initial address where the memory is mapped into the address space
void * mmalloc(size_t size) {

    // Call to map the memory in the address space with the size
    return mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
}

// Frees the address space from a given range, that is given with
// a pointer to the start and how far it shold bd
void mmfree(void *p, size_t size) {

}