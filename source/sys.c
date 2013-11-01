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
#include "video.h"
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
#include "thread.h"


u8 sysMenuInfoContent = 0;
const char *Regions[] = {"NTSC-J", "NTSC-U", "PAL", "", "KOR", "NTSC-J"}; //Last is actually China

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

// Get the system menu version from TMD
u32 GetSysMenuVersion(void)
{
	static u64 TitleID ATTRIBUTE_ALIGN(32) = 0x0000000100000002LL;
	static u32 tmdSize ATTRIBUTE_ALIGN(32);

	// Get the stored TMD size for the system menu
	if (ES_GetStoredTMDSize(TitleID, &tmdSize) < 0) return false;

	signed_blob *TMD = (signed_blob *)memalign(32, (tmdSize+32)&(~31));
	memset(TMD, 0, tmdSize);

	// Get the stored TMD for the system menu
	if (ES_GetStoredTMD(TitleID, TMD, tmdSize) < 0) return false;

	// Get the system menu version from TMD
	tmd *rTMD = (tmd *)(TMD+(0x140/sizeof(tmd *)));
	u32 version = rTMD->title_version;

	free(TMD);

	// Return the system menu version
	return version;
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

float GetSysMenuNintendoVersion(u32 sysVersion)
{
	float ninVersion = 0.0;

	switch (sysVersion)
	{
		case 33:
			ninVersion = 1.0f;
			break;

		case 97:
		case 128:
		case 130:
			ninVersion = 2.0f;
			break;

		case 162:
			ninVersion = 2.1f;
			break;

		case 192:
		case 193:
		case 194:
			ninVersion = 2.2f;
			break;

		case 224:
		case 225:
		case 226:
			ninVersion = 3.0f;
			break;

		case 256:
		case 257:
		case 258:
			ninVersion = 3.1f;
			break;

		case 288:
		case 289:
		case 290:
			ninVersion = 3.2f;
			break;

		case 352:
		case 353:
		case 354:
		case 326:
			ninVersion = 3.3f;
			break;

		case 384:
		case 385:
		case 386:
			ninVersion = 3.4f;
			break;

		case 390:
			ninVersion = 3.5f;
			break;

		case 416:
		case 417:
		case 418:
			ninVersion = 4.0f;
			break;

		case 448:
		case 449:
		case 450:
		case 454:
		case 54448: // mauifrog's custom version
		case 54449: // mauifrog's custom version
		case 54450: // mauifrog's custom version
		case 54454: // mauifrog's custom version
			ninVersion = 4.1f;
			break;

		case 480:
		case 481:
		case 482:
		case 486:
			ninVersion = 4.2f;
			break;

		case 512:
		case 513:
		case 514:
		case 518:
		case 544:
		case 545:
		case 546:
			ninVersion = 4.3f;
			break;
	}

	return ninVersion;
}

char GetSysMenuRegion(u32 sysVersion) {
	char SysMenuRegion;
	switch(sysVersion)
	{
		case 1:  //Pre-launch
		case 97: //2.0U
		case 193: //2.2U
		case 225: //3.0U
		case 257: //3.1U
		case 289: //3.2U
		case 353: //3.3U
		case 385: //3.4U
		case 417: //4.0U
		case 449: //4.1U
		case 54449: // mauifrog 4.1U
		case 481: //4.2U
		case 513: //4.3U
		case 545:
			SysMenuRegion = 'U';
			break;
		case 130: //2.0E
		case 162: //2.1E
		case 194: //2.2E
		case 226: //3.0E
		case 258: //3.1E
		case 290: //3.2E
		case 354: //3.3E
		case 386: //3.4E
		case 418: //4.0E
		case 450: //4.1E
		case 54450: // mauifrog 4.1E
		case 482: //4.2E
		case 514: //4.3E
		case 546:
			SysMenuRegion = 'E';
			break;
		case 128: //2.0J
		case 192: //2.2J
		case 224: //3.0J
		case 256: //3.1J
		case 288: //3.2J
		case 352: //3.3J
		case 384: //3.4J
		case 416: //4.0J
		case 448: //4.1J
		case 54448: // mauifrog 4.1J
		case 480: //4.2J
		case 512: //4.3J
		case 544:
			SysMenuRegion = 'J';
			break;
		case 326: //3.3K
		case 390: //3.5K
		case 454: //4.1K
		case 54454: // mauifrog 4.1K
		case 486: //4.2K
		case 518: //4.3K
			SysMenuRegion = 'K';
			break;
		default:
			SysMenuRegion = getSystemMenuRegionFromContent();
			break;
	}
	return SysMenuRegion;
}

void zero_sig(signed_blob *sig)
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


// Check fake signatures (aka Trucha Bug)
bool CheckFakeSignature(void)
{
	s32 ret = ES_AddTicket((signed_blob *)ticket_dat, ticket_dat_size, (signed_blob *)certs, sizeof(certs), 0, 0);

	if (ret > -1) RemoveBogusTicket();
	return (ret > -1 || ret == -1028);
}


// Check if you can still call ES_DiVerify (aka ES_Identify) to make IOS think that you actually are a different title
bool CheckESIdentify(void)
{
	int ret = ES_Identify((signed_blob *)certs, sizeof(certs), (signed_blob *)tmd_dat, tmd_dat_size, (signed_blob *)ticket_dat, ticket_dat_size, NULL);
	return ((ret >= 0) || (ret == -2011));
}


// Check flash access
bool CheckFlashAccess(void)
{
	s32 ret = IOS_Open("/dev/flash", 1);
	if (ret >= 0) IOS_Close(ret);
	return (ret >= 0);
}

bool CheckMload(void)
{
	int ret = IOS_Open("/dev/mload", 0);
	if (ret >= 0) IOS_Close(ret);
	return (ret >= 0);
}


// Check NAND access
bool CheckNANDAccess(void)
{
	int ret = IOS_Open("/ticket/00000001/00000002.tik", 1);
	if (ret >= 0) IOS_Close(ret);
	return (ret >= 0);
}


// Check boot2 access
bool CheckBoot2Access(void)
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
	free(buffer);

	free(stmd);
	ptmd = NULL;

	return retValue;

}



s32 get_miosinfo(char *str)
{
	struct tm time;

	// Timestamp of DML r52
	strptime("Mar 7 2012 19:36:06", "%b %d %Y %H:%M:%S", &time);
	const time_t dml_r52_time = mktime(&time);

	// Timestamp of DML 1.2
	strptime("Apr 24 2012 19:44:08", "%b %d %Y %H:%M:%S", &time);
	const time_t dml_1_2_time = mktime(&time);

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


	u32 size = 0;
	u32 i = 0;
	s32 ret = 0;
	u8 *appfile = NULL;
	char buffer[20];

	ret = read_file_from_nand("/title/00000001/00000101/content/0000000c.app", &appfile, &size);
	if(ret >= 0 && appfile)
	{
		for(i = 0; i < size; ++i)
		{
			if((*(vu32*)(appfile+i)) == 0x44494F53 && (*(vu32*)(appfile+i+5)) == 0x4D494F53) //DIOS MIOS
			{
				if(*(vu32*)(appfile+i+10) == 0x4C697465) //Lite
				{
					memcpy(&buffer, appfile+i+31, sizeof(buffer));

					strptime(buffer, "%b %d %Y %H:%M:%S", &time);
					time_t unixTime = mktime(&time);

					strcat(str, " (DIOS MIOS Lite");

					if(difftime(unixTime, dml_2_6_time) >= 0)			strcat(str, " 2.6+");
					else if(difftime(unixTime, dml_2_5_time) >= 0)		strcat(str, " 2.5+");
					else if(difftime(unixTime, dml_2_4_time) >= 0)		strcat(str, " 2.4+");
					else if(difftime(unixTime, dml_2_3_time) >= 0)		strcat(str, " 2.3+");
					else if(difftime(unixTime, dml_2_3m_time) >= 0)		strcat(str, " 2.3+");
					else if(difftime(unixTime, dml_2_2_1_time) >= 0)	strcat(str, " 2.2.1+");
					else if(difftime(unixTime, dml_2_2_time) >= 0)		strcat(str, " 2.2+");
					else if(difftime(unixTime, dml_1_5_time) >= 0)		strcat(str, " 1.5+");
					else if(difftime(unixTime, dml_1_4b_time) >= 0)		strcat(str, " 1.4b+");
					else if(difftime(unixTime, dml_1_2_time) > 0)		strcat(str, " 1.4+");
					else if(difftime(unixTime, dml_1_2_time) == 0)		strcat(str, " 1.2");
					else if (difftime(unixTime, dml_r52_time) >= 0)		strcat(str, " r52");
					else strcat(str, " r51-");

					strcat(str, ")");
					return 1;
				} else {
					memcpy(&buffer, appfile+i+27, sizeof(buffer));

					strptime(buffer, "%b %d %Y %H:%M:%S", &time);
					time_t unixTime = mktime(&time);

					strcat(str, " (DIOS MIOS");

					if(difftime(unixTime, dm_2_6_1_time) >= 0)        	strcat(str, " 2.6.1+");
					else if(difftime(unixTime, dm_2_6_0_time) >= 0)    strcat(str, " 2.6+");
					else if(difftime(unixTime, dm_2_5_time) >= 0)      strcat(str, " 2.5+");
					else if(difftime(unixTime, dm_2_4_time) >= 0)      strcat(str, " 2.4+");
					else if(difftime(unixTime, dm_2_3_time) >= 0)      strcat(str, " 2.3+");
					else if(difftime(unixTime, dm_2_2_2_time) >= 0)    strcat(str, " 2.2.2+");
					else if(difftime(unixTime, dm_2_2_time) >= 0)      strcat(str, " 2.2+");
					else if(difftime(unixTime, dm_2_1_time) >= 0)      strcat(str, " 2.1+");
					else if(difftime(unixTime, dm_2_0_time) >= 0)      strcat(str, " 2.0+");

					strcat(str, ")");
					return 2;
				}
			}
		}
		memset(appfile, 0, size);
		free(appfile);
	}

	return 0;
}