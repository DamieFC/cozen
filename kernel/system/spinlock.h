#ifndef SPINLOCK_H
#define SPINLOCK_H

void spinlock_acquire(int lock);
int spinlock_release();

#endif
