#include <stdlib.h>
#include <string.h>

#include "bm.h"
#include "utils.h"

// Gets the size of the BitMap in bits
// b = a BitMap
// Returns: size_t, The number of bits in the bitmap
static size_t bmbits(BM b)
{

  size_t *bits = b;
  return *--bits;
}

// Gets the size of the BitMap in bytes
// b = a BitMap
// Returns: size_t, The number of bytes in the bitmap
static size_t bmbytes(BM b)
{

  return bits2bytes(bmbits(b));
}

// Determines if the indexed location is trying to access is a valid spot
// b = a BitMap
// i = indexed location of a bit
static void ok(BM b, size_t i)
{
  if (i < bmbits(b))
    return;
  fprintf(stderr, "bitmap index out of range\n");
  exit(1);
}

// Creates a BitMap that can store all sorts of bits
// bits = the size the BitMap should be
// Returns: BM,
extern BM bmcreate(size_t bits)
{
  size_t bytes = bits2bytes(bits);
  size_t *p = mmalloc(sizeof(size_t) + bytes);
  if ((long)p == -1)
    return 0;
  *p = bits;
  BM b = ++p;
  memset(b, 0, bytes);
  return b;
}

// Deletes the BitMap
// b = a BitMap
extern void bmdelete(BM b)
{
  size_t *p = b;
  p--;
  mmfree(p, *p);
}

// Sets a bit on the BitMap at a given location
// b = a BitMap
// i = offset value, simliar to indexing
extern void bmset(BM b, size_t i)
{
  ok(b, i);
  bitset(b + i / bitsperbyte, i % bitsperbyte);
}

// Clears a bit on the BitMap at a given location
// b = a BitMap
// i = offset value, simliar to indexing
extern void bmclr(BM b, size_t i)
{
  ok(b, i);
  bitclr(b + i / bitsperbyte, i % bitsperbyte);
}

// Tests the bit on the BitMap at a given location
// b = a BitMap
// i = offset value, simliar to indexing
extern int bmtst(BM b, size_t i)
{
  ok(b, i);
  return bittst(b + i / bitsperbyte, i % bitsperbyte);
}

// An outputting function of the BitMap
// b = a BitMap
// extern void bmprt(BM b)
// {
//   for (int byte = bmbytes(b) - 1; byte >= 0; byte--)
//     printf("%02x%s", ((char *)b)[byte], (byte ? " " : "\n"));
// }

// Prints out the bitmap in binary as opposed to hexadecimal
extern void bmprt(BM b)
{
  // Converting to a char pointer
  char *bitmapPtr = (char *)b;

  // Loop through all bytes in the bitmap
  for (int byte = bmbytes(b) - 1; byte >= 0; byte--)
  {
    // Loop through the current byte
    for (int bit = 7; bit >= 0; bit--)
    {
      // Shifting the bit to the end and masking to know what
      // value is there
      printf("%d", ((bitmapPtr[byte] >> bit) & 1));
    }

    // Whitespace for the next byte
    printf("%s", (byte ? " " : "\n"));
  }
}
