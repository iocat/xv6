// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file

struct run {
  struct run *next;
};

// the maximum number of free pages
#define MAX_PAGES (PGROUNDUP(PHYSTOP)/PGSIZE)
#define REFCOUNT_INDEX(v) (V2P(v) >> PGSHIFT)
// memory allocator
struct {
    struct spinlock lock;
    int use_lock;
    struct run *freelist;
    // reference counts of the read only pages
    // for copy-on-write kalloc, indexing based on physical page number
    int refs[MAX_PAGES];
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;
  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
    char *p;
    p = (char*)PGROUNDUP((uint)vstart);
    for(; p + PGSIZE <= (char*)vend; p += PGSIZE){
        // set all ref count to 1
        kmem.refs[REFCOUNT_INDEX(p)] = 1;
        kfree(p);
    }
}

static void
freepg_range_check(char *v){
    if((uint)v % PGSIZE || v < end || v2p(v) >= PHYSTOP)
        panic("kfree");
}

// Free the page of physical memory pointed at by v if refcount is 0,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
    freepg_range_check(v);
 
    if(kmem.use_lock)
        acquire(&kmem.lock);
    int count = --kmem.refs[REFCOUNT_INDEX(v)];
    if(count == 0){
        // free the page
        memset(v, 1, PGSIZE);
        struct run* r = (struct run*) v;
        r-> next = kmem.freelist;
        kmem.freelist = r; 
    }else if(count<0){
        cprintf("ref is %d", count);
        panic("ref count cannot be < 0");
    }
    if(kmem.use_lock)
        release(&kmem.lock);
}

// reuses a page (basically increment the ref count)
// returns the new ref count
void
kreuse(char *v){
    freepg_range_check(v);
    if(kmem.use_lock)
        acquire(&kmem.lock);
    kmem.refs[REFCOUNT_INDEX(v)]++;
    if(kmem.use_lock)
        release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory with refcount 1.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char* 
kalloc(void)
{
    struct run *r;
    if(kmem.use_lock)
        acquire(&kmem.lock);
    r = kmem.freelist;
    if(r){
        kmem.freelist = r->next;
        kmem.refs[REFCOUNT_INDEX(r)] = 1;
    }
    if(kmem.use_lock)
        release(&kmem.lock);
    return (char*)r;
}
