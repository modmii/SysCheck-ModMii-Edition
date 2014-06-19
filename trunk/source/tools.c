#include <string.h>
#include <gccore.h>
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ogcsys.h>
#include <stdarg.h>
#include <network.h>
#include <ogc/machine/processor.h>
#include <ogc/conf.h>
#include <wiiuse/wpad.h>
#include <ctype.h>

#include "sys.h"
#include "SysMenuInfo.h"

#include "ticket_dat.h"
#include "tmd_dat.h"

#include "mload.h"
#include "title.h"
#include "sha1.h"
#include "gecko.h"
#include "http.h"
#include "gui.h"
#include "languages.h"
#include "fatMounter.h"

// Variables
bool NandInitialized = false;
bool debug = false;

void logfile(const char *format, ...)
{
	if (!debug) return;
	MountSD();
	FILE *f;
	f= fopen("SD:/sysCheckDebug.log", "a");
	if(f == NULL) return;
	va_list args;
	va_start(args, format);
	vfprintf(f,format, args);
	va_end (args);
	fclose(f);
	UnmountSD();
}

/**
* A simple structure to keep track of the size of a malloc()ated block of memory
*/
struct block
{
	u32 size;
	unsigned char *data;
};

const struct block emptyblock = { 0, NULL };

void *allocate_memory(u32 size)
{
	return memalign(32, (size+31)&(~31) );
}

int NandStartup(void)
{
	if (NandInitialized)
		return 1;

	int ret = ISFS_Initialize();

	NandInitialized = (ret == ISFS_OK);

	sleep(1);

	return ret;
}

void NandShutdown(void)
{
	if (!NandInitialized)
		return;

	ISFS_Deinitialize();

	NandInitialized = false;
}

u32 DetectInput(u8 DownOrHeld) {
	u32 pressed = 0;
	u16 gcpressed = 0;
	VIDEO_WaitVSync();
	// Wii Remote (and Classic Controller) take precedence over GC to save time
	if (WPAD_ScanPads() > WPAD_ERR_NONE) // Scan the Wii remotes.  If there any problems, skip checking buttons
	{
		if (DownOrHeld == DI_BUTTONS_DOWN) {
			pressed = WPAD_ButtonsDown(0) | WPAD_ButtonsDown(1) | WPAD_ButtonsDown(2) | WPAD_ButtonsDown(3); //Store pressed buttons
		} else {
			pressed = WPAD_ButtonsHeld(0) | WPAD_ButtonsHeld(1) | WPAD_ButtonsHeld(2) | WPAD_ButtonsHeld(3); //Store pressed buttons
		}
		
		// Convert to wiimote values
		if (pressed & WPAD_CLASSIC_BUTTON_ZR) pressed |= WPAD_BUTTON_PLUS;
		if (pressed & WPAD_CLASSIC_BUTTON_ZL) pressed |= WPAD_BUTTON_MINUS;

		if (pressed & WPAD_CLASSIC_BUTTON_PLUS) pressed |= WPAD_BUTTON_PLUS;
		if (pressed & WPAD_CLASSIC_BUTTON_MINUS) pressed |= WPAD_BUTTON_MINUS;

		if (pressed & WPAD_CLASSIC_BUTTON_A) pressed |= WPAD_BUTTON_A;
		if (pressed & WPAD_CLASSIC_BUTTON_B) pressed |= WPAD_BUTTON_B;
		if (pressed & WPAD_CLASSIC_BUTTON_X) pressed |= WPAD_BUTTON_2;
		if (pressed & WPAD_CLASSIC_BUTTON_Y) pressed |= WPAD_BUTTON_1;
		if (pressed & WPAD_CLASSIC_BUTTON_HOME) pressed |= WPAD_BUTTON_HOME;
		
		if (pressed & WPAD_CLASSIC_BUTTON_UP) pressed |= WPAD_BUTTON_UP;
		if (pressed & WPAD_CLASSIC_BUTTON_DOWN) pressed |= WPAD_BUTTON_DOWN;
		if (pressed & WPAD_CLASSIC_BUTTON_LEFT) pressed |= WPAD_BUTTON_LEFT;
		if (pressed & WPAD_CLASSIC_BUTTON_RIGHT) pressed |= WPAD_BUTTON_RIGHT;
	}

	// Return Classic Controller and Wii Remote values
	if (pressed) return pressed;

	// No buttons on the Wii remote or Classic Controller were pressed
	if (PAD_ScanPads() > PAD_ERR_NONE)
	{
		if (DownOrHeld == DI_BUTTONS_HELD) {
			gcpressed = PAD_ButtonsHeld(0) | PAD_ButtonsHeld(1) | PAD_ButtonsHeld(2) | PAD_ButtonsHeld(3); //Store pressed buttons
		} else {
			gcpressed = PAD_ButtonsDown(0) | PAD_ButtonsDown(1) | PAD_ButtonsDown(2) | PAD_ButtonsDown(3); //Store pressed buttons
		}

		if (gcpressed) {
			// Button on GC controller was pressed
			if (gcpressed & PAD_TRIGGER_Z) pressed |= WPAD_NUNCHUK_BUTTON_Z;
			if (gcpressed & PAD_TRIGGER_R) pressed |= WPAD_BUTTON_PLUS;
			if (gcpressed & PAD_TRIGGER_L) pressed |= WPAD_BUTTON_MINUS;
			if (gcpressed & PAD_BUTTON_A) pressed |= WPAD_BUTTON_A;
			if (gcpressed & PAD_BUTTON_B) pressed |= WPAD_BUTTON_B;
			if (gcpressed & PAD_BUTTON_X) pressed |= WPAD_BUTTON_1;
			if (gcpressed & PAD_BUTTON_Y) pressed |= WPAD_BUTTON_2;
			if (gcpressed & PAD_BUTTON_MENU) pressed |= WPAD_BUTTON_HOME;
			if (gcpressed & PAD_BUTTON_UP) pressed |= WPAD_BUTTON_UP;
			if (gcpressed & PAD_BUTTON_LEFT) pressed |= WPAD_BUTTON_LEFT;
			if (gcpressed & PAD_BUTTON_DOWN) pressed |= WPAD_BUTTON_DOWN;
			if (gcpressed & PAD_BUTTON_RIGHT) pressed |= WPAD_BUTTON_RIGHT;
		}
	}
	return pressed;
}
