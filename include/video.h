#ifndef _VIDEO_H_
#define _VIDEO_H_

// Wii Light state
#define WIILIGHT_OFF	0
#define WIILIGHT_ON		1

// Prototypes
void InitialiseVideo(void);
void SetConsoleForegroundColor(u8, u8);
void SetConsoleBackgroundColor(u8, u8);
void ClearConsole(int);
void PrintText(char *, bool, bool, bool, int, int);
void WiiLightControl(int);

#endif

