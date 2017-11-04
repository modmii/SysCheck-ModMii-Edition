/*-------------------------------------------------------------
 
 detect_settings.h -- detects various system settings
 
 Copyright (C) 2008 tona
 Copyright (C) 2014 JoostinOnline
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
    u32 deviceID;
	u8 deviceType;
    u32 boot2version;
    u32 sysMenuVer;
    u32 dvdSupport;
    s32 sysMenuIOS;
    s32 sysMenuIOSVersion;
    s32 sysMenuIOSType;
    float sysNinVersion;
    char sysMenuRegion;
	s32 systemRegion;
	bool validregion;
    char country[44];
	char miosInfo[128];
    u32 countTitles;
    u32 countBCMIOS;
    u32 countIOS;
    u32 countStubs;
	u8 shopcode;
	u8 priiloader;
	bool nandAccess;
	u32 runningIOS;
	bool runningIOSType;
	u32 runningIOSRevision;
} SysSettings_t;

typedef struct {
    char name[ISFS_MAXPATH + 1];
    int type;
} dirent_t;

typedef struct {
	u32 hbcversion;
	u32 hbfversion;
	s32 hbc;
	s32 hbf;
	u32 hbcIOS;
} homebrew_t;

#endif
