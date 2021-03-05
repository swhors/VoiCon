#pragma once

#ifndef __SKY_HOOK_H
#define __SKY_HOOK_H
#ifdef __cplusplus
extern "C"{
#endif

#ifdef LINUX

#include <pthread.h>
typedef pthread_mutex_t SKY_LOCK, * P_SKY_LOCK;
#define SKY_MUTEX_INI PTHREAD_MUTEX_INITIALIZER

#elif defined WIN32

#include "WinLock.h"
typedef long SKY_LOCK, * P_SKY_LOCK;
#define SKY_MUTEX_INI 0

#endif

void sky_mutex_lock(SKY_LOCK * lock);
void sky_mutex_unlock(SKY_LOCK *lock);


#ifdef __cplusplus
}
#endif
#endif
