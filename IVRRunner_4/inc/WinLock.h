#pragma once

#ifdef __cplusplus
extern "C"{
#endif
#ifndef __WIN_LOCK_H
#define __WIN_LOCK_H
#ifdef WIN32

void sky_lock(long * _flag);
void sky_unlock(long * _flag);

#endif
#endif // __WIN_LOCK_H
#ifdef __cplusplus
}
#endif
