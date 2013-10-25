/*-------------------------------------------------------------

iosreload.c -- IOS control

Copyright (C) 2008
Michael Wiedenbauer (shagkur)
Dave Murphy (WinterMute)
Hector Martin (marcan)
JoostinOnline

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1.	The origin of this software must not be misrepresented; you
must not claim that you wrote the original software. If you use
this software in a product, an acknowledgment in the product
documentation would be appreciated but is not required.

2.	Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3.	This notice may not be removed or altered from any source
distribution.

-------------------------------------------------------------*/

// This is just stripped down code from ios.c.
// I didn't do any extra work, I'm just making it faster for this one situation

#if defined(HW_RVL)
#include <gctypes.h>
#include <gcutil.h>

#include <stdio.h>
#include <malloc.h>
#include <ogc/machine/asm.h>
#include <ogc/machine/processor.h>
#include <ogc/cache.h>
#include <ogc/ipc.h>
#include <ogc/stm.h>
#include <ogc/es.h>
#include <ogc/ios.h>
#include <ogc/irq.h>

#define IOS_HEAP_SIZE 0x1000
#define MAX_IPC_RETRIES 400

extern void udelay(int us);

// These two functions deal with the "internal" IOS subsystems that are used by default by libogc
// Other stuff should be inited by the user and deinited by the exit callbacks. The user is also responsible
// for deiniting other stuff before an IOS reload and reiniting them after.
/*static s32 __IOS_InitializeSubsystems(void)
{
	s32 res;
	s32 ret = 0;
	res = __ES_Init();
	if(res < 0) {
		ret = res;
	}
	res = __STM_Init();
	if(res < 0) {
		ret = res;
	}
	return ret;
}

static s32 __IOS_ShutdownSubsystems(void)
{
	s32 res;
	s32 ret = 0;
	res = __STM_Close();
	if(res < 0) ret = res;
	res = __ES_Close();
	if(res < 0) ret = res;
	return ret;
}
*/
static s32 LaunchNewIOS(int version)
{
	u32 numviews;
	s32 res;
	u64 titleID = 0x100000000LL;
	//raw_irq_handler_t irq_handler;
	u32 counter;

	STACK_ALIGN(tikview,views,4,32);
	s32 newversion;

	if(version < 3 || version > 0xFF) {
		return IOS_EBADVERSION;
	}

	titleID |= version;

	res = ES_GetNumTicketViews(titleID, &numviews);
	if(res < 0) {
		return res;
	}
	if(numviews > 4) {
		printf(" GetNumTicketViews too many views: %u\n",numviews);
		return IOS_ETOOMANYVIEWS;
	}
	res = ES_GetTicketViews(titleID, views, numviews);
	if(res < 0) {
		return res;
	}

	write32(0x80003140, 0);

	res = ES_LaunchTitleBackground(titleID, &views[0]);
	if(res < 0) {
		return res;
	}

	__ES_Reset();

	// Mask IPC IRQ while we're busy reloading
	//__MaskIrq(IRQ_PI_ACR);
	//irq_handler = IRQ_Free(IRQ_PI_ACR);

	while ((read32(0x80003140) >> 16) == 0)
		udelay(1000);

	for (counter = 0; !(read32(0x0d000004) & 2); counter++) {
		udelay(1000);
		
		if (counter >= MAX_IPC_RETRIES)
			break;
	}

	//IRQ_Request(IRQ_PI_ACR, irq_handler, NULL);
	//__UnmaskIrq(IRQ_PI_ACR);

	__IPC_Reinitialize();

	newversion = IOS_GetVersion();

	if(newversion != version) {
		return IOS_EMISMATCH;
	}

	return version;
}


s32 ReloadIOS(int version)
{
	int ret = 0;
	int res = 0;

	//res = __IOS_ShutdownSubsystems();
	if(res < 0) {
		ret = res;
	}

	res = __ES_Init();
	if(res < 0) {
		ret = res;
	} else {
		res = LaunchNewIOS(version);
		if(res < 0) {
			ret = res;
			__ES_Close();
		}
	}

	res = __IOS_InitializeSubsystems();
	if(res < 0) {
		ret = res;
	}

	return ret;
}

#endif /* defined(HW_RVL) */
