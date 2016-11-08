#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "thread.h"

int
sys_clone(void){
    int func;
    int arg;
    int stack;
    if (argint(0, &func) < 0 \
        || argint(1, &arg) < 0 \
        || argint(2, &stack) < 0){
        return -1;
    }
    return clone(func, arg, stack);
}

int
sys_join(void){
    int pid;
    int stack;
    int retval;
    if(argint(0, &pid) < 0 \
        || argint(1, &stack) < 0 \
        || argint(2, &retval) < 0) {
        return -1;
    }
    return join(pid, stack, retval);
}

int
sys_texit(void){
    int retval;
    if (argint(0, &retval) < 0){
        return -1;
    }
    texit(retval);
    return 0;
}

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
