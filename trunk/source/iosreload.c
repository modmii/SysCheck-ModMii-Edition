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

#define MAX_IPC_RETRIES 400

extern void udelay(int us);

void ReloadIOS(int version) {
	__STM_Close();
	u32 numviews;
	u64 titleID = 0x100000000LL;
	raw_irq_handler_t irq_handler;
	u32 counter;

	STACK_ALIGN(tikview,views,4,32);

	titleID |= version;

	ES_GetNumTicketViews(titleID, &numviews);

	ES_GetTicketViews(titleID, views, numviews);

	write32(0x80003140, 0);

	ES_LaunchTitleBackground(titleID, &views[0]);

	__ES_Reset();

	// Mask IPC IRQ while we're busy reloading
	__MaskIrq(IRQ_PI_ACR);
	irq_handler = IRQ_Free(IRQ_PI_ACR);
	while ((read32(0x80003140) >> 16) == 0)
		udelay(1000);

	for (counter = 0; !(read32(0x0d000004) & 2); counter++) {
		udelay(1000);
		
		if (counter >= MAX_IPC_RETRIES)
			break;
	}

	IRQ_Request(IRQ_PI_ACR, irq_handler, NULL);
    __UnmaskIrq(IRQ_PI_ACR);

	__IPC_Reinitialize();
	__ES_Init();
	__STM_Init();
}