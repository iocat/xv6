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

void thread_init(struct* proc np) {
    np->athread = 1; // mark as a thread
    np->tstack = stack; // save the stack address
    np->pgdir = proc->pgdir; // share address space
    np->sz = proc->sz; // same program size
    np->parent = proc;
    np->killed = 0;
    np->file = proc->file; // share same files ??
    np->cwd = proc->cwd; // share cwd ??
    safestrcpy(proc->name, np->name, sizeof(np->name));
}

// creates a new thread that lives in the same address space
// as the parent process
int
clone(void *(*func) (void *), void *arg, void *stack){
    // stack check
    if ((uint) stack % PGSIZE != 0) {
        return -1; /* user's stack is not aligned properly as a page */
    }

    if((np = allocproc())== 0 ){
        return -1; /* cannot allocate a new process */
    }
    // initialize the user thread
    thread_init(np);

    // Set up the user thread's stack
    np->tf->esp = (uint) stack + PGSIZE - 2*sizeof(int);
    ((uint*) np->tf->esp)[0] = 0xFFFFFFFF; /* return address */
    ((uint*) np->tf->esp)[1] = (uint) arg; /* set up arg for func */
    np->tf->eip = (uint) func; /* thread starts at func */

    // Allow the user thread to run
    acquire(&ptable.lock);
    np->state = RUNNABLE;
    release(&ptable.lock);
    return np->pid;
}

// waits for a particular child thread to finishes its execution
int
join(int pid, void **stack, void **retval){
    for (;;){
        int hasChild = 0;
        struct *proc child;
        acquire(&ptable.lock);
        for (int i = 0; i < NPROC; i ++) {
            if (ptable.proc[i].parent == proc && \
                ptable.proc[i].pid == pid){
                child = &ptable.proc[i];
                hasChild = 1;
                break;
            }
        }
        if (hasChild == 0){ /* error: no child with pid is found */
            release(&ptable.lock);
            return -1;
        }else if (child->state == ZOMBIE){
            *retval = child->tretval;
            *stack = child->tstack;
            // Funeral here, cough, clean up here
            child->athread = 0;
            child->tretval = 0;
            child->tstack = 0;
            child->sz = 0;
            child->pgdir = 0;
            kfree(child->kstack);
            child->kstack = 0;
            child->state = UNUSED;
            child->parent = 0;
            child->killed = 0;
            child->file = 0;
            child->cwd = 0;
            child->name[0] = 0;
            release(&ptable.lock);
            return 0;
        }else{ // child did not texit()
            sleep(proc->pid, &ptable.lock);
            // wake up, go find the zombie children
        }
    }
    return 0;
}

// finishes the execution and allows the parent process/thread to collect
// the returned value
void
texit(void *retval){
    if(proc->athread == 0){ // do not allow a normal process to texit()
        return -1;
    }
    proc->tretval = retval;
    acquire(&ptable.lock);
    wakeup1(proc->parent);
    // Pass abandoned children of this thread to init.
    for(struct proc* p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(p->parent == proc){
            p->parent = initproc;
            if(p->state == ZOMBIE){
                wakeup1(initproc);
            }
        }
    }

    // Jump into the scheduler, never to return.
    proc->state = ZOMBIE;
    sched();
    panic("zombie attacks!");
}
