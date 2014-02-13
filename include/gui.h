#ifndef __GUI_H__
#define __GUI_H__

#include <grrlib.h>

#define HEX_WHITE	0xFFFFFFFF
#define HEX_BLACK	0x00000000

#define CopyBuf() 	GRRLIB_Screen2Texture(0, 0, tex_ScreenBuf, GX_FALSE)
#define DrawBuf()	GRRLIB_DrawImg(0, 0, tex_ScreenBuf, 0, 1, 1, HEX_WHITE)

extern GRRLIB_ttfFont *myFont;
extern GRRLIB_texImg *tex_background_png;
extern GRRLIB_texImg *tex_Checkicon_png;
extern GRRLIB_texImg *tex_Deleteicon_png;
extern GRRLIB_texImg *tex_Refreshicon_png;
extern GRRLIB_texImg *tex_WiiButtonA_png;
extern GRRLIB_texImg *tex_WiiButtonHome_png;
extern GRRLIB_texImg *tex_WiiButtonMinus_png;
extern GRRLIB_texImg *tex_WiiButtonPlus_png;
extern GRRLIB_texImg *tex_WiiDpadLeft_png;
extern GRRLIB_texImg *tex_WiiDpadRight_png;
extern GRRLIB_texImg *tex_loadingbargrey_png;
extern GRRLIB_texImg *tex_loadingbarblue_png;
extern GRRLIB_texImg *tex_window_png;
extern GRRLIB_texImg *tex_ScreenBuf;

int initGUI(void);
void deinitGUI(void);
int printError(const char* msg);
int printSuccess(const char* msg);
int printLoading(const char* msg);
int printSelectIOS(const char* msg, const char* ios);
int printLoadingBar(const char* msg, const f32 percent);
int printEndSuccess(const char* msg);
int printEndError(const char* msg);
int printReport(char report[200][100], int firstLine, bool completeReport);
int printUploadSuccess(const char* msg);
int printUploadError(const char* msg);

#endif