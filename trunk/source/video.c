#include <stdio.h>
#include <ogcsys.h>
#include <string.h>
#include <gccore.h>
#include <malloc.h>
#include <wiilight.h>

#include "video.h"

static GXRModeObj *rmode = NULL;
static void *frontBuffer = NULL;

// Initialise video
void InitialiseVideo(void)
{
	// Initialise the video system
	VIDEO_Init();

	// Get the preferred video mode from the system (corresponding to the settings in the Wii menu)
	rmode = VIDEO_GetPreferredMode(NULL);

    GX_AdjustForOverscan(rmode, rmode, 32, 24);

	// Allocate memory for the display in the uncached region
	frontBuffer = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    
	// Initialise the console (required for printf)
	console_init(frontBuffer, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth*VI_DISPLAY_PIX_SZ);

	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);

	// Now configure the FrameBuffer (display memory)
	VIDEO_SetNextFramebuffer(frontBuffer);

	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();

	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
}

// Set the console foreground color
void SetConsoleForegroundColor(u8 color, u8 bold)
{
    printf("\x1b[%u;%um", color+30, bold);
    fflush(stdout);
}

// Set the console background color
void SetConsoleBackgroundColor(u8 color, u8 bold)
{
    printf("\x1b[%u;%um", color+40, bold);
    fflush(stdout);
}

// Clears the console setting the background with the specified colour
// The console supports VT terminal escape codes
void ClearConsole(int backgroundCol)
{
	printf("\x1b[4%im", backgroundCol);
	printf("\x1b[2J");
	fflush(stdout);
}

// Print text to the center of the screen
void PrintCenter(char *text, int width)
{
	int textLen = strlen(text);
	int leftPad = (width - textLen) / 2;
	int rightPad = (width - textLen) - leftPad;

	printf("%*s%s%*s", leftPad, " ", text, rightPad, " ");
}

// Print text
void PrintText(char *text, bool clear, bool border, bool center, int backgroundCol, int foregroundCol)
{
	const int consoleLineLength = 72;
	const int defaultBGColor = 0; // Black is the default background color
	const int defaultFGColor = 7; // White is the default background color

	// Clear the screen
	if (clear) ClearConsole(backgroundCol);

	// Set the console background and foreground color
    SetConsoleBackgroundColor(backgroundCol, 0);
    SetConsoleForegroundColor(foregroundCol, 0);

	// Draw the upper border	
	if (border) printf("%*s", consoleLineLength, " ");	

	// Display the text
	if (center)
		PrintCenter(text, consoleLineLength);
	else
		printf(text);
	
	// Draw the lower border	
	if (border) printf("%*s", consoleLineLength, " ");

	// Set the console background and foreground default color
    SetConsoleBackgroundColor(defaultBGColor, 0);
    SetConsoleForegroundColor(defaultFGColor, 0);

    fflush(stdout);
}

// Turn on/off the Wii Light
void WiiLightControl(int state)
{
	switch (state)
	{
		case WIILIGHT_ON:
			WIILIGHT_SetLevel(255);
			WIILIGHT_TurnOn();
			break;

		case WIILIGHT_OFF:
		
		default:
			WIILIGHT_SetLevel(0);
			WIILIGHT_TurnOn();
			WIILIGHT_Toggle();
			break;
	}
}
