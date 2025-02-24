#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "balloc.h"

// Used purely for testing
#include "utils.h"

// Tests for bcreate

// Tests for balloc

// Tests for bfree

// Tests for bdelete

// Tests for bprint

// Tests for fail conditions

int main()
{

    Balloc pool1 = bcreate(64, 3, 5);
    Balloc pool2 = bcreate(256, 5, 5);
    // Balloc pool3 = bcreate(32, 4, 5);

    // Should create 2^5 = 32 (8 Blocks)
    // Balloc pool5 = bcreate(256, 3, 5);

    // Before allocation
    fprintf(stdout, "Before allocation!\n");
    bprint(pool1);
    // bprint(pool2);

    // bprint(pool3);
    // bprint(pool4);
    // bprint(pool5);

    void *allocation1 = balloc(pool1, 16);
    void *allocation2 = balloc(pool1, 8);

    // balloc(pool1, 16);
    // balloc(pool1, 32);
    // balloc(pool1, 32);

    // fprintf(stdout, "Location of free mem: %p\n", allocation2);
    // fprintf(stdout, "Location of free mem: %p\n", balloc(pool2, 32));
    // fprintf(stdout, "Location of free mem: %p\n", balloc(pool2, 32));
    // fprintf(stdout, "Location of free mem: %p\n", balloc(pool2, 32));
    // fprintf(stdout, "Location of free mem: %p\n", balloc(pool2, 32));
    // fprintf(stdout, "Location of free mem: %p\n", balloc(pool2, 32));
    // fprintf(stdout, "Size of allocation (Expecting 8): %d\n", (int)bsize(pool1, allocation1));
    // fprintf(stdout, "Size of allocation (Expecting 4): %d\n", (int)bsize(pool1, allocation2));

    // After allocation & before freeing
    fprintf(stdout, "After allocation and before freeing!\n");
    bprint(pool1);
    // bprint(pool2);

    bfree(pool1, allocation1);
    // bfree(pool1, allocation2);

    // After freeing
    fprintf(stdout, "After freeing!\n");
    bprint(pool1);

    bdelete(pool1);
    bdelete(pool2);
    // bdelete(pool3);
    // bdelete(pool4);
    // bdelete(pool5);

    // Presumably the same location as pool3 (Unsure if that should be the case)
    // Balloc pool4 = bcreate(32, 4, 5);

    // Wow, it is the same address...
    // bprint(pool4);

    // UTILS TESTS

    // fprintf(stdout, "Size of 2 as an exponent (Expecting 1): Actual %d\n", size2e(2));

    // size_t blocksize = e2size(5);
    // size_t blocks = divup(32, blocksize);
    // size_t buddies = divup(blocks, 2);

    // fprintf(stdout, "Number of buddies: %ld\n", buddies);

    // fprintf(stdout, "64 bits to bytes is... (Expecting 8): Actual %ld\n", bits2bytes(64));

    // Running bcreate tests

    // Running balloc tests

    // Running bfree tests

    // Running bdelete tests

    // Running bprint tests

    // Commented out tests (should intentionally fail)

    // Random Tests

    return 0;
}
