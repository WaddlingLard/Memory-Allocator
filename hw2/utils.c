/**
 * A utility file full of useful functions for balloc
 *
 * @author Brian Wu
 * @version 1.0
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

// Used for bitshifting to get values
static const int bitShiftingExponentiation = 1;

// Number of bits in a byte
static const int bitsInAByte = 8;

// Calls mmap which with a provided size, will map the address space
// of the given range, effectively
// size = size of how much bytes should be mapped
// Returns: void *, the initial address where the memory is mapped into the address space
void *mmalloc(size_t size)
{

    // Call to map the memory in the address space with the size
    void *poolAddr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // Checking to see if the mapping was a success, MAP_FAILED is a constant from mmam.h for a fail pointer
    if (poolAddr == MAP_FAILED)
    {

        // Output error message
        fprintf(stderr, "Memory mapping failed!\n");
        exit(1);
    }

    return poolAddr;
}

// Frees the address space from a given range, that is given with
// a pointer to the start and a size of how many addresses should be unmapped
// * p = a pointer to the location where the addresses have been mapped (It should be at the base)
// size = the amount of address space that should be unmapped
void mmfree(void *p, size_t size)
{

    // Call to unmap the memory in the address space with a provided size
    // Note: also need to make sure the unmapping was successful, if this call
    // returns 0, then that means the unmapping failed.
    int status = munmap(p, size);

    if (status == -1)
    {

        // Output error message
        fprintf(stderr, "Failed to unmap the address space with the location and size!");
        exit(1);
    }
}

// Calculates the size of a provided exponent
// e = The exponent of a 2-base (Ex: 2 ^ (4) <--e)
// Returns: size_t, the size of the exponent
size_t e2size(int e)
{

    // Bitshifting alternative
    return bitShiftingExponentiation << e;
}

// Calculates the exponent from a provided size on 2-base exponentiation
// size = The size provided
// Returns: int, the exponent that reflects 2^e = size
int size2e(size_t size)
{

    int e = 0;
    size_t value = size;

    // Does simple bitshifting arithmetic to get the exponent
    while (value > 1)
    {
        value = value >> 1;
        e++;
    }

    // Need one bump
    // * COULD BE INVALID
    if (e2size(e) != size)
    {
        e++;
    }

    return e;
}

// Divides up the provided two numbers
// n = Numerator
// d = Denominator
// Returns: size_t, The divided result of the two values
size_t divup(size_t n, size_t d)
{

    // Bitshifting logic to calculate the division of n given d
    // * Unsure if correct
    // return n >> size2e(d);

    // Revised version
    // The numerator is being divided but with a slight offset to
    // account for a remainder, acting like rounding up
    return (n + d - 1) / d;
}

// Converts the provided number of bits into the respective amount of bytes
// bits = The number of bits
// Returns: size_t, of the amount bytes from the bits given
size_t bits2bytes(size_t bits)
{

    // This line is taking the ceiling of the operation where you take the
    // provided number of bits per how many are in a byte (8). The reason
    // why you take the ceiling is because if you were to provide something
    // bits % 8 != 0 then that means you have to have an additional byte ontop
    return (bits + 7) / bitsInAByte;
}

// Sets the bit at a provided address via void pointer
// * p = Void pointer of the address that will be set
// bit = Location of the bit in the address
void bitset(void *p, int bit)
{

    // Converting pointer to be a char one
    unsigned char *bitPtr = (unsigned char *)p;

    // Masking bit to set by dereferncing pointer and applying bitwise
    *bitPtr |= (1 << bit);
}

// Clears the bit at a provided address via a void pointer
// * p = Void pointer of the address what will be set
// bit = Location of the bit in the address
void bitclr(void *p, int bit)
{

    // Converting pointer to be a char one
    unsigned char *bitPtr = (unsigned char *)p;

    // Creating bit to clear one at the address
    unsigned int mask = ~(1 << bit);

    // Clearing bit at the location of the dereferenced pointer
    // by using a masked bit that is inverted
    *bitPtr &= mask;
}

// Inverts the bit at a provided address via void pointer
// * p = Void pointer of the location where the inversion will occur
// bit = Location of the bit in the address
void bitinv(void *p, int bit)
{

    // Converting pointer to be a char one
    unsigned char *bitPtr = (unsigned char *)p;

    // Creating bit to invert one at the address
    // unsigned int mask = (1 << bit);

    // Inverting bit at location by dereferencing and applying bitwise
    // *bitPtr ^= mask;

    // You need to directly apply the mask
    *bitPtr ^= (1 << bit);
}

// Tests the bit at the provided address via void pointer
// * p = Void pointer of the bit's address that will be tested
// bit = Location of the bit in the address
// ? Returns: int, 1 for true, 0 for false
int bittst(void *p, int bit)
{

    // Converting pointer to be a char one
    unsigned char *bitPtr = (unsigned char *)p;

    // Testing bit at the location with the 1 using bitwise
    return (*bitPtr >> bit) & 1;
}