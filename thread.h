
#define NSEM 200 /* defines the number of semaphores */

struct semaphore
{
    int value;
    int active;
    struct spinlock lock;
}
