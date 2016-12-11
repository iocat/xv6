#include "types.h"

#define SIGKILL	0
#define SIGFPE	1
#define SIGSEGV 2

typedef struct {
    uint addr;
    uint type;
} siginfo_t;

typedef void (*sighandler_t)(int, siginfo_t);
