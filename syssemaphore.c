#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "semaphore.h"

int sys_sem_init(void){
    int semId;
    int n;
    if (argint(0, &semId) < 0 || \
            argint(1, &n) < 0){
        return -1;
    }
    return sem_init(semId, n);
}

int sys_sem_destroy(void){
    int semId;
    if (argint(0, &semId) < 0) {
        return -1;
    }
    return sem_destroy(semId);
}

int sys_sem_wait(void){
    int semId;
    if (argint(0, &semId) < 0){
        return -1;
    }
    return sem_wait(semId);
}

int sys_sem_signal(void){
    int semId;
    if (argint(0, &semId) < 0){
        return -1;
    }
    return sem_signal(semId);
}
