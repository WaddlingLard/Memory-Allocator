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

    // Balloc pool1 = bcreate(32, 5, 5);
    // Balloc pool2 = bcreate(64, 5, 5);
    Balloc pool3 = bcreate(32, 4, 5);

    // bprint(pool1);

    // printf("\n");

    // bprint(pool2);

    // printf("\n");

    bprint(pool3);

    // printf("Location of pool1 %p\n", pool1);

    // Running bcreate tests

    // Running balloc tests

    // Running bfree tests

    // Running bdelete tests

    // Running bprint tests

    // Commented out tests (should intentionally fail)

    // Random Tests

    return 0;
}
