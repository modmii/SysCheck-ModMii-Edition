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
#include <ctype.h>

#include "fatMounter.h"
#include "gecko.h"
#include "gui.h"
#include "http.h"
#include "languages.h"
#include "mload.h"
#include "sha1.h"
#include "sys.h"
#include "SysMenuInfo.h"
#include "title.h"
#include "tmdIdentification.h"
#include "wiibasics.h"


#include "ticket_dat.h"
#include "tmd_dat.h"

#define DM_INSTALLED 	((*(vu32*)(appfile+i)) == 0x44494F53 && (*(vu32*)(appfile+i+5)) == 0x4D494F53) // true = DM or DML installed
#define DML_OR_DM		(*(vu32*)(appfile+i+10) == 0x4C697465) // true = DML
#define CMP_TIME(X) 	(difftime(unixTime, (X)) >= 0)

typedef struct {
	const u32 titleID;
	const u32 hashes[5];
} vIOSdb_t;

u8 sysMenuInfoContent = 0;
const char *Regions[] = {"NTSC-J", "NTSC-U", "PAL", "", "KOR", "NTSC-J"}; //Last is actually China

// Distinct vIOS versions
const vIOSdb_t vIOSdb[] = {
	{9,  {0x5febd050, 0xe51b4cd7, 0xf3accee4, 0xb77a6674, 0x5a944e92}},	// v1290
	{12, {0x1bf4f9fc, 0x04a7c04d, 0x14371ebf, 0xa22f8906, 0xcd687b37}},	// v782
	{13, {0xb9f05ccb, 0x5b0f6f50, 0xf1f67815, 0x54505f42, 0xfe7346ae}},	// v1288
	{14, {0x578a2b74, 0x4de6d6b3, 0xb7d1847c, 0xf1e96b8c, 0x9a439929}},	// v1288
	{15, {0xbfe87fc4, 0x556b6920, 0xfb1ab7be, 0x9920a1b9, 0x7d3f3368}},	// v1288
	{17, {0x516a92f8, 0x425a502e, 0x4516cd0c, 0xe1a4b188, 0xca911640}},	// v1288
	{21, {0x5f74e599, 0xf6ff78b3, 0x03db1d9e, 0x41de0e44, 0x700a0bbe}},	// v1295
	{22, {0xb637fd6e, 0x6389d456, 0x3eea7fb8, 0x3a00e89e, 0xbf91b5c2}},	// v1550
	{28, {0x1844bba1, 0x701eecfa, 0x59f649b2, 0x922f178f, 0x8f3776c2}},	// v2063
	{31, {0x3f08b3bb, 0xd40eca36, 0xdd7ca54f, 0x18fa4cb3, 0xf3af6b3d}},	// v3864
	{33, {0xb2b36ad9, 0x70b96c52, 0x1150a307, 0xd70b864e, 0x871108ee}},	// v3864
	{34, {0xcf9ec31e, 0x5f1dc978, 0x022263ef, 0x731c19f3, 0xff441707}},	// v3864
	{35, {0xf6019cd4, 0x4f435eca, 0x0a7bc86e, 0xaea6d727, 0x421dc71f}},	// v3864
	{36, {0xfcc68cf6, 0x96463d48, 0x4b62a3d6, 0x8210bac0, 0x5627b6dd}},	// v3864
	{37, {0x6681b0ce, 0xd63a6971, 0x7fda96e4, 0xacd08365, 0xf76a206f}},	// v5919
	{38, {0x486e21de, 0x4016a4d7, 0x9d7e2bb5, 0xa3b8ac91, 0x73915a00}},	// v4380
	{41, {0x5d524f58, 0x21df26e7, 0x557b6fe7, 0x602568d9, 0xb1798e46}},	// v3863
	{43, {0x54f59a45, 0x91f87a50, 0x8051db3f, 0x1ecf517c, 0x859c38a2}},	// v3863
	{45, {0x3a608c3e, 0xc35ae2b1, 0x70f57be4, 0x0cf64a2b, 0xd1cf986d}},	// v3863
	{46, {0xcab6c43f, 0x4d4cdf49, 0x1786ee08, 0x2663b16a, 0xd93f8e0a}},	// v3863
	{48, {0xc3a1ffbc, 0x6f2b2649, 0x0a767330, 0x959ff1df, 0xba8d01b2}},	// v4380
	{53, {0x89bf1a35, 0x2bd8e143, 0xa6e1fd47, 0x596902f3, 0xae24864d}},	// v5919
	{55, {0x7b8e33ae, 0xade979ca, 0xbfb2bbd6, 0x20ca08a7, 0xeac82eff}},	// v5919
	{56, {0x1ad56238, 0x10fd144b, 0x94fd24a9, 0x655b1665, 0xa2d1cb5b}},	// v5918
	{57, {0x9bcd04d5, 0x4db165be, 0xf07a0935, 0x9a5eea68, 0xc56fe221}},	// v6175
	{58, {0xedce2922, 0x86d540cf, 0x1eb00fdc, 0x09687aae, 0xcaba4f60}},	// v6432
	{59, {0x20f8f664, 0x8a1c5642, 0x3a3177fa, 0xa85631a4, 0x095bae0f}},	// v9249
	{62, {0xc630ecc3, 0x245e8cd1, 0xe2cd25a9, 0x2f5528b4, 0x3a9c0f01}},	// v6942
	{80, {0x176c6167, 0x32213287, 0x8795750a, 0x84ad4e1b, 0x70867b3c}}	// v7200
};

int get_title_ios(u64 title) {
	s32 ret, fd;
	static char filepath[256] ATTRIBUTE_ALIGN(32);

	// Check to see if title exists
	if (ES_GetDataDir(title, filepath) >= 0 ) {
		u32 tmd_size;
		static u8 tmd_buf[MAX_SIGNED_TMD_SIZE] ATTRIBUTE_ALIGN(32);

		ret = ES_GetStoredTMDSize(title, &tmd_size);
		if (ret < 0) {
			// If we fail to use the ES function, try reading manually
			// This is a workaround added since some IOS (like 21) don't like our
			// call to ES_GetStoredTMDSize

			sprintf(filepath, "/title/%08x/%08x/content/title.tmd", TITLE_UPPER(title), TITLE_LOWER(title));

			ret = ISFS_Open(filepath, ISFS_OPEN_READ);
			if (ret <= 0) return 0;

			fd = ret;

			ret = ISFS_Seek(fd, 0x184, 0);

			ret = ISFS_Read(fd,tmd_buf,8);
			if (ret < 0) return 0;

			ret = ISFS_Close(fd);
			if (ret < 0) return 0;

			return be64(tmd_buf);

		} else {
			// Normal versions of IOS won't have a problem, so we do things the "right" way.

			// Some of this code adapted from bushing's title_lister.c
			signed_blob *s_tmd = (signed_blob *)tmd_buf;
			ret = ES_GetStoredTMD(title, s_tmd, tmd_size);
			if (ret < 0) return -1;

			tmd *t = SIGNATURE_PAYLOAD(s_tmd);
			return t->sys_version;
		}

	}
	return 0;
}

bool getInfoFromContent(IOS_t *ios) {
	bool retValue = false;
	iosinfo_t *iosinfo = NULL;
	char filepath[ISFS_MAXPATH] ATTRIBUTE_ALIGN(0x20);
	u8 *buffer = NULL;
	u32 filesize = 0;
	s32 ret = 0;

	// Try to identify the cIOS by the info put in by the installer/ModMii
	sprintf(filepath, "/title/%08x/%08x/content/%08x.app", 0x00000001, ios->titleID, ios->infoContent);
	ret = read_file_from_nand(filepath, &buffer, &filesize);

	iosinfo = (iosinfo_t *)(buffer);
	if (ret >= 0 && ios->titleID == TID_CBOOT2 && ios->num_contents == 1) {
		int i;
		for (i = 0; i < filesize - sizeof("bootcb2")-1; i++)
		{
			if (!strncmp((char*)buffer + i, "bootcb2", sizeof("bootcb2")-1))
			{
				sprintf(ios->info, " cBoot252");
				gprintf("is cBoot252\n");
				gprintf("is cBoot252\r\n");
				retValue = true;
				ios->isStub = true;
				break;
			}
		}
		if (buffer) free(buffer);
	} else if (ret >= 0 && iosinfo != NULL && iosinfo->magicword == 0x1ee7c105 && iosinfo->magicversion == 1) {
		if (ios->titleID != iosinfo->baseios)
			ios->baseIOS = iosinfo->baseios;

		if (strcmp(iosinfo->name, "nintendo") == 0)
			snprintf(ios->info, MAX_ELEMENTS(ios->info), "rev %u", iosinfo->version);
		else
			snprintf(ios->info, MAX_ELEMENTS(ios->info), "%s-v%u%s", iosinfo->name, iosinfo->version, iosinfo->versionstring);
		gprintf("is %s\n", ios->info);
		gprintf("is %s\r\n", ios->info);
		retValue = true;
		if (buffer != NULL) free(buffer);
	}

	return retValue;
}

s32 read_isfs(char *path, u8 **out, u32 *size)
{
	s32 ret, fd;
	static fstats status ATTRIBUTE_ALIGN(32);
	
	fd = ISFS_Open(path, ISFS_OPEN_READ);
	if (fd < 0)
	{
		gprintf("ISFS_Open for '%s' returned %d.\r\n", path, fd);
		return -1;
	}
	
	ret = ISFS_GetFileStats(fd, &status);
	if (ret < 0)
	{
		gprintf("ISFS_GetFileStats(fd) returned %d.\r\n", ret);
		ISFS_Close(fd);
		return -1;
	}
	
	if (status.file_length == 0)
	{
		ISFS_Close(fd);
		return -1;
	}
	
	*size = status.file_length;
	gprintf("Size = %u bytes.\r\n", *size);
	
	*out = allocate_memory(*size);
	if (*out == NULL) 
	{ 
		gprintf("\r\nError allocating memory for out.\r\n");
		ISFS_Close(fd);
		return -2;
	}
	
	u32 blksize, writeindex = 0, restsize = *size;
	
	while (restsize > 0)
	{
		if (restsize >= 0x4000)
		{
			blksize = 0x4000;
		} else {
			blksize = restsize;
		}
		
		ret = ISFS_Read(fd, *out + writeindex, blksize);
		if (ret < 0) 
		{
			gprintf("\r\nISFS_Read(%d, %d) returned %d.\r\n", fd, blksize, ret);
			free(*out);
			ISFS_Close(fd);
			return -1;
		}
		
		writeindex += blksize;
		restsize -= blksize;
	}
	
	ISFS_Close(fd);
	return 0;
}

s32 brute_tmd(tmd *p_tmd)
{
	u16 fill;
	for(fill=0; fill<65535; fill++)
	{
		p_tmd->fill3 = fill;
		sha1 hash;
		SHA1((u8 *)p_tmd, TMD_SIZE(p_tmd), hash);;
		if (hash[0]==0)
		{
			return 0;
		}
	}
	return -1;
}

// Certificates
static u8 certs[0xA00] ATTRIBUTE_ALIGN(32);

bool GetCertificates(void)
{
	ISFS_Initialize();
	int fd = ISFS_Open("/sys/cert.sys", ISFS_OPEN_READ);

	if (fd < 0) return false;
	if (ISFS_Read(fd, certs, sizeof(certs)) != sizeof(certs)) return false;

	ISFS_Close(fd);
	return true;
}

s32 GetTMD(u64 TicketID, signed_blob **Output, u32 *Length)
{
	signed_blob* TMD = NULL;

	u32 TMD_Length;
	s32 ret;

	/* Retrieve TMD length */
	ret = ES_GetStoredTMDSize(TicketID, &TMD_Length);
	if (ret < 0)
		return ret;

	/* Allocate memory */
	TMD = (signed_blob*)memalign(32, (TMD_Length+31)&(~31));
	if (!TMD)
		return IPC_ENOMEM;

	/* Retrieve TMD */
	ret = ES_GetStoredTMD(TicketID, TMD, TMD_Length);
	if (ret < 0)
	{
		free(TMD);
		return ret;
	}

	/* Set values */
	*Output = TMD;
	*Length = TMD_Length;

	return 0;
}

s32 read_file_from_nand(char *filepath, u8 **buffer, u32 *filesize)
{
	s32 Fd;
	int ret;

	if (buffer == NULL)
	{
		gprintf("NULL Pointer\n");
		return -1;
	}

	Fd = ISFS_Open(filepath, ISFS_OPEN_READ);
	if (Fd < 0)
	{
		gprintf("ISFS_Open %s failed %d\n", filepath, Fd);
		return Fd;
	}

	fstats *status;
	status = allocate_memory(sizeof(fstats));
	if (status == NULL)
	{
		gprintf("Out of memory for status\n");
		return -1;
	}

	ret = ISFS_GetFileStats(Fd, status);
	if (ret < 0)
	{
		gprintf("ISFS_GetFileStats failed %d\n", ret);
		ISFS_Close(Fd);
		free(status);
		return -1;
	}

	*buffer = allocate_memory(status->file_length);
	if (*buffer == NULL)
	{
		gprintf("Out of memory for buffer\n");
		ISFS_Close(Fd);
		free(status);
		return -1;
	}

	ret = ISFS_Read(Fd, *buffer, status->file_length);
	if (ret < 0)
	{
		gprintf("ISFS_Read failed %d\n", ret);
		ISFS_Close(Fd);
		free(status);
		free(*buffer);
		return ret;
	}

	ISFS_Close(Fd);

	*filesize = status->file_length;
	free(status);

	if (*filesize > 0)
	{
		DCFlushRange(*buffer, *filesize);
		ICInvalidateRange(*buffer, *filesize);
	}

	return 0;
}

inline void zero_sig(signed_blob *sig)
{
	u8 *sig_ptr = (u8 *)sig;
	memset(sig_ptr + 4, 0, SIGNATURE_SIZE(sig)-4);
}


// Get the boot2 version
inline u32 GetBoot2Version(void)
{
	u32 boot2version = 0;
	if (ES_GetBoot2Version(&boot2version) < 0) boot2version = 0;
	return boot2version;
}


// Get the console ID
inline u32 GetDeviceID(void)
{
	u32 deviceId = 0;
	if (ES_GetDeviceID(&deviceId) < 0) deviceId = 0;
	return deviceId;
}


// Remove bogus ticket
int RemoveBogusTicket(void)
{
	tikview *viewdata = NULL;
	u64 titleId = 0x100000000LL;
	u32 cnt, views;
	s32 ret;

	// Get number of ticket views
	ret = ES_GetNumTicketViews(titleId, &views);

	if (ret < 0) return ret;

	if (!views) return 1;
	else if (views > 16) return -1;

	// Get ticket views
	viewdata = (tikview*)memalign(32, sizeof(tikview) * views);
	ret = ES_GetTicketViews(titleId, viewdata, views);

	if (ret < 0) return ret;

	// Remove tickets
	for (cnt = 0; cnt < views; cnt++)
	{
		ret = ES_DeleteTicket(&viewdata[cnt]);

		if (ret < 0) return ret;
	}

	return ret;
}

// Remove bogus ticket
inline s32 RemoveBogusTMD(void)
{
	return ES_DeleteTitle(0x100000000LL);
}

inline bool CheckBeerTicket(u32 titleID) {
	char filepath[ISFS_MAXPATH] ATTRIBUTE_ALIGN(0x20);
	u8 *buffer = NULL;
	u32 tik_size = 0;
	s32 ret = 0;

	sprintf(filepath, "/ticket/00000001/%08x.tik", titleID);
	if (read_isfs(filepath, &buffer, &tik_size)) {
		gprintf("Failed to read IOS%u ticket\n", titleID);
		return false;
	}
	
	if (tik_size != 676) return false;
	ret = !strcmp((char*)buffer + 0x01BF, "GottaGetSomeBeer");
	free(buffer);
	return ret;
}

inline bool CheckIOSType(u32 titleID, s32 revision) {
	u32 start_address = IOS_TOP;
	const char WL_String[] = {0x57, 0x4C, 0x3A, 0x20, 0x30, 0x32, 0x2F, 0x30, 0x32, 0x2F, 0x31, 0x32}; // "WL: 02/02/12"
	u32 i;
	// Should work on all vIOS, but requires AHB access
	for(i = start_address; i < 0x94000000 - sizeof(WL_String); i++) {
		if (memcmp((char*)i, WL_String, sizeof(WL_String)) == 0) return IOS_WII_U;
	}
	return IOS_WII;
}

// Check fake signatures (aka Trucha Bug)
inline bool CheckFakeSignature(void)
{
	s32 ret = ES_AddTicket((signed_blob *)ticket_dat, ticket_dat_size, (signed_blob *)certs, sizeof(certs), 0, 0);

	if (ret >= 0) RemoveBogusTicket();
	return (ret >= 0 || ret == -1028);
}


// Check if you can still call ES_DiVerify (aka ES_Identify) to make IOS think that you actually are a different title
inline bool CheckESIdentify(void)
{
	int ret = ES_Identify((signed_blob *)certs, sizeof(certs), (signed_blob *)tmd_dat, tmd_dat_size, (signed_blob *)ticket_dat, ticket_dat_size, NULL);
	return ((ret >= 0) || (ret == -2011));
}


// Check flash access
inline bool CheckFlashAccess(void)
{
	s32 ret = IOS_Open("/dev/flash", 1);
	if (ret >= 0) IOS_Close(ret);
	return (ret >= 0);
}

inline bool CheckMload(void)
{
	int ret = IOS_Open("/dev/mload", 0);
	if (ret >= 0) IOS_Close(ret);
	return (ret >= 0);
}


// Check NAND access
inline bool CheckNANDAccess(void)
{
	int ret = IOS_Open("/ticket/00000001/00000002.tik", 1);
	if (ret >= 0) IOS_Close(ret);
	return (ret >= 0);
}


// Check boot2 access
inline bool CheckBoot2Access(void)
{
	int ret = IOS_Open("/dev/boot2", 1);
	if (ret >= 0) IOS_Close(ret);
	return (ret >= 0);
}


// Check USB 2.0 module
bool CheckUSB2(u32 titleID)
{
	// Hermes' IOS supports USB 2.0 module
	switch (titleID)
	{
		case 58:
		case 202:
		case 222:
		case 223:
		case 224:
			return true;
			break;
	}

	// Open USB 2.0 module
	gprintf("IOS_Open(\"/dev/usb2\", 1) \n");
	int ret = IOS_Open("/dev/usb2", 1);

	// If fail, try old USB 2.0 module
	gprintf("IOS_Open(\"/dev/usb/ehc\", 1) \n");
	if (ret < 0) ret = IOS_Open("/dev/usb/ehc", 1);

	if (ret >= 0) IOS_Close(ret);
	return (ret >= 0);
}


// Check if this is an IOS stub (according to WiiBrew.org)
bool IsKnownStub(u32 noIOS, s32 noRevision)
{
	if (noIOS == 254) {
		if (noRevision == 65280 ||
			noRevision ==   260 ||
			noRevision ==   3   ||
			noRevision ==   2 ) return true;
	}
	// BootMii As IOS is installed on IOS254 rev 31338
	if (noIOS == 254 && (noRevision == 31338 || noRevision == 65281)) return true;
	if (noIOS == 253 && noRevision == 65535) return true;


	/*if (noIOS ==   3 && noRevision == 65280) return true;
	if (noIOS ==   4 && noRevision == 65280) return true;
	if (noIOS ==  10 && noRevision ==   768) return true;
	if (noIOS ==  11 && noRevision ==   256) return true;
	if (noIOS ==  16 && noRevision ==   512) return true;
	if (noIOS ==  20 && noRevision ==   256) return true;
	if (noIOS ==  30 && noRevision ==  2816) return true;
	if (noIOS ==  40 && noRevision ==  3072) return true;
	if (noIOS ==  50 && noRevision ==  5120) return true;
	if (noIOS ==  51 && noRevision ==  4864) return true;
	if (noIOS ==  52 && noRevision ==  5888) return true;
	if (noIOS ==  60 && noRevision ==  6400) return true;
	if (noIOS ==  70 && noRevision ==  6912) return true;
	if (noIOS == 222 && noRevision == 65280) return true;
	if (noIOS == 223 && noRevision == 65280) return true;
	if (noIOS == 249 && noRevision == 65280) return true;
	if (noIOS == 250 && noRevision == 65280) return true;*/


	return false;
}

int checkSysLoader(void) {
	char filepath[ISFS_MAXPATH] ATTRIBUTE_ALIGN(0x20);
	static u64 titleId ATTRIBUTE_ALIGN(32) = 0x0000000100000002LL;
	int ret = 0;
	tmd *ptmd = NULL;
	u32 TMD_size = 0;
	signed_blob *stmd = NULL;
	u32 i = 0;
	u32 filesize = 0;
	u8 *buffer = NULL;
	const char	*checkStr = "priiloader";
	const char	*checkStr2 = "prefiix";
	int retValue = -1;

	ret = GetTMD(titleId, &stmd, &TMD_size);

	if (ret < 0)
		goto end;

	if (!stmd)
	{
		ret = -1;

		goto end;
	}

	ptmd = (tmd*)SIGNATURE_PAYLOAD(stmd);

	for (i = 0; i < ptmd->num_contents; i++)
	{
		if (ptmd->contents[i].index == ptmd->boot_index)
		{
			sprintf(filepath, "/title/%08x/%08x/content/%08x.app" , 0x00000001, 0x00000002, ptmd->contents[i].cid);
			ret = read_file_from_nand(filepath, &buffer, &filesize);
			if (ret < 0 || filesize < 0) {
				retValue = -2;
				goto end;
			}
			break;
		}
	}

	for (i = 0; i < filesize - strlen(checkStr); i++)
	{
		if (!strncmp((char*)buffer + i, checkStr, strlen(checkStr)))
		{
			retValue = 1;

			break;
		} else 	if (!strncmp((char*)buffer + i, checkStr2, strlen(checkStr2)))
		{
			retValue = 2;

			break;
		}
	}

end:
	if (buffer != NULL) free(buffer);

	if (stmd != NULL) free(stmd);
	ptmd = NULL;

	return retValue;

}



s32 get_miosinfo(char *str)
{
	u32 size = 0;
	u32 i = 0;
	s32 ret = 0;
	u8 *appfile = NULL;
	char buffer[20];

	ret = read_file_from_nand("/title/00000001/00000101/content/0000000c.app", &appfile, &size);
	if(ret >= 0 && appfile)
	{
		struct tm time;

		// Timestamp of DML r52
		strptime("Mar 7 2012 19:36:06", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_r52_time = mktime(&time);

		// Timestamp of DML 1.2
		strptime("Apr 24 2012 19:44:08", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_1_2_time = mktime(&time);
		
		// I can't find one for 1.4 or 1.3, so I'm just doing 1 second over 1.2
		strptime("Apr 24 2012 19:44:09", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_1_4_time = mktime(&time);

		// Timestamp of DML 1.4b
		strptime("May  7 2012 21:12:47", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_1_4b_time = mktime(&time);

		// Timestamp of DML 1.5
		strptime("Jun 14 2012 00:05:09", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_1_5_time = mktime(&time);

		// Timestamp of DM 2.0
		strptime("Jun 23 2012 19:43:21", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_0_time = mktime(&time);

		// Timestamp of DM 2.1
		strptime("Jul 17 2012 11:25:35", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_1_time = mktime(&time);

		// Timestamp of DM 2.2 initial release
		strptime("Jul 18 2012 16:57:47", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_2_time = mktime(&time);

		// Timestamp of DM 2.2 update2
		strptime("Jul 20 2012 14:49:47", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_2_2_time = mktime(&time);

		// Timestamp of DML 2.2 initial release
		strptime("Aug  6 2012 15:19:17", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_2_time = mktime(&time);

		// Timestamp of DML 2.2 update1
		strptime("Aug 13 2012 00:12:46", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_2_1_time = mktime(&time);

		// Timestamp of DML 2.3 mirror link
		strptime("Sep 24 2012 13:13:42", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_3m_time = mktime(&time);

		// Timestamp of DM 2.3
		strptime("Sep 24 2012 15:51:54", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_3_time = mktime(&time);

		// Timestamp of DML 2.3 main link
		strptime("Sep 25 2012 03:03:41", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_3_time = mktime(&time);

		// Timestamp of DM 2.4
		strptime("Oct 21 2012 22:57:12", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_4_time = mktime(&time);

		// Timestamp of DML 2.4
		strptime("Oct 21 2012 22:57:17", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_4_time = mktime(&time);

		// Timestamp of DM 2.5
		strptime("Nov  9 2012 21:18:52", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_5_time = mktime(&time);

		// Timestamp of DML 2.5
		strptime("Nov  9 2012 21:18:56", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_5_time = mktime(&time);

		// Timestamp of DM 2.6.0
		strptime("Dec  1 2012 01:52:53", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_6_0_time = mktime(&time);

		// Timestamp of DML 2.6
		strptime("Dec  1 2012 16:22:29", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_6_time = mktime(&time);

		// Timestamp of DM 2.6.1
		strptime("Dec  1 2012 16:42:34", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_6_1_time = mktime(&time);
		
		// Timestamp of DM 2.7
		strptime("Feb 20 2013 14:54:33", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_7_time = mktime(&time);
		
		// Timestamp of DML 2.7
		strptime("Feb 20 2013 14:54:36", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_7_time = mktime(&time);
		
		// Timestamp of DM 2.8
		strptime("Feb 24 2013 14:17:03", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_8_time = mktime(&time);
		
		// Timestamp of DML 2.8
		strptime("Feb 24 2013 13:30:29", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_8_time = mktime(&time);
		
		// Timestamp of DM 2.9
		strptime("Apr  5 2013 18:29:35", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_9_time = mktime(&time);
		
		// Timestamp of DML 2.9
		strptime("Apr  5 2013 18:20:33", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_9_time = mktime(&time);
		
		// Timestamp of DM 2.10
		strptime("May 24 2013 21:22:22", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_10_time = mktime(&time);
		
		// Timestamp of DML 2.10
		strptime("May 24 2013 18:51:58", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_10_time = mktime(&time);
		
		// Timestamp of DM 2.11
		strptime("Jul  2 2014 10:31:15", "%b %d %Y %H:%M:%S", &time);
		const time_t dm_2_11_time = mktime(&time);

		// Timestamp of DML 2.11
		strptime("Jul  2 2014 10:31:06", "%b %d %Y %H:%M:%S", &time);
		const time_t dml_2_11_time = mktime(&time);
		
		for(i = 0; i < size; ++i)
		{
			if(DM_INSTALLED) //DIOS MIOS
			{
				if(DML_OR_DM) //Lite
				{
					memcpy(&buffer, appfile+i+31, sizeof(buffer));

					strptime(buffer, "%b %d %Y %H:%M:%S", &time);
					time_t unixTime = mktime(&time);

					strcat(str, " (DIOS MIOS Lite");

					if(CMP_TIME(dml_2_11_time))			strcat(str, " 2.11+");
					else if(CMP_TIME(dml_2_10_time))	strcat(str, " 2.10");
					else if(CMP_TIME(dml_2_9_time))		strcat(str, " 2.9");
					else if(CMP_TIME(dml_2_8_time))		strcat(str, " 2.8");
					else if(CMP_TIME(dml_2_7_time))		strcat(str, " 2.7");
					else if(CMP_TIME(dml_2_6_time))		strcat(str, " 2.6");
					else if(CMP_TIME(dml_2_5_time))		strcat(str, " 2.5");
					else if(CMP_TIME(dml_2_4_time))		strcat(str, " 2.4");
					else if(CMP_TIME(dml_2_3_time))		strcat(str, " 2.3");
					else if(CMP_TIME(dml_2_3m_time))	strcat(str, " 2.3");
					else if(CMP_TIME(dml_2_2_1_time))	strcat(str, " 2.2.1");
					else if(CMP_TIME(dml_2_2_time))		strcat(str, " 2.2");
					else if(CMP_TIME(dml_1_5_time))		strcat(str, " 1.5");
					else if(CMP_TIME(dml_1_4b_time))	strcat(str, " 1.4b");
					else if(CMP_TIME(dml_1_4_time))		strcat(str, " 1.4");
					else if(CMP_TIME(dml_1_2_time))		strcat(str, " 1.2");
					else if (CMP_TIME(dml_r52_time))	strcat(str, " r52");
					else 								strcat(str, " r51-");

					strcat(str, ")");
					return 1;
				} else {
					memcpy(&buffer, appfile+i+27, sizeof(buffer));

					strptime(buffer, "%b %d %Y %H:%M:%S", &time);
					time_t unixTime = mktime(&time);

					strcat(str, " (DIOS MIOS");
					if(CMP_TIME(dm_2_11_time))			strcat(str, " 2.11+");
					else if(CMP_TIME(dm_2_10_time))		strcat(str, " 2.10");
					else if(CMP_TIME(dm_2_9_time))		strcat(str, " 2.9");
					else if(CMP_TIME(dm_2_8_time))		strcat(str, " 2.8");
					else if(CMP_TIME(dm_2_7_time))		strcat(str, " 2.7");
					else if(CMP_TIME(dm_2_6_1_time))	strcat(str, " 2.6.1");
					else if(CMP_TIME(dm_2_6_0_time))	strcat(str, " 2.6");
					else if(CMP_TIME(dm_2_5_time))		strcat(str, " 2.5");
					else if(CMP_TIME(dm_2_4_time))		strcat(str, " 2.4");
					else if(CMP_TIME(dm_2_3_time))		strcat(str, " 2.3");
					else if(CMP_TIME(dm_2_2_2_time))	strcat(str, " 2.22");
					else if(CMP_TIME(dm_2_2_time))		strcat(str, " 2.2");
					else if(CMP_TIME(dm_2_1_time))		strcat(str, " 2.1");
					else if(CMP_TIME(dm_2_0_time))		strcat(str, " 2.0");
					else 								strcat(str, " 1.0");

					strcat(str, ")");
					return 2;
				}
			}
		}
		if (appfile) free(appfile);
	}
	return 0;
}
