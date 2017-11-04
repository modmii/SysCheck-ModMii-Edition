#ifndef __TOOLS_H__
#define __TOOLS_H__

#ifdef __cplusplus
extern "C"
{
#endif

// Values for DetectInput
#define DI_BUTTONS_HELD		0
#define DI_BUTTONS_DOWN		1

#define CHECK_ARG(X)		(!strncmp((X), argv[i], sizeof((X))-1))
#define CHECK_ARG_VAL(X)	(argv[i] + sizeof((X))-1)

#define CheckTime()			while(!(ticks_to_millisecs(diff_ticks(current_time, gettime())) > 450))
#define UpdateTime()		current_time = gettime();

typedef struct {
	bool	AHB_At_Start;
	bool	debug;
	int		skipIOSlist[512];
	int		skipIOScnt;
	bool	USB;
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
