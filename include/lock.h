#pragma once

#include <types.h>

void acquire(Lock *lock);
void release(Lock *lock);
int holding(Lock *lock);