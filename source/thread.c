#include <stdlib.h>
#include <unistd.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/lwp.h>
#include <ogc/mutex.h>

#include "thread.h"
#include "gui.h"

#define STACKSIZE (1024 * 64) 	// 64KB of dedicated memory for thread
#define PRIORITY 50 			// Medium-ish (0-127 accepted)

lwp_t Cog_Thread;
u8 stack[STACKSIZE] ATTRIBUTE_ALIGN (32);
vu8 done = 0;
u8 Cog_Num = 0;
static u64 Last_Cog_Turn = 0;
static mutex_t GUI_mutex = LWP_THREAD_NULL;
static bool mutex_initialized = false;

void * DrawCogThread(void *arg) {
	while(!done) { // Keep the thread running until done != 0
		GRRLIB_DrawImg(0, 0, tex_ScreenBuf, 0, 1, 1, HEX_WHITE);
		if (CheckTime(Last_Cog_Turn, 25)) {
			Cog_Num++;
			if (Cog_Num > 4) Cog_Num = 0;
			Last_Cog_Turn = gettick();
		}
		GRRLIB_DrawImg(220, 150, tex_Cogs_png[Cog_Num], 0, 1, 1, HEX_WHITE);
		GRRLIB_Render();
	}
	return NULL;
}

inline void InitThread(void) {
	memset (&stack, 0, STACKSIZE);
	LWP_CreateThread (&Cog_Thread, DrawCogThread, NULL, stack, STACKSIZE, PRIORITY);
	usleep(200);
}

inline s32 PauseThread(void) {
	//if(LWP_ThreadIsSuspended(Cog_Thread) == false) return LWP_ALREADY_SUSPENDED;
	return LWP_SuspendThread(Cog_Thread);
}

inline s32 ResumeThread(void) {
	//if(LWP_ThreadIsSuspended(Cog_Thread) == true) return LWP_NOT_SUSPENDED;
	return LWP_ResumeThread(Cog_Thread);
}

// Have yet to test this
inline s32 StopThread(void) {
	done = 1;
	ResumeThread();
	return LWP_JoinThread(Cog_Thread, NULL);
}

inline void InitMutex(void) {
	if (GUI_mutex == LWP_THREAD_NULL) LWP_MutexInit(&GUI_mutex, false);
	mutex_initialized = true;
}
	
inline void DeinitMutex(void) {
	if (GUI_mutex != LWP_THREAD_NULL) LWP_MutexDestroy(GUI_mutex);
	mutex_initialized = false;
}
	
inline void LockMutex(void) {
	if (mutex_initialized) LWP_MutexLock(GUI_mutex);
}

inline void UnlockMutex(void) {
	if (mutex_initialized) LWP_MutexUnlock(GUI_mutex);
}