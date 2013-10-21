/*-------------------------------------------------------------
 
 detect_settings.h -- detects various system settings
 
 Copyright (C) 2008 tona
 Unless other credit specified
 
 This software is provided 'as-is', without any express or implied
 warranty.  In no event will the authors be held liable for any
 damages arising from the use of this software.
 
 Permission is granted to anyone to use this software for any
 purpose, including commercial applications, and to alter it and
 redistribute it freely, subject to the following restrictions:
 
 1.The origin of this software must not be misrepresented; you
 must not claim that you wrote the original software. If you use
 this software in a product, an acknowledgment in the product
 documentation would be appreciated but is not required.
 
 2.Altered source versions must be plainly marked as such, and
 must not be misrepresented as being the original software.
 
 3.This notice may not be removed or altered from any source
 distribution.
 
 -------------------------------------------------------------*/

#ifndef __SYSMENUINFO_H__
#define __SYSMENUINFO_H__

#define SADR_LENGTH 0x1007+1
#define round_up(x,n) (-(-(x) & -(n)))

typedef struct {
    u32 deviceId;
    u32 hollywoodVersion;
    u32 boot2version;
    u16 sysMenuVer;
    s32 sysMenuIOS;
    bool sysMenuIOSisStub;
    bool regionChangedKoreanWii;
    double sysMenuNinVersion;
    char sysMenuRegion;
    char regionFromSerial;
    s32 lang;
    s32 area;
    s32 game;
    s32 video;
    s32 eula;
    s32 country;
    bool reRunWithSU;
    bool missingIOSwarning;
    bool SMRegionMismatchWarning;
    bool failSMContentRead;
    u16 bcVersion;
    u16 miosVersion;
    u32 titleCnt;
    u32 iosCount;
    bool ahbprot;
} SYSSETTINGS;
extern SYSSETTINGS wiiSettings;

typedef struct {
    char name[ISFS_MAXPATH + 1];
    int type;
} dirent_t;

char getSystemMenuRegionFromContent();

#endif
