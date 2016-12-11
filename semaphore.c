#include "types.h"
#include "defs.h"
#include "param.h"
#include "fs.h"
#include "semaphore.h"
#include "x86.h"
#include "date.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

#define NSEM 200 /* defines the number of semaphores */

struct semaphore
{
    int value;
    int active;
    struct spinlock lock;
};

struct semaphore semtable[NSEM]; /* the list of semaphore */

int sem_init(int semId, int n){
    if (semId < 0 ||  semId >= NSEM){
        return -1; /* semaphore's id out of range */
    }
    if (n < 0) {
        return -1; /* improper initial value of semaphore */
    }
    struct semaphore* sem = &semtable[semId];
    acquire(&sem->lock);
    if (sem->active == 1){
        return -1; /* semaphore is already activated */
    }
    sem->active = 1;
    sem->value = n;
    release(&sem->lock);
    return 0;
}


int sem_destroy(int semId){
    if (semId < 0 ||  semId >= NSEM){
        return -1; /* semaphore's id out of range */
    }
    struct semaphore* sem = &semtable[semId];
    sem->active = 0; // NOTE: atomic??? no sem lock
    return 0;
}


int sem_wait(int semId){
    if (semId < 0 ||  semId >= NSEM){
        return -1; /* semaphore's id out of range */
    }
    struct semaphore* sem = &semtable[semId];
    acquire(&sem->lock);
    while (sem->value == 0){ /* keep sleeping until someone increments the sem */
        sleep((void*)semId, &sem->lock);
    }
    sem->value--;
    release(&sem->lock);
    return 0;
}


int sem_signal(int semId){
    if (semId < 0 ||  semId >= NSEM){
        return -1; /* semaphore's id is out of range */
    }
    struct semaphore* sem = &semtable[semId];
    acquire(&sem->lock);
    sem->value++; /* increment the semaphore */
    wakeup((void*)semId);  /* wake up sleeping processes, if there are any */
    release(&sem->lock);
    return 0;
}
