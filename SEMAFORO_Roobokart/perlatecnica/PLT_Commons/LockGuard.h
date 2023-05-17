#ifndef __PLT_LOCK_GUARD_H__
#define __PLT_LOCK_GUARD_H__
 
#include <mbed.h>

class LockGuard 
{
    public:
    LockGuard(Mutex & mutex) : my_mutex(mutex) 
    {
        my_mutex.lock();
    } 
    
    ~LockGuard() 
    {
        my_mutex.unlock();
    }

    LockGuard(const LockGuard &) = delete;
    LockGuard(LockGuard &&) = delete;

    LockGuard & operator=(const LockGuard &) = delete;
    LockGuard & operator=(LockGuard &&) = delete;
 
    private:
    Mutex & my_mutex;
};
 
#endif 