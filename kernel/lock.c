#include <cpu.h>
#include <proc.h>
#include <lock.h>
#include <intr.h>

void acquire(Lock *lock)
{
    while (lock->locked) {
        sleep(lock);
    }
    lock->locked = 1;
    lock->procp  = cpu.procp;
}

void release(Lock *lock)
{
    lock->locked = 0;
    lock->procp  = 0;
    wakeup(lock);
}

int holding(Lock *lock)
{
    return lock->locked && (lock->procp == cpu.procp);
}