#include <stdio.h>
#include "sky_lock.h"

void sky_mutex_lock(SKY_LOCK * lock)
{
#ifdef LINUX
    pthread_mutex_lock(lock);
#elif defined(WIN32)
    sky_lock(lock);
#endif
}

void sky_mutex_unlock(SKY_LOCK *lock)
{
#ifdef LINUX
    pthread_mutex_unlock(lock);
#elif defined(WIN32)
    sky_unlock(lock);
#endif
}
