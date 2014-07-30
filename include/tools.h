#ifndef __TOOLS_H__
#define __TOOLS_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Values for DetectInput
#define DI_BUTTONS_HELD		0
#define DI_BUTTONS_DOWN		1

#define CHECK_ARG(X) (!strncmp((X), argv[i], sizeof((X))-1))
#define CHECK_ARG_VAL(X) (argv[i] + sizeof((X))-1)

typedef struct {
	bool	forceNoAHBPROT;
	bool	debug;
	int		skipIOSlist[513];
	int		skipIOScnt;
} arguments_t;

extern arguments_t arguments;

void logfile(const char *format, ...);
void *allocate_memory(u32 size);
int NandStartup(void);
void NandShutdown(void);
u32 DetectInput(u8 DownOrHeld);
void sort(u64 *titles, u32 cnt);
void formatDate(u32 date, char ReportBuffer[200][100]);

#ifdef __cplusplus
}
#endif

#endif
