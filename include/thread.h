#ifndef __THREAD_H__
#define __THREAD_H__

extern vu8 done;
extern lwp_t Cog_Thread;

void InitThread(void);
void * DrawCogThread(void *arg);
s32 PauseThread(void);
s32 ResumeThread(void);
#endif