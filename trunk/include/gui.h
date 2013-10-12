#ifndef __GUI_H__
#define __GUI_H__

#define HEX_WHITE	0xFFFFFFFF
#define HEX_BLACK	0x00000000

int initGUI(void);
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