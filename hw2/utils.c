#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// * Gives error, but only because it is an IDE running in windows (:hand_grab_right: :gun_pointing_right: :guy_emptyless:)
// * (Will be built in WSL)
#include <sys/mman.h>

// * Unsure if this is needed, ideally it is used
// * Need to consider if addresses need to be moved 
// * in case of another allocator is created 
// * Note: Cannot be declared as const due to mmap's definition
static void * baseAddress = (void *) 0;

// Used for base in getting values requiring 2^n exponentiation
// static const int baseExponentiation = 2;

// ? Used for bitshifting to get values
static const int bitShiftingExponentiation = 1;

// Number of bits in a byte
static const int bitsPerByte = 8;

// Calls mmap which with a provided size, will map the address space
// of the given range, effectively
// size = size of how much bytes should be mapped
// Returns: void *, the initial address where the memory is mapped into the address space
void * mmalloc(size_t size) {

    // Call to map the memory in the address space with the size
    return mmap(baseAddress, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
}

// Frees the address space from a given range, that is given with
// a pointer to the start and a size of how many addresses should be unmapped
// p = a pointer to the location where the addresses have been mapped (It should be at the base)
// size = the amount of address space that should be unmapped 
void mmfree(void *p, size_t size) {

    // Call to unmap the memory in the address space with a provided size
    // Note: also need to make sure the unmapping was successful, if this call
    // returns 0, then that means the unmapping failed.
    if (munmap(p, size) == -1) {

        // ? Printing error message due to failed unmapping, maybe use ERROR.h?
        // * For now, outputting an exit code along the stderr message is the solution
        fprintf(stderr, "Failed to unmap the address space with the location and size!");
        exit(1);
    }
}

// Divides up the provided 
// n =
// d =
// Returns: size_t
size_t divup(size_t n, size_t d) {

    return 0;
}

// Converts the provided number of bits into the respective amount of bytes
// bits = The number of bits
// Returns: size_t, of the amount bytes from the bits given
size_t bits2bytes(size_t bits) {

    // This line is taking the ceiling of the operation where you take the
    // provided number of bits per how many are in a byte (8). The reason
    // why you take the ceiling is because if you were to provide something
    // bits % 8 != 0 then that means you have to have an additional byte ontop
    return ceil(bits / bitsPerByte);

    // Additional possible solution as log_2(8) = 3, so you can use bit shifting
    // to get the correct value
    // return ceil(bits >> 3);
    // return bits >> 3; // Unsure if you need to cast and ceil in this case
}

// Calculates the size of a provided exponent
// e = The exponent of a 2-base (Ex: 2 ^ (4) <--e)
// Returns: size_t
size_t e2size(int e) {

    // Taking the power of 2^e
    // return pow(baseExponentiation, e);

    // Bitshifting alternative
    return bitShiftingExponentiation << e;

}

// Calculates the exponent from a provided size on 2-base exponentiation
// size = The size provided 
// Returns: int, the exponent that reflects 2^e = size
int size2e(size_t size) {

    // Does simple log arithmetic to get the exponent
    return log2(size);
}

// Sets the bit at a provided address via void pointer
// p = Void pointer of the address that will be set
// bit = What the address is being set with
void bitset(void * p, int bit) {

    // Storing the location of the bit into the void pointer
    // * IS THIS CORRECT?
    p = &bit;
}

//
void bitclr(void * p, int bit) {

}

//
void bitinv(void * p, int bit) {

}

//
int bittst(void * p, int bit) {

    return 0;
}