#ifndef __THREAD_H__
#define __THREAD_H__

#include <ogc/lwp.h>

extern lwp_t Cog_Thread;
extern u8 Cog_Num;
extern vu8 done;

void InitThread(void);
void * DrawCogThread(void *arg);
s32 PauseThread(void);
s32 ResumeThread(void);
s32 StopThread(void);
void InitMutex(void);
void DeinitMutex(void);
void LockMutex(void);
void UnlockMutex(void);

#endif