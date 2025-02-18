#include "bbm.h"
#include "bm.h"
#include "utils.h"

// Gets the size of the bitmap
// size = Size of the whole pool
// e = Size of the block exponent
// Returns: size_t, how many buddies there are in the map
static size_t mapsize(size_t size, int e) {
  size_t blocksize=e2size(e);
  size_t blocks=divup(size,blocksize);
  size_t buddies=divup(blocks,2);
  return buddies;
}

// 
// * base =
// * mem = 
// e =
// Returns: size_t,
static size_t bitaddr(void *base, void *mem, int e) {
  size_t addr=baddrclr(base,mem,e)-base;
  size_t blocksize=e2size(e);
  return addr/blocksize/2;
}

// Creates a buddy bit map
// size = The size of the pool
// e = Size of the block exponent
// Returns: BBM, a tool that can be used to keep track of bits for blocks of memory in freelist
extern BBM bbmcreate(size_t size, int e) {
  return bmcreate(mapsize(size,e));
}

// Deletes a buddy bit map
// b = The buddy bit map to be deleted
extern void bbmdelete(BBM b) {
  bmdelete(b);
}

//
// b =
// * base =
// * mem =
// e =
extern void bbmset(BBM b, void *base, void *mem, int e) {
  bmset(b,bitaddr(base,mem,e));
}

//
// b =
// * base =
// * mem =
// e =
extern void bbmclr(BBM b, void *base, void *mem, int e) {
  bmclr(b,bitaddr(base,mem,e));
}

// Tests the bit at a provided address using the normal bit map function
// b = A buddy bit map
// * base = 
// * mem = 
// e = Size of the block exponent
// Returns: int, 
extern int bbmtst(BBM b, void *base, void *mem, int e) {
  return bmtst(b,bitaddr(base,mem,e));
}

// Outputs the buddy bit map
// b = A buddy bit map
extern void bbmprt(BBM b) { 
  bmprt(b); 
}

//
// * base = 
// * mem =
// e = 
// Returns: void *, 
extern void *baddrset(void *base, void *mem, int e) {
  unsigned int mask=1<<e;
  return base+((mem-base)|mask);
}

//
// * base =
// * mem = 
// e =
// Returns: void *,
extern void *baddrclr(void *base, void *mem, int e) {
  unsigned int mask=~(1<<e);
  return base+((mem-base)&mask);
}

// 
// * base =
// * mem = 
// e =
// Returns: void *, 
extern void *baddrinv(void *base, void *mem, int e) {
  unsigned int mask=1<<e;
  return base+((mem-base)^mask);
}

// Tests a bit at a provided address
// * base = The base of the 
// * mem = The address of the bit
// e =
// Returns: int, 
extern int baddrtst(void *base, void *mem, int e) {
  unsigned int mask=1<<e;
  return (mem-base)&mask;
}
