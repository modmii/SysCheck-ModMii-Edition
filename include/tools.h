#ifndef __TOOLS_H__
#define __TOOLS_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Values for DetectInput
#define DI_BUTTONS_HELD		0
#define DI_BUTTONS_DOWN		1

void logfile(const char *format, ...);
void *allocate_memory(u32 size);
int NandStartup(void);
void NandShutdown(void);
u32 DetectInput(u8 DownOrHeld);

#ifdef __cplusplus
}
#endif

#endif
