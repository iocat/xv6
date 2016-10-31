#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "thread.h"
#include "spinlock.h"
#include "x86.h"
#include "date.h"
#include "mmu.h"
#include "proc.h"

// creates a new thread that lives in the same address space
// as the current thread
int
clone(void *(*func) (void *), void *arg, void *stack){
    // TODO
    return -1;
}

// waits for a particular child thread to finishes its execution
int
join(int pid, void **stack, void **retval){
    // TODO: check if there is a corresponding child with pid
    // TODO: implement this
    return -1;
}

// finishes the execution and allows the parent process to collect
// the returned value
void
texit(void *retval){
    // TODO:
    return ;
}
