#include <stdlib.h>
#include <wiiuse/wpad.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/mutex.h>

#include "tahoma_ttf.h"

#include "background.h"
#include "Checkicon.h"
#include "Deleteicon.h"
#include "Refreshicon.h"
#include "WiiButtonA.h"
#include "WiiButtonHome.h"
#include "WiiButtonMinus.h"
#include "WiiButtonPlus.h"
#include "WiiDpadLeft.h"
#include "WiiDpadRight.h"
#include "loadingbargrey.h"
#include "loadingbarblue.h"
#include "window.h"
#include "Cog1.h"
#include "Cog2.h"
#include "Cog3.h"
#include "Cog4.h"
#include "Cog5.h"
#include "languages.h"
#include "gui.h"
#include "gecko.h"
#include "sys.h"
#include "fatMounter.h"
#include "thread.h"

GRRLIB_ttfFont *myFont;
GRRLIB_texImg *tex_background_png;
GRRLIB_texImg *tex_Checkicon_png;
GRRLIB_texImg *tex_Deleteicon_png;
GRRLIB_texImg *tex_Refreshicon_png;
GRRLIB_texImg *tex_WiiButtonA_png;
GRRLIB_texImg *tex_WiiButtonHome_png;
GRRLIB_texImg *tex_WiiButtonMinus_png;
GRRLIB_texImg *tex_WiiButtonPlus_png;
GRRLIB_texImg *tex_WiiDpadLeft_png;
GRRLIB_texImg *tex_WiiDpadRight_png;
GRRLIB_texImg *tex_loadingbargrey_png;
GRRLIB_texImg *tex_loadingbarblue_png;
GRRLIB_texImg *tex_window_png;
GRRLIB_texImg *tex_Cogs_png[5];
GRRLIB_texImg *tex_ScreenBuf;

//mutex_t lock_thread;

typedef struct map_entry
{
	char name[8];
	u8 hash[20];
} __attribute__((packed)) map_entry_t;
static char contentMapPath[] ATTRIBUTE_ALIGN(32) = "/shared1/content.map";
static const u8 WIIFONT_HASH[]		= {0x32, 0xb3, 0x39, 0xcb, 0xbb, 0x50, 0x7d, 0x50, 0x27, 0x79, 0x25, 0x9a, 0x78, 0x66, 0x99, 0x5d, 0x03, 0x0b, 0x1d, 0x88};
static const u8 WIIFONT_HASH_KOR[]	= {0xb7, 0x15, 0x6d, 0xf0, 0xf4, 0xae, 0x07, 0x8f, 0xd1, 0x53, 0x58, 0x3e, 0x93, 0x6e, 0x07, 0xc0, 0x98, 0x77, 0x49, 0x0e};
u8 *systemFont;
s32 systemFontSize = 0;


bool loadSystemFont(bool korean)
{
	u8 *contentMap = NULL;
	u32 mapsize = 0;
	int i = 0;
	s32 ret = 0;
	
	ret = read_file_from_nand(contentMapPath, &contentMap, &mapsize);
	if(ret < 0)
		return false;

	int fileCount = mapsize / sizeof(map_entry_t);

	map_entry_t *mapEntryList = (map_entry_t *) contentMap;

	for (i = 0; i < fileCount; i++)
	{
		if (memcmp(mapEntryList[i].hash, korean ? WIIFONT_HASH_KOR : WIIFONT_HASH, 20) != 0)
				continue;

		// Name found, load it and unpack it
		char font_filename[32] ATTRIBUTE_ALIGN(32);
		snprintf(font_filename, sizeof(font_filename), "/shared1/%.8s.app", mapEntryList[i].name);

		u8 *fontArchive = NULL;
		u32 filesize = 0;
		
		ret = read_file_from_nand(font_filename, &fontArchive, &filesize);
		if(ret < 0)
			return false;

		const U8Header *u8Archive = (U8Header *) fontArchive;
		const U8Entry *fst = (const U8Entry *) (((const u8 *) u8Archive) + u8Archive->rootNodeOffset);

		if(fst[0].numEntries < 1)
		{
			free(fontArchive);
			continue;
		}

		if(systemFont)
			free(systemFont);

		systemFontSize = fst[1].fileLength;
		systemFont = allocate_memory(systemFontSize);
		if(!systemFont)
		{
			free(fontArchive);
			continue;
		}

		memcpy(systemFont, fontArchive + fst[1].fileOffset, systemFontSize);

		free(fontArchive);
		free(contentMap);
		MountSD();
		static char buf[128];
		snprintf(buf, 128, "sd:/test.ttf");
		FILE *fp = NULL;
		fp = fopen(buf, "wb");
		if (fp)
		{
			fwrite(systemFont, 1, systemFontSize, fp);
			fclose(fp);
		}
		UnmountSD();
		
		return true;
	}

	free(contentMap);

	return false;
}

int initGUI(void) {
	// Initialise the Graphics & Video subsystem
    GRRLIB_Init();

    // Initialise the Wiimotes
    WPAD_Init();
	
	//Load Images
	myFont = GRRLIB_LoadTTF(tahoma_ttf, tahoma_ttf_size);
	tex_background_png = GRRLIB_LoadTexturePNG(background);
	tex_Checkicon_png = GRRLIB_LoadTexturePNG(Checkicon);
	tex_Deleteicon_png = GRRLIB_LoadTexturePNG(Deleteicon);
	tex_Refreshicon_png = GRRLIB_LoadTexturePNG(Refreshicon);
	tex_WiiButtonA_png = GRRLIB_LoadTexturePNG(WiiButtonA);
	tex_WiiButtonHome_png = GRRLIB_LoadTexturePNG(WiiButtonHome);
	tex_WiiButtonMinus_png = GRRLIB_LoadTexturePNG(WiiButtonMinus);
	tex_WiiButtonPlus_png = GRRLIB_LoadTexturePNG(WiiButtonPlus);
	tex_WiiDpadLeft_png = GRRLIB_LoadTexturePNG(WiiDpadLeft);
	tex_WiiDpadRight_png = GRRLIB_LoadTexturePNG(WiiDpadRight);
	tex_loadingbargrey_png = GRRLIB_LoadTexturePNG(loadingbargrey);
	tex_loadingbarblue_png = GRRLIB_LoadTexturePNG(loadingbarblue);
	tex_window_png = GRRLIB_LoadTexturePNG(window);
	tex_Cogs_png[0] = GRRLIB_LoadTexturePNG(Cog1);
	tex_Cogs_png[1] = GRRLIB_LoadTexturePNG(Cog2);
	tex_Cogs_png[2] = GRRLIB_LoadTexturePNG(Cog3);
	tex_Cogs_png[3] = GRRLIB_LoadTexturePNG(Cog4);
	tex_Cogs_png[4] = GRRLIB_LoadTexturePNG(Cog5);
	tex_ScreenBuf = GRRLIB_CreateEmptyTexture(rmode->fbWidth, rmode->efbHeight);
	InitThread();
	//LWP_MutexInit(&lock_thread, false);
	
	return 0;
}

void deinitGUI(void) {
	StopThread();
	GRRLIB_FreeTTF(myFont);
	GRRLIB_FreeTexture(tex_background_png);
	GRRLIB_FreeTexture(tex_Checkicon_png);
	GRRLIB_FreeTexture(tex_Deleteicon_png);
	GRRLIB_FreeTexture(tex_Refreshicon_png);
	GRRLIB_FreeTexture(tex_WiiButtonA_png);
	GRRLIB_FreeTexture(tex_WiiButtonHome_png);
	GRRLIB_FreeTexture(tex_WiiButtonMinus_png);
	GRRLIB_FreeTexture(tex_WiiButtonPlus_png);
	GRRLIB_FreeTexture(tex_WiiDpadLeft_png);
	GRRLIB_FreeTexture(tex_WiiDpadRight_png);
	GRRLIB_FreeTexture(tex_loadingbargrey_png);
	GRRLIB_FreeTexture(tex_loadingbarblue_png);
	GRRLIB_FreeTexture(tex_window_png);
	GRRLIB_FreeTexture(tex_Cogs_png[0]);
	GRRLIB_FreeTexture(tex_Cogs_png[1]);
	GRRLIB_FreeTexture(tex_Cogs_png[2]);
	GRRLIB_FreeTexture(tex_Cogs_png[3]);
	GRRLIB_FreeTexture(tex_Cogs_png[4]);
	GRRLIB_FreeTexture(tex_ScreenBuf);
	GRRLIB_Exit();
	return;
}

int printError(const char* msg) {
	int i;
	PauseThread();
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Deleteicon_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	
	return 0;
}

int printSuccess(const char* msg) {
	int i;
	PauseThread();
	//GRRLIB_ClearTex(tex_ScreenBuf);
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Checkicon_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}

int printLoading(const char* msg) {
	//int i;
	//ResumeThread();
	u64 current_ticks = gettick();
	PauseThread();
	//GRRLIB_DrawImg(256, 112, tex_Refreshicon_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	CopyBuf();
	ResumeThread();
	//for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
	while(!CheckTime(current_ticks, 250)) {
		//DrawBuf();
		//DrawCog();
		//GRRLIB_Render();
	}
	return 0;
}

int printSelectIOS(const char* msg, const char* ios) {
	int i;
	PauseThread();
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	//GRRLIB_DrawImg(256, 112, tex_Refreshicon_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	GRRLIB_PrintfTTF((640-strlen(ios)*9)/2, 300, myFont, ios, 20, HEX_WHITE);
	GRRLIB_DrawImg(222, 286, tex_WiiDpadLeft_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(370, 286, tex_WiiDpadRight_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_DrawImg(310, 388, tex_WiiButtonPlus_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(335-(strlen(BUT_Update)*7.8)/2, 425, myFont, BUT_Update, 14, HEX_WHITE);
	CopyBuf();
	ResumeThread();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}

int printLoadingBar(const char* msg, const f32 percent) {
	int loaded, notloaded;
	u64 current_ticks = gettick();
	
	loaded = 536.0/100.0*percent;
	if (loaded > 536) loaded = 536;
	
	notloaded = 536 - loaded;
	
	//int i;
	PauseThread();
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	//GRRLIB_DrawImg(256, 112, tex_Refreshicon_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	GRRLIB_DrawPart(52, 340, 0, 0, loaded, 36, tex_loadingbarblue_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawPart(52+loaded, 340, loaded, 0, notloaded, 36, tex_loadingbargrey_png, 0, 1, 1, HEX_WHITE);
	CopyBuf();
	ResumeThread();
	//for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
	while(!CheckTime(current_ticks, 250)) {
		//DrawBuf();
		//DrawCog();
		//GRRLIB_Render();
	}
	return 0;
}
	
int printEndSuccess(const char* msg) {
	int i;
	PauseThread();
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Checkicon_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	GRRLIB_DrawImg(302, 292, tex_WiiButtonA_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_DrawImg(52, 340, tex_loadingbarblue_png, 0, 1, 1, HEX_WHITE);

	GRRLIB_DrawImg(144, 388, tex_WiiButtonMinus_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(160-(strlen(BUT_Shutoff)*7.8)/2, 425, myFont, BUT_Shutoff, 14, HEX_WHITE);
	
	GRRLIB_DrawImg(302, 386, tex_WiiButtonHome_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(320-(strlen(BUT_HBC)*7.8)/2, 425, myFont, BUT_HBC, 14, HEX_WHITE);
	
	GRRLIB_DrawImg(464, 388, tex_WiiButtonPlus_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(480-(strlen(BUT_SysMenu)*7.8)/2, 425, myFont, BUT_SysMenu, 14, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	
	return 0;
}

/*inline void DrawCog(void) {
	if (CheckTime(Last_Cog_Turn, 25)) {
		Cog_Num++;
		if (Cog_Num > 4) Cog_Num = 0;
		Last_Cog_Turn = gettick();
	}
	GRRLIB_DrawImg(220, 150, tex_Cogs_png[Cog_Num], 0, 1, 1, HEX_WHITE);
	return;
}*/

int printEndError(const char* msg) {
	int i;
	PauseThread();
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Deleteicon_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	GRRLIB_DrawImg(302, 292, tex_WiiButtonA_png, 0, 1, 1, HEX_WHITE);

	GRRLIB_DrawImg(144, 388, tex_WiiButtonMinus_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(160-(strlen(BUT_Shutoff)*7.8)/2, 425, myFont, BUT_Shutoff, 14, HEX_WHITE);
	
	GRRLIB_DrawImg(302, 386, tex_WiiButtonHome_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(320-(strlen(BUT_HBC)*7.8)/2, 425, myFont, BUT_HBC, 14, HEX_WHITE);
	
	GRRLIB_DrawImg(464, 388, tex_WiiButtonPlus_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(480-(strlen(BUT_SysMenu)*7.8)/2, 425, myFont, BUT_SysMenu, 14, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	
	return 0;
}

int printReport(char report[200][100], int firstLine, bool completeReport) {
	int i, j;
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
		GRRLIB_DrawImg(36, 40, tex_window_png, 0, 1, 1, HEX_WHITE);
		
		for (j = 0; j < BLANK + 1; j++) {
			GRRLIB_PrintfTTF(80, 98+j*17, myFont, report[firstLine+j], 12, HEX_BLACK);
		}
		if (completeReport) {
			GRRLIB_DrawImg(80, 388, tex_WiiButtonMinus_png, 0, 1, 1, HEX_WHITE);
			GRRLIB_PrintfTTF(96-(strlen(BUT_Shutoff)*7.8)/2, 425, myFont, BUT_Shutoff, 14, HEX_WHITE);
		
			GRRLIB_DrawImg(238, 386, tex_WiiButtonHome_png, 0, 1, 1, HEX_WHITE);
			GRRLIB_PrintfTTF(256-(strlen(BUT_HBC)*7.8)/2, 425, myFont, BUT_HBC, 14, HEX_WHITE);
			
			GRRLIB_DrawImg(380, 388, tex_WiiButtonA_png, 0, 1, 1, HEX_WHITE);
			GRRLIB_PrintfTTF(408-(strlen(BUT_ConfirmUpload)*7.8)/2, 425, myFont, BUT_ConfirmUpload, 14, HEX_WHITE);
		
			GRRLIB_DrawImg(528, 388, tex_WiiButtonPlus_png, 0, 1, 1, HEX_WHITE);
			GRRLIB_PrintfTTF(544-(strlen(BUT_SysMenu)*7.8)/2, 425, myFont, BUT_SysMenu, 14, HEX_WHITE);
		} else {
			GRRLIB_DrawImg(144, 388, tex_WiiButtonMinus_png, 0, 1, 1, HEX_WHITE);
			GRRLIB_PrintfTTF(160-(strlen(BUT_Shutoff)*7.8)/2, 425, myFont, BUT_Shutoff, 14, HEX_WHITE);
		
			GRRLIB_DrawImg(302, 386, tex_WiiButtonHome_png, 0, 1, 1, HEX_WHITE);
			GRRLIB_PrintfTTF(320-(strlen(BUT_HBC)*7.8)/2, 425, myFont, BUT_HBC, 14, HEX_WHITE);
		
			GRRLIB_DrawImg(464, 388, tex_WiiButtonPlus_png, 0, 1, 1, HEX_WHITE);
			GRRLIB_PrintfTTF(480-(strlen(BUT_SysMenu)*7.8)/2, 425, myFont, BUT_SysMenu, 14, HEX_WHITE);
		}
		
		GRRLIB_Render();
	}
	
	return 0;
}

int printUploadSuccess(const char* msg) {
	int i;
	PauseThread();
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Checkicon_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	
	GRRLIB_DrawImg(302, 300, tex_WiiButtonA_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(319-(strlen(BUT_OK)*7.8)/2, 342, myFont, BUT_OK, 14, HEX_WHITE);
	
	//GRRLIB_DrawImg(52, 340, tex_loadingbarblue_png, 0, 1, 1, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}

int printUploadError(const char* msg) {
	int i;
	PauseThread();
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Deleteicon_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_PrintfTTF((620-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	
	GRRLIB_DrawImg(302, 300, tex_WiiButtonA_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(319-(strlen(BUT_OK)*7.8)/2, 342, myFont, BUT_OK, 14, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}

