#include <stdlib.h>
#include <wiiuse/wpad.h>

#include "tahoma_ttf.h"

#include "background_png.h"
#include "Checkicon_png.h"
#include "Deleteicon_png.h"
#include "Refreshicon_png.h"
#include "WiiButtonA_png.h"
#include "WiiButtonHome_png.h"
#include "WiiButtonMinus_png.h"
#include "WiiButtonPlus_png.h"
#include "WiiDpadLeft_png.h"
#include "WiiDpadRight_png.h"
#include "loadingbargrey_png.h"
#include "loadingbarblue_png.h"
#include "window_png.h"
#include "languages.h"
#include "gui.h"
#include "gecko.h"
#include "sys.h"
#include "fatMounter.h"

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
GRRLIB_texImg *tex_ScreenBuf;


int initGUI(void) {
	// Initialise the Graphics & Video subsystem
    GRRLIB_Init();

	//Load Images
	myFont = GRRLIB_LoadTTF(tahoma_ttf, tahoma_ttf_size);
	tex_background_png = GRRLIB_LoadTexturePNG(background_png);
	tex_Checkicon_png = GRRLIB_LoadTexturePNG(Checkicon_png);
	tex_Deleteicon_png = GRRLIB_LoadTexturePNG(Deleteicon_png);
	tex_Refreshicon_png = GRRLIB_LoadTexturePNG(Refreshicon_png);
	tex_WiiButtonA_png = GRRLIB_LoadTexturePNG(WiiButtonA_png);
	tex_WiiButtonHome_png = GRRLIB_LoadTexturePNG(WiiButtonHome_png);
	tex_WiiButtonMinus_png = GRRLIB_LoadTexturePNG(WiiButtonMinus_png);
	tex_WiiButtonPlus_png = GRRLIB_LoadTexturePNG(WiiButtonPlus_png);
	tex_WiiDpadLeft_png = GRRLIB_LoadTexturePNG(WiiDpadLeft_png);
	tex_WiiDpadRight_png = GRRLIB_LoadTexturePNG(WiiDpadRight_png);
	tex_loadingbargrey_png = GRRLIB_LoadTexturePNG(loadingbargrey_png);
	tex_loadingbarblue_png = GRRLIB_LoadTexturePNG(loadingbarblue_png);
	tex_window_png = GRRLIB_LoadTexturePNG(window_png);
	tex_ScreenBuf = GRRLIB_CreateEmptyTexture(rmode->fbWidth, rmode->efbHeight);
	return 0;
}

void deinitGUI(void) {
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
	GRRLIB_FreeTexture(tex_ScreenBuf);
	GRRLIB_Exit();
	return;
}

int printError(const char* msg) {
	int i;
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
	int i;
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Refreshicon_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}

int printSelectIOS(const char* msg, const char* ios) {
	int i;

	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Refreshicon_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	GRRLIB_PrintfTTF((640-strlen(ios)*9)/2, 300, myFont, ios, 20, HEX_WHITE);
	GRRLIB_DrawImg(222, 286, tex_WiiDpadLeft_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(370, 286, tex_WiiDpadRight_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_DrawImg(310, 388, tex_WiiButtonPlus_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(335-(strlen(BUT_Update)*7.8)/2, 425, myFont, BUT_Update, 14, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}

int printLoadingBar(const char* msg, const f32 percent) {
	int loaded, notloaded, i;
	
	loaded = 536.0/100.0*percent;
	if (loaded > 536) loaded = 536;
	
	notloaded = 536 - loaded;
	
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Refreshicon_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	GRRLIB_DrawPart(52, 340, 0, 0, loaded, 36, tex_loadingbarblue_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawPart(52+loaded, 340, loaded, 0, notloaded, 36, tex_loadingbargrey_png, 0, 1, 1, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}
	
int printEndSuccess(const char* msg) {
	int i;
	
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

int printEndError(const char* msg) {
	int i;

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
	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(36, 40, tex_window_png, 0, 1, 1, HEX_WHITE);
	
	for (j = 0; j < 15; j++) {
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
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}

int printUploadSuccess(const char* msg) {
	int i;

	GRRLIB_DrawImg(0, 0, tex_background_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_DrawImg(256, 112, tex_Checkicon_png, 0, 1, 1, HEX_WHITE);
	
	GRRLIB_PrintfTTF((640-strlen(msg)*9)/2, 256, myFont, msg, 20, HEX_WHITE);
	
	GRRLIB_DrawImg(302, 300, tex_WiiButtonA_png, 0, 1, 1, HEX_WHITE);
	GRRLIB_PrintfTTF(319-(strlen(BUT_OK)*7.8)/2, 342, myFont, BUT_OK, 14, HEX_WHITE);
	
	GRRLIB_DrawImg(52, 340, tex_loadingbarblue_png, 0, 1, 1, HEX_WHITE);
	CopyBuf();
	for (i = 0; i < 3; i++) { //Workaround for GRRLIB_Render() bug
		DrawBuf();
		GRRLIB_Render();
	}
	return 0;
}

int printUploadError(const char* msg) {
	int i;

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

