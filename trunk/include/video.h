#ifndef _VIDEO_H_
#define _VIDEO_H_

// Attributes
#define RESET		0
#define BRIGHT 		1
#define DIM			2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

// Available colors
#define BLACK	0
#define RED		1
#define GREEN	2
#define YELLOW	3
#define BLUE	4
#define MAGENTA	5
#define CYAN	6
#define WHITE	7

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

