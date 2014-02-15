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