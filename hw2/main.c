/**
 * Main method to run balloc
 *
 * @author Brian Wu
 * @version 1.0
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "balloc.h"

// Used purely for testing
#include "utils.h"
#include "deq.h"
// #include "wrapper.c"

void testpool1()
{
    // pool1 Tests

    // Basic pool
    fprintf(stdout, "\nRunning tests for pool 1!\n");
    Balloc pool1 = bcreate(64, 3, 5);

    // Outputting start of pool1 (no operations)
    bprint(pool1);

    // Test 1: Allocation of 16 bytes
    fprintf(stdout, "\nAllocating 16 bytes!\n");
    void *allocation1 = balloc(pool1, 16);

    // What size is the allocation?
    // Test 1.5: Allocation 1 size
    fprintf(stdout, "The size of the allocation is: %d\n", bsize(pool1, allocation1));

    // Allocation should be reflected in the output
    bprint(pool1);

    // Test 2: Allocation of 8 bytes
    fprintf(stdout, "\nNow allocating 8 bytes!\n");
    void *allocation2 = balloc(pool1, 8);

    // What size is the allocation?
    // Test 2.5: Allocation 2 size
    fprintf(stdout, "The size of the allocation is: %d\n", bsize(pool1, allocation2));

    // Allocation should be reflected in the output
    bprint(pool1);

    // Now going to initiate frees
    // Test 3: Freeing first allocation
    fprintf(stdout, "\nNow freeing first allocation!\n");
    bfree(pool1, allocation1);

    // Freed block should be reflected in the output statement
    bprint(pool1);

    // Test 4: Freeing second allocation
    fprintf(stdout, "\nNow freeing second allcation\n");
    bfree(pool1, allocation2);

    // Freed block should have returned to constructed state
    bprint(pool1);

    // Deleting pool
    // Test 5: Delete pool
    bdelete(pool1);

    fprintf(stdout, "\nPool1 tests complete!\n");

    // Tests complete
    return;
}

void testpool2()
{
    // pool2 tests

    // Bigger pool
    fprintf(stdout, "\nRunning tests for pool2!\n");
    Balloc pool2 = bcreate(4096, 8, 11);

    // Outputting start of pool2 (no operations)
    bprint(pool2);

    // Test 6: Multiple Allocations
    fprintf(stdout, "\nAllocation of 1024, 512, and 256 in that order!\n");
    void *allocation1 = balloc(pool2, 1024);
    void *allocation2 = balloc(pool2, 512);
    void *allocation3 = balloc(pool2, 256);

    // Output should reflect those allocations
    bprint(pool2);

    // Test 7: Freeing the allocations in random order
    fprintf(stdout, "\nFreeing all allocations in jumbled order!\n");
    bfree(pool2, allocation3);
    bfree(pool2, allocation1);
    bfree(pool2, allocation2);

    // Output should return to constructed state
    bprint(pool2);

    // Tests complete
    // Test 8: Delete list
    bdelete(pool2);

    fprintf(stdout, "\nPool2 tests complete!\n");

    return;
}

void testpool3()
{
    // pool3 tests

    // Small pool
    fprintf(stdout, "\nRunning tests for pool3!\n");
    Balloc pool3 = bcreate(32, 1, 4);

    // Outputting start of pool3 (no operations)
    bprint(pool3);

    // Test 9: Multiple smallest size allocations
    fprintf(stdout, "\nAllocations of size 2, 4 times!\n");
    void *allocation1 = balloc(pool3, 2);
    void *allocation2 = balloc(pool3, 2);
    void *allocation3 = balloc(pool3, 2);
    void *allocation4 = balloc(pool3, 2);

    // Output should reflect those allocations
    bprint(pool3);

    // Test 10: Free allocations in jumbled order
    fprintf(stdout, "\nFreeing all allocations in reversed order!\n");
    bfree(pool3, allocation4);
    bfree(pool3, allocation3);
    bfree(pool3, allocation2);
    bfree(pool3, allocation1);

    // Output should return to constructed state
    bprint(pool3);

    // Tests complete
    // Test 11: Delete list
    bdelete(pool3);

    fprintf(stdout, "\nPool3 tests complete!\n");

    // Tests complete
    return;
}

int main()
{

    fprintf(stdout, "\nRunning some balloc tests\n");

    testpool1();
    testpool2();
    testpool3();

    // RUNNING DEQ TEST PORTION
    fprintf(stdout, "Running a simple deq test\n");

    // Going though all the basic operations
    Deq testdeq = deq_new();

    // Testing put operation
    deq_head_put(testdeq, "hello");

    // Testing ith operation
    Data ith = deq_head_ith(testdeq, 0);

    // Can you find the ith value?
    int result1 = strcmp((char *)ith, "hello");

    // Testing get operation
    Data get = deq_head_get(testdeq);

    // Can you get the value?
    int result2 = strcmp((char *)get, "hello");

    // Testing rem operation
    deq_head_put(testdeq, "friend");
    Data rem = deq_head_rem(testdeq, "friend");

    // Can you rem the value?
    int result3 = strcmp((char *)rem, "friend");

    // Deleting the deq
    deq_del(testdeq, 0);

    // Sorry this is rather hasty, but it will do.
    fprintf(stdout, "Results from the tests: %d %d %d\n", result1, result2, result3);

    return 0;
}
