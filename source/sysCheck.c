#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <gccore.h>
#include <malloc.h>
#include <fat.h>
#include <unistd.h>
#include <ogc/conf.h>
#include <ogc/es.h>
#include <ogc/ios.h>
#include <wiiuse/wpad.h>
#include <stdlib.h>
#include <stdarg.h>
#include <di/di.h>
#include <CheckRegion.h>

#include "tmd_dat.h"
#include "sys.h"
#include "video.h"
#include "fatMounter.h"
#include "languages.h"
#include "gui.h"
#include "mload.h"
#include "title.h"
#include "sha1.h"
#include "wiibasics.h"
#include "tmdIdentification.h"
#include "gecko.h"
#include "update.h"
#include "thread.h"

// Filename
#define REPORT "sd:/sysCheck.csv"
#define HASHLOG "sd:/IOSsyscheck.log"
#define VERSION_1_1_0 65536


extern bool geckoinit;
extern void ReloadIOS(int version);

char miosInfo[128] = {0};
extern void __exception_setreload(int t);


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

			//printf("Error! ES_GetStoredTMDSize: %d\n", ret);

			sprintf(filepath, "/title/%08x/%08x/content/title.tmd", TITLE_UPPER(title), TITLE_LOWER(title));

			ret = ISFS_Open(filepath, ISFS_OPEN_READ);
			if (ret <= 0)
			{
				printf("Error! ISFS_Open (ret = %d)\n", ret);
				return 0;
			}

			fd = ret;

			ret = ISFS_Seek(fd, 0x184, 0);
			if (ret < 0)
			{
				printf("Error! ISFS_Seek (ret = %d)\n", ret);
				return 0;
			}

			ret = ISFS_Read(fd,tmd_buf,8);
			if (ret < 0)
			{
				printf("Error! ISFS_Read (ret = %d)\n", ret);
				return 0;
			}

			ret = ISFS_Close(fd);
			if (ret < 0)
			{
				printf("Error! ISFS_Close (ret = %d)\n", ret);
				return 0;
			}

			return be64(tmd_buf);

		} else {
			// Normal versions of IOS won't have a problem, so we do things the "right" way.

			// Some of this code adapted from bushing's title_lister.c
			signed_blob *s_tmd = (signed_blob *)tmd_buf;
			ret = ES_GetStoredTMD(title, s_tmd, tmd_size);
			if (ret < 0){
				printf("Error! ES_GetStoredTMD: %d\n", ret);
				return -1;
			}
			tmd *t = SIGNATURE_PAYLOAD(s_tmd);
			return t->sys_version;
		}

	}
	return 0;
}

bool getInfoFromContent(IOS *ios) {
	bool retValue = false;
	iosinfo_t *iosinfo = NULL;
	char filepath[ISFS_MAXPATH] ATTRIBUTE_ALIGN(0x20);
	u8 *buffer = 0;
	u32 filesize = 0;
	s32 ret = 0;

	// Try to identify the cIOS by the info put in by the installer/ModMii
	sprintf(filepath, "/title/%08x/%08x/content/%08x.app", 0x00000001, ios->titleID, ios->infoContent);
	ret = read_file_from_nand(filepath, &buffer, &filesize);

	iosinfo = (iosinfo_t *)(buffer);
	if (ret >= 0 && ios->titleID == 252 && ios->num_contents == 1) {
		const char *checkStr = "bootcb2";
		int i;
		for (i = 0; i < filesize - strlen(checkStr); i++)
		{
			if (!strncmp((char*)buffer + i, checkStr, strlen(checkStr)))
			{
				sprintf(ios->info, " cBoot252");
				gprintf("is cBoot252\n");
				logfile("is cBoot252\r\n");
				retValue = true;
				ios->isStub = true;
				break;
			}
		}
		if (buffer != 0)
		{
			free(buffer);
		}
	} else if (ret >= 0 && iosinfo != NULL && iosinfo->magicword == 0x1ee7c105 && iosinfo->magicversion == 1) {
		if (ios->titleID != iosinfo->baseios)
			ios->baseIOS = iosinfo->baseios;

		if (strcmp(iosinfo->name, "nintendo") == 0)
			sprintf(ios->info, "rev %u", iosinfo->version);
		else
			sprintf(ios->info, "%s-v%u%s", iosinfo->name, iosinfo->version, iosinfo->versionstring);
		gprintf("is %s\n", ios->info);
		logfile("is %s\r\n", ios->info);
		retValue = true;
		if (buffer) free(buffer);
	}

	return retValue;
}

void formatDate(u32 date, char ReportBuffer[200][100]) {
	char temp[8] = {0};
	char day[2] = {0};
	char month[2] = {0};
	char year[4] = {0};

	sprintf(temp, "%08x", date);
	sprintf(year, "%c%c%c%c", temp[0], temp[1], temp[2], temp[3]);
	sprintf(month, "%c%c", temp[4], temp[5]);
	sprintf(day, "%c%c", temp[6], temp[7]);

	gprintf("MONTH: %s\n", month);
	gprintf("DAY: %s\n", day);
	gprintf("YEAR: %s\n", year);
	logfile("MONTH: %s\r\n", month);
	logfile("DAY: %s\r\n", day);
	logfile("YEAR: %s\r\n", year);

	char result[10] = {0};

	switch (CONF_GetLanguage()) {
		case CONF_LANG_GERMAN:
		case CONF_LANG_ITALIAN:
		case CONF_LANG_SPANISH:
			sprintf(result, "%s.%s.%s", day, month, year);
			break;
		default:
			sprintf(result, "%s.%s.%s", month, day, year); // You don't say "I was born 1990 January 1"  The year comes last
			break;
	}
	gprintf("String: %s\n", result);
	logfile("String: %s\r\n", result);
	if (strlen(result) > 1)
		sprintf(ReportBuffer[DVD], TXT_DVD, result);
	else
		sprintf(ReportBuffer[DVD], TXT_NoDVD);
}

void sort(u64 *titles, u32 cnt) {
	int i, j;
	for (i = 0; i < cnt -1; ++i) {
		for (j = 0; j < cnt - i - 1; ++j) {
			if (titles[j] > titles[j + 1]) {
				u64 tmp = titles[j];
				titles[j] = titles[j + 1];
				titles[j + 1] = tmp;
			}
		}
	}
}
// Main 
int main(int argc, char **argv)
{
	__exception_setreload(2);
	bool forceNoAHBPROT = false;
	geckoinit = InitGecko();
	if(argc>=1){
		int i;
		for(i=0; i<argc; i++){
			if(strncmp("--debug=true", argv[i], 12)==0){
				debug = true;
				gprintf("--debug=true\n");
			} else if(strncmp("--forceNoAHBPROT=true", argv[i], 21)==0){
				forceNoAHBPROT = true;
				gprintf("--forceNoAHBPROT=true\n");
			}
		}
	}
	

	if (HAVE_AHBPROT && !forceNoAHBPROT)
		IOSPATCH_Apply();
	bool nandAccess = CheckNANDAccess();

	// Get and display the current date and time
	struct tm today;
	time_t rawtime;
	time(&rawtime);
	today = *localtime(&rawtime);

	initGUI();
	initLanguages(today);

	// Get the console region
	printLoading(MSG_GetConsoleRegion);
	//usleep(250000);
	int regionSelection = CONF_GetRegion();
	
	u8 shopcode = 0;
	char country[COUNTRY_SIZE] = "Unknown";
	if (!CONF_GetShopCode(&shopcode)) strncpy(country, CONF_CountryCodes[shopcode], COUNTRY_SIZE);

	// Get the system menu version
	printLoading(MSG_GetSysMenuVer);
	//usleep(250000);
	u32 sysVersion = GetSysMenuVersion();
	sysMenu systemmenu;

	printLoading(MSG_GetHBCVer);
	//usleep(250000);
	u32 hbcversion = 0;
	u32 hbfversion = 0;
	s32 hbc = 0;
	s32 hbf = 0;
	u32 hbcIOS = 0;
	u32 dvdSupport = 0;
	s32 ret = Title_GetVersionNObuf(0x000100014C554C5All);
	if (ret<0) {
		ret = Title_GetVersionNObuf(0x00010001AF1BF516ll);
		if (ret<0) {
			ret = Title_GetVersionNObuf(0x000100014A4F4449ll);
			if (ret<0) {
				hbc = 1;
				ret = Title_GetVersionNObuf(0x0001000148415858ll);
				if (ret<0) {
					hbc = 0;
				} else {
					hbc = 1;
					hbcversion = ret;
				}
			} else {
				hbc = 2;
				hbcversion = ret;
			}
		} else {
			hbc = 3;
			hbcversion = ret;
			if (hbcversion == 0)
				hbcversion = VERSION_1_1_0;
		}
	} else {
		hbc = 4;
		hbcversion = (ret != 257) +1;
	}
	if (hbc == 4) {
		hbcIOS =  get_title_ios(TITLE_ID(0x10001, 0x4C554C5A));
	} else if (hbc == 3) {
		hbcIOS =  get_title_ios(TITLE_ID(0x10001, 0xAF1BF516));
	} else if (hbc == 2) {
		hbcIOS =  get_title_ios(TITLE_ID(0x10001, 0x4A4F4449));
	} else if (hbc == 1) {
		hbcIOS = get_title_ios(TITLE_ID(0x10001, 0x48415858));
	}

	ret = Title_GetVersionNObuf(0x0001000148424630LL); //HBF0
	if (ret<0) {
		ret = Title_GetVersionNObuf(0x0001000154484246LL); //THBF
		if (ret<0) {
			hbf = 0;
		} else {
			hbf = 1;
			hbfversion = ret;
		}
	} else {
		hbf = 2;
		hbfversion = ret;
	}

	if (HAVE_AHBPROT && !forceNoAHBPROT) {
		DI_Init();
		DI_DriveID id;

		if(DI_Identify(&id) == 0) {
			dvdSupport = id.rel_date;
		}
		DI_Close();
	}

	float sysNinVersion = GetSysMenuNintendoVersion(sysVersion);
	char SysMenuRegion = GetSysMenuRegion(sysVersion);

	// Get the running IOS version and revision
	u32 runningIOS = IOS_GetVersion();
	u32 runningIOSRevision = IOS_GetRevision();
	printLoading(MSG_GetRunningIOS);
	//usleep(250000);

	// Get the console ID
	printLoading(MSG_GetConsoleID);
	//usleep(250000);
	u32 deviceID = GetDeviceID();

	// Get the boot2 version
	printLoading(MSG_GetBoot2);
	//usleep(250000);
	u32 boot2version = GetBoot2Version();

	// Get number of titles
	printLoading(MSG_GetNrOfTitles);
	//usleep(250000);

	u32 tempTitles;
	if (ES_GetNumTitles(&tempTitles) < 0) {
		PauseThread();
		printError(ERR_GetNrOfTitles);
		sleep(5);
		deinitGUI();
		exit(1);
		//return false;
	}

	s32 nbTitles = tempTitles;

	// Allocate the memory for titles
	u64 *titles = memalign(32, nbTitles*sizeof(u64));
	if (titles == NULL) {
		sprintf(MSG_Buffer, ERR_AllocateMemory, titles);
		printError(MSG_Buffer);
		sleep(5);
		deinitGUI();
		exit(1);
	}

	// Get list of titles
	printLoading(MSG_GetTitleList);
	//usleep(250000);
	if (ES_GetTitles(titles, nbTitles) < 0) {
		printError(ERR_GetTitleList);
		sleep(5);
		deinitGUI();
		exit(1);
	}

	int i;
	int j;
	int countIOS = 0; // Number of IOS
	int countStubs = 0; // Number of IOS Stubs
	int countBCMIOS = 0; //Number of BC and MIOS. Should be 2.
	//u32 titleID;
	char HashLogBuffer[300][100] = {{0}};
	int lines = 0;

	// For each titles found
	for (i = 0; i < nbTitles; i++)
	{
		// Skip non-system titles
		if (titles[i] >> 32 != 1) {
			titles[i] = 0;
			continue;
		}
		u32 titleID = titles[i] & 0xFFFFFFFF;

		// Skip BC, MIOS and possible other non-IOS titles
		if (titleID > 200 && titleID < 258) {
			u32 tmdSize = 0;
			if (ES_GetStoredTMDSize(0x0000000100000000ULL | titleID, &tmdSize) < 0) {
				titles[i] = 0;
				continue;
			}
		}

		// Skip the running IOS, the System Menu, and out of range IOS
		if ((titleID > 258 && titleID < 512) || (titleID == 0) || (titleID == 2)) {
			titles[i] = 0;
			continue;
		}
		countIOS++;
	}

	// Sort IOS titles
	printLoading(MSG_SortTitles);
	//usleep(250000);

	u64 *newTitles = memalign(32, countIOS*sizeof(u64));
	u32 cnt = 0;
	for (i = 0; i < nbTitles; i++) {
		if (titles[i] > 0) {
			newTitles[cnt] = titles[i];
			cnt++;
		}
	}

	sort(newTitles, countIOS);
	free(titles);

	IOS ios[countIOS];
	// ios Liste initialisieren
	//for (i = 0; i < countIOS; i++) {
	for (i = countIOS; i--;) { // Should be slightly faster
		ios[i].infoContent = 0;
		ios[i].titleID = 0;
		ios[i].mloadVersion = 0;
		ios[i].baseIOS = -1;
		sprintf(ios[i].info, "NULL");
		ios[i].isStub = false;
		ios[i].revision = 0;
		ios[i].infoFakeSignature = false;
		ios[i].infoESIdentify = false;
		ios[i].infoFlashAccess = false;
		ios[i].infoNANDAccess = false;
		ios[i].infoBoot2Access = false;
		ios[i].infoUSB2 = false;
		ios[i].infoVersionPatch = false;
	}

	MountSD();
	NandStartup();

	// Check Priiloader
	int priiloader = checkSysLoader();

	// Check MIOS
	if (nandAccess) get_miosinfo(miosInfo);

	// For each titles found
	//for (i = 0; i < countIOS; i++)
	for (i = countIOS; i--;) // Should be slightly faster
	{
		ios[i].titleID = newTitles[i] & 0xFFFFFFFF;

		// Check if this title is an IOS stub
		u32 tmdSize = 0;
		tmd *iosTMD = NULL;
		signed_blob *iosTMDBuffer = NULL;

		// Get the stored TMD size for the title
		if (ES_GetStoredTMDSize(0x0000000100000000ULL | ios[i].titleID, &tmdSize) < 0)
		{
			sprintf(MSG_Buffer, ERR_GetIosTMDSize, ios[i].titleID);
			printError(MSG_Buffer);
			sleep(5);
			deinitGUI();
			exit(1);
		}

		iosTMDBuffer = (signed_blob*)memalign(32, (tmdSize+31)&(~31));
		memset(iosTMDBuffer, 0, tmdSize);

		// Get the stored TMD for the title
		logfile("Getting TMD for title %d\r\n", ios[i].titleID);
		gprintf("Getting TMD for title %d\n", ios[i].titleID);
		if (ES_GetStoredTMD(0x0000000100000000ULL | ios[i].titleID, iosTMDBuffer, tmdSize) < 0)
		{
			sprintf(MSG_Buffer, ERR_GetIosTMD, ios[i].titleID);
			printError(MSG_Buffer);
			sleep(5);
			deinitGUI();
			exit(1);
		}

		iosTMD = (tmd*)SIGNATURE_PAYLOAD(iosTMDBuffer);

		// Get the title version
		u32 titleSize = Title_GetSize_FromTMD(iosTMD);
		ios[i].revision = iosTMD->title_version;
		ios[i].isStub = false;
		ios[i].infoContent = *(u8 *)((u32)iosTMDBuffer+0x1E7);
		ios[i].num_contents = iosTMD->num_contents;
		gprintf("ios%d rev%d iosTMD->num_contents = %d size=%d\n", ios[i].titleID, ios[i].revision, iosTMD->num_contents, titleSize);
		logfile("ios%d rev%d iosTMD->num_contents = %d size=%d\r\n", ios[i].titleID, ios[i].revision, iosTMD->num_contents, titleSize);

		// Check if this is an IOS stub (according to WiiBrew.org)
		if (IsKnownStub(ios[i].titleID, ios[i].revision))
			ios[i].isStub = true;
		else
		{
			if (ios[i].titleID != 256 && ios[i].titleID != 257 && ios[i].titleID != 512 && ios[i].titleID != 513 && titleSize < 0x100000)
				ios[i].isStub = true;
			else
				ios[i].isStub = false;

			if (ios[i].isStub) {
				gprintf("is stub\n");
				logfile("is stub\r\n");
			}
		}

		if (!ios[i].isStub || ios[i].titleID == 252) {
			if (nandAccess)
				if (!getInfoFromContent(&ios[i])) {
					// Hash des TMDs abrufen
					iosTMD->title_id = ((u64)(1) << 32) | 249;
					brute_tmd(iosTMD);

					sha1 hash;
					SHA1((u8 *)iosTMDBuffer, tmdSize, hash);

					sprintf(HashLogBuffer[lines], "IOS%d get_ios_base: \n%x %x %x %x, %x %x %x %x, %x %x %x %x, %x %x %x %x, %x %x %x %x\n", ios[i].titleID, (char)hash[0], (char)hash[1], (char)hash[2], (char)hash[3],  (char)hash[4], (char)hash[5], (char)hash[6], (char)hash[7],  (char)hash[8], (char)hash[9], (char)hash[10], (char)hash[11], (char)hash[12], (char)hash[13], (char)hash[14], (char)hash[15], (char)hash[16], (char)hash[17], (char)hash[18], (char)hash[19]);
					lines++;

					for (j = 0;j < base_number;j++)
					{
						// Hashes berprfen
						if (memcmp((void *)hash, (u32 *)&iosHash[j].hashes, sizeof(sha1)) == 0)
						{
							if (ios[i].titleID != iosHash[j].base)
								ios[i].baseIOS = iosHash[j].base;
							strcpy(ios[i].info, iosHash[j].info);
							gprintf("is %s\n", ios[i].info);
							logfile("is %s\r\n", ios[i].info);
						}
					}
				}
		}

		free(iosTMDBuffer);

		if (ios[i].titleID == 256 || ios[i].titleID == 257) countBCMIOS++;

		if (ios[i].isStub && !(iosTMD->title_version == 31338) && !(iosTMD->title_version == 65281) && !(iosTMD->title_version == 65535)) countStubs++;
	}

	// Check if this title is an IOS stub
	u32 tmdSize = 0;
	signed_blob *iosTMDBuffer = NULL;

	// Get the stored TMD size for the title
	if (ES_GetStoredTMDSize(0x0000000100000000ULL | 2, &tmdSize) < 0)
	{
		sprintf(MSG_Buffer, ERR_GetIosTMDSize, 2);
		printError(MSG_Buffer);
		sleep(5);
		deinitGUI();
		exit(1);
	}

	iosTMDBuffer = (signed_blob*)memalign(32, (tmdSize+31)&(~31));
	memset(iosTMDBuffer, 0, tmdSize);

	// Get the stored TMD for the title
	if (ES_GetStoredTMD(0x0000000100000000ULL | 2, iosTMDBuffer, tmdSize) < 0)
	{
		sprintf(MSG_Buffer, ERR_GetIosTMD, 2);
		printError(MSG_Buffer);
		sleep(5);
		deinitGUI();
		exit(1);
	}

	char filepath[ISFS_MAXPATH] ATTRIBUTE_ALIGN(0x20);
	u8 *buffer = 0;
	u32 filesize = 0;
	iosinfo_t *sysInfo;

	// Try to identify the cIOS by the info put in by the installer/ModMii
	sysMenuInfoContent = *(u8 *)((u32)iosTMDBuffer+0x1E7);
	sprintf(filepath, "/title/%08x/%08x/content/%08x.app", 0x00000001, 2, sysMenuInfoContent);
	gprintf("/title/%08x/%08x/content/%08x.app\n", 0x00000001, 2, sysMenuInfoContent);
	ret = read_file_from_nand(filepath, &buffer, &filesize);

	sysInfo = (iosinfo_t *)(buffer);
	if (ret >= 0 && sysInfo != NULL && sysInfo->magicword == 0x1ee7c105 && sysInfo->magicversion == 1)
	{
		systemmenu.realRevision = sysInfo->version;
		systemmenu.hasInfo = true;
		//strcpy(systemmenu.info, sysInfo->name);
		sprintf(systemmenu.info, "%s%s", sysInfo->name, sysInfo->versionstring);
		if (buffer != NULL) free(buffer);
	} else {
		systemmenu.realRevision = 0;
		systemmenu.hasInfo = false;
		strcpy(systemmenu.info, "NONE");
	}

	NandShutdown();
	UnmountSD();

	u32 countTitles = nbTitles;
	nbTitles = countIOS;

	// Get the certificates from the NAND
	printLoading(MSG_GetCertificates);
	//usleep(250000);
	if (!GetCertificates()) {
		printError(ERR_GetCertificates);
		sleep(5);
		deinitGUI();
		exit(1);
	}

	//Select an IOS to test
	WPAD_Init();
	int selectedIOS = -1;
	u16 wpressed;
	time_t starttime;
	starttime = time(NULL);

	printSelectIOS(MSG_SelectIOS, MSG_All);

	bool completeReport = true;

	while (difftime (time(NULL),starttime) < 15) {
	//while(1) {
		WPAD_ScanPads();
		wpressed = WPAD_ButtonsHeld(0);
		usleep(50000);

		if (wpressed & WPAD_BUTTON_RIGHT && selectedIOS < (nbTitles-1)){
			selectedIOS++;
			starttime = time(NULL);

			u32 titleID = newTitles[selectedIOS] & 0xFFFFFFFF;

			switch (titleID)
			{
				case 256:
					sprintf(MSG_Buffer, "BC");
					break;

				case 257:
					sprintf(MSG_Buffer, "MIOS");
					break;

				default:
					sprintf(MSG_Buffer, "IOS%d", titleID);
					break;
			}
			printSelectIOS(MSG_SelectIOS, MSG_Buffer);
		}

		if (wpressed & WPAD_BUTTON_LEFT && selectedIOS > -1) {
			selectedIOS--;
			starttime = time(NULL);

			if (selectedIOS > -1) {
				u32 titleID = newTitles[selectedIOS] & 0xFFFFFFFF;

				switch (titleID)
				{
					case 256:
						sprintf(MSG_Buffer, "BC");
						break;

					case 257:
						sprintf(MSG_Buffer, "MIOS");
						break;

					default:
						sprintf(MSG_Buffer, "IOS%d", titleID);
						break;
				}

				printSelectIOS(MSG_SelectIOS, MSG_Buffer);
			} else {
				printSelectIOS(MSG_SelectIOS, MSG_All);
			}
		}

		if (wpressed & WPAD_BUTTON_PLUS) {
			printLoading(MSG_Update);
			ret = updateApp();
			if (ret == 2) {
				printSuccess(MSG_NoUpdate);
				sleep(5);
				starttime = time(NULL);
			} else if (ret >= 0) {
				printSuccess(MSG_UpdateSuccess);
				sleep(10);
				deinitGUI();
				if (*(u32*)0x80001800) exit(0);
				SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
			} else if (ret < 0) {
				printError(MSG_UpdateFail);
				sleep(5);
				starttime = time(NULL);
			}
		}

		if (wpressed & WPAD_BUTTON_A) {
			break;
		}
	}
	WPAD_Shutdown();
	if (selectedIOS > -1) {
		nbTitles = 1;
		completeReport = false;
	}

	// Test vulnerabilities in IOS
	ResumeThread();
	for (i = 0; i < nbTitles; i++)
	{
		if (selectedIOS > -1) i = selectedIOS; //If specific IOS is selected

		if (ios[i].titleID == 256) sprintf(MSG_Buffer2, "BC");
		else if (ios[i].titleID == 257) sprintf(MSG_Buffer2, "MIOS");
		else sprintf(MSG_Buffer2, "IOS%d", ios[i].titleID);

		sprintf(MSG_Buffer, MSG_TestingIOS, MSG_Buffer2);
		printLoadingBar(MSG_Buffer, (100.0/(nbTitles-1)*(i+1)));

		if (ios[i].isStub || ios[i].titleID == 256 || ios[i].titleID == 257)
		{
			ios[i].infoFakeSignature = false;
			ios[i].infoESIdentify = false;
			ios[i].infoFlashAccess = false;
			ios[i].infoNANDAccess = false;
			ios[i].infoBoot2Access = false;
			ios[i].infoUSB2 = false;
		}
		else
		{
			// Reload IOS
			gprintf("// IOS_ReloadIOS(%d)\n", ios[i].titleID);
			logfile("// IOS_ReloadIOS(%d)\r\n", ios[i].titleID);
			//IOS_ReloadIOS(ios[i].titleID);
			ReloadIOS(ios[i].titleID);

			// Test fake signature
			gprintf("// Test fake signature\n");
			logfile("// Test fake signature\r\n");
			ios[i].infoFakeSignature = (CheckFakeSignature());

			// Test ES Identify
			gprintf("// Test ES Identify\n");
			logfile("// Test ES Identify\r\n");
			ios[i].infoESIdentify = (CheckESIdentify());

			// Test Flash Access
			gprintf("// Test Flash Access\n");
			logfile("// Test Flash Access\r\n");
			ios[i].infoFlashAccess = (CheckFlashAccess());

			// Test NAND Access
			gprintf("// Test NAND Access\n");
			logfile("// Test NAND Access\r\n");
			ios[i].infoNANDAccess = (CheckNANDAccess());

			// Test Boot2 Access
			gprintf("// Test Boot2 Access\n");
			logfile("// Test Boot2 Access\r\n");
			ios[i].infoBoot2Access = (CheckBoot2Access());

			// Test USB 2.0
			gprintf("// Test USB 2.0\n");
			logfile("// Test USB 2.0\r\n");
			ios[i].infoUSB2 = (CheckUSB2(ios[i].titleID));

			if (ios[i].infoFakeSignature) {
				//ios[i].infoVersionPatch = (CheckVersionPatch());
			}

			// Check Priiloader
			if (!nandAccess && priiloader == -2 && ios[i].infoNANDAccess) {
				priiloader = checkSysLoader();
			}

			// Check Base IOS
			if (!nandAccess && ios[i].infoNANDAccess) {
				NandStartup();
				int k = 0;
				for (k = 0; k < nbTitles; k++) {
					if ((ios[i].isStub || ios[i].titleID == 256 || ios[i].titleID == 257) && ios[i].titleID != 252) continue;
					getInfoFromContent(&ios[k]);
				}
				NandShutdown();
			}
			if ((ios[i].titleID==222 || ios[i].titleID==224 || ios[i].titleID==223 || ios[i].titleID==202 || ios[i].titleID==225) && ios[i].baseIOS < 0) {
				ret = mload_init();
				gprintf("// mload_get_IOS_base()\n");
				logfile("// mload_get_IOS_base()\r\n");
				ios[i].baseIOS = mload_get_IOS_base();
				gprintf("// mload_get_version()\n");
				logfile("// mload_get_version()\r\n");
				ios[i].mloadVersion = mload_get_version();
				mload_close();
			}
		}
	}


	// Reload the running IOS
	//IOS_ReloadIOS(runningIOS);
	ReloadIOS(runningIOS);
	sprintf(MSG_Buffer, MSG_ReloadIOS, runningIOS, runningIOSRevision);
	printLoading(MSG_Buffer);
	//usleep(250000);

	//--Generate Report--
	printLoading(MSG_GenerateReport);
	//usleep(250000);

		char ReportBuffer[200][100] = {{0}};

		if (dvdSupport > 0)
			formatDate(dvdSupport, ReportBuffer);
		else
			sprintf(ReportBuffer[DVD], TXT_NoDVD);

		// Display Title
		sprintf(ReportBuffer[APP_TITLE], TXT_AppTitle, TXT_AppVersion);
		sprintf(ReportBuffer[APP_IOS], TXT_AppIOS, runningIOS, IOS_GetRevision());
		bool validregion = regionSelection >= CONF_REGION_JP && regionSelection <= CONF_REGION_CN;

		// Display the console region
		if (sysNinVersion != 0.0f) {
			sprintf(ReportBuffer[TEXT_REGION], "%s: %s", TXT_Region, validregion ? Regions[regionSelection] : "");
			if (validregion)
				sprintf(ReportBuffer[SYSMENU], TXT_SysMenu, sysNinVersion, SysMenuRegion, sysVersion);
			else
				strcat(ReportBuffer[SYSMENU], TXT_Unknown);
			
		} else if (systemmenu.hasInfo) {
			u32 realSysVersion = systemmenu.realRevision;
			sysNinVersion = GetSysMenuNintendoVersion(realSysVersion);
			SysMenuRegion = GetSysMenuRegion(sysVersion);
			sprintf(ReportBuffer[TEXT_REGION], "%s: %s", TXT_Region, validregion ? Regions[regionSelection] : "");
			if (validregion)
				sprintf(ReportBuffer[SYSMENU], TXT_SysMenu3, sysNinVersion, SysMenuRegion, sysVersion, realSysVersion, systemmenu.info);
			else
				strcat(ReportBuffer[SYSMENU], TXT_Unknown);
		} else {
			signed_blob *TMD = NULL;
			tmd *t = NULL;
			u32 TMD_size = 0;

			ret = GetTMD((((u64)(1) << 32) | (0x0000000100000002LL)), &TMD, &TMD_size);

			t = (tmd*)SIGNATURE_PAYLOAD(TMD);
			t->title_id = ((u64)(1) << 32) | 249;
			brute_tmd(t);

			sha1 hash;
			SHA1((u8 *)TMD, TMD_size, hash);

			free(TMD);

			u32 hashtest[5] = {0xc6404e23, 0x39eff390, 0x1d17c28f, 0xc3970680, 0xf44524e7};

			if (memcmp((void *)hash, (u32 *)&hashtest, sizeof(sha1)) == 0)
			{
				sysNinVersion = 4.1f;
				sprintf(ReportBuffer[TEXT_REGION], "%s: PAL", TXT_Region);
				sprintf(ReportBuffer[SYSMENU], TXT_SysMenu, sysNinVersion, "E", sysVersion);
			} else {
				s32 sysIOS = get_title_ios(TITLE_ID(0x00000001, 0x00000002));

				char Region[100];

				switch (regionSelection)
				{
					case CONF_REGION_US:
						sprintf(ReportBuffer[TEXT_REGION], "%s: NTSC-U", TXT_Region);
						break;

					case CONF_REGION_EU:
						sprintf(ReportBuffer[TEXT_REGION], "%s: PAL", TXT_Region);
						break;

					case CONF_REGION_JP:
						sprintf(ReportBuffer[TEXT_REGION], "%s: NTSC-J", TXT_Region);
						break;

					case CONF_REGION_KR:
						sprintf(ReportBuffer[TEXT_REGION], "%s: KOR", TXT_Region);
						break;

					default:
						sprintf(ReportBuffer[TEXT_REGION], "%s: ", TXT_Region);
						strcat(ReportBuffer[TEXT_REGION], TXT_Unknown);
				}
				sprintf(Region, "%c", SysMenuRegion);

				switch (sysIOS)
				{
					case 9:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "1.0", Region, sysVersion);
					break;

					case 11:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "2.0/2.1", Region, sysVersion);
					break;

					case 20:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "2.2", Region);
					break;

					case 30:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "3.0/3.1/3.2/3.3", Region, sysVersion);
					break;

					case 40:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "3.3", Region, sysVersion);
					break;

					case 50:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "3.4", Region, sysVersion);
					break;

					case 60:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "4.0/4.1", Region, sysVersion);
					break;

					case 70:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "4.2", Region, sysVersion);
					break;

					case 80:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "4.3", Region, sysVersion);
					break;

					default:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "0.0", Region, sysVersion);

				}
			}
		}

		switch (CONF_GetSerialCode(NULL)) {
			case CONF_CODE_JPN:
			case CONF_CODE_VJPNI:
			case CONF_CODE_VJPNO:
				// JAP
				if (regionSelection != CONF_REGION_JP) {
					strcat(ReportBuffer[TEXT_REGION], TXT_OriginalRegion);
					strcat(ReportBuffer[TEXT_REGION], "JAP)");
				}
				break;
			case CONF_CODE_USA:
			case CONF_CODE_USAK:
			case CONF_CODE_VUSAI:
			case CONF_CODE_VUSAO:
				// USA
				if (regionSelection != CONF_REGION_US) {
					strcat(ReportBuffer[TEXT_REGION], TXT_OriginalRegion);
					strcat(ReportBuffer[TEXT_REGION], "USA)");
				}
				break;
			case CONF_CODE_EURH:
			case CONF_CODE_EURHK:
			case CONF_CODE_EURM:
			case CONF_CODE_EURMK:
			case CONF_CODE_EURF:
			case CONF_CODE_EURFK:
			case CONF_CODE_VEURHI:
			case CONF_CODE_VEURHO:
			case CONF_CODE_VEURMI:
			case CONF_CODE_VEURMO:
			case CONF_CODE_VEURFI:
			case CONF_CODE_VEURFO:
				// EU
				if (regionSelection != CONF_REGION_EU) {
					strcat(ReportBuffer[TEXT_REGION], TXT_OriginalRegion);
					strcat(ReportBuffer[TEXT_REGION], "PAL)");
				}
				break;
			case CONF_CODE_KOR:
				// KOR
				if (regionSelection != CONF_REGION_KR) {
					strcat(ReportBuffer[TEXT_REGION], TXT_OriginalRegion);
					strcat(ReportBuffer[TEXT_REGION], "KOR)");
				}
				break;
			case CONF_CODE_AUS:
			case CONF_CODE_AUSK:
			case CONF_CODE_VAUSI:
			case CONF_CODE_VAUSO:
				// AUS
				strcat(ReportBuffer[TEXT_REGION], TXT_OriginalRegion);
				strcat(ReportBuffer[TEXT_REGION], "AUS)");
				break;
			default:
				break;
		}

		if (priiloader == 1)
			sprintf(ReportBuffer[PRIILOADER], TXT_Priiloader);
		else if (priiloader == 2)
			sprintf(ReportBuffer[PRIILOADER], TXT_PreFiix);

		if (hbc == 0 || hbcversion == 0)
			sprintf(ReportBuffer[HBC], "Homebrew Channel is not installed");
		else if (hbcIOS == 0)
			sprintf(ReportBuffer[HBC], TXT_HBC_STUB);
		else if (hbc == 4)
			sprintf(ReportBuffer[HBC], TXT_HBC_112, hbcversion, hbcIOS);
		else if (hbcversion == VERSION_1_1_0)
			sprintf(ReportBuffer[HBC], TXT_HBC_NEW, hbcIOS);
		else if (hbcversion > 0)
			sprintf(ReportBuffer[HBC], TXT_HBC, hbcversion, hbcIOS);

		if (hbf > 0)
			sprintf(ReportBuffer[HBF], TXT_HBF, hbfversion);

		sprintf(ReportBuffer[HOLLYWOOD], TXT_Hollywood, *HOLLYWOOD_VERSION);
		sprintf(ReportBuffer[CONSOLE_ID], TXT_ConsoleID, deviceID);
		sprintf(ReportBuffer[BOOT2_VERSION], TXT_vBoot2, boot2version);
		sprintf(ReportBuffer[COUNTRY], "Shop Channel Country: %s (%u)", (strlen(country)) ? country : TXT_Unknown, shopcode);
		sprintf(ReportBuffer[NR_OF_TITLES], TXT_NrOfTitles, countTitles);
		sprintf(ReportBuffer[NR_OF_IOS], TXT_NrOfIOS, (countIOS - countBCMIOS), countStubs);


		// Display IOS vulnerabilities
		int lineOffset = 0;
		for (i = 0; i < nbTitles; i++)
		{
			lineOffset = i;
			if (selectedIOS > -1) i = selectedIOS; //If specific IOS is selected

			if (ios[i].titleID == 256) {
				sprintf(ReportBuffer[LAST+lineOffset], "BC v%d", ios[i].revision);
			} else if (ios[i].titleID == 257) {
				sprintf(ReportBuffer[LAST+lineOffset], "MIOS v%d%s", ios[i].revision, miosInfo);
			} else if ((ios[i].titleID==222 || ios[i].titleID==224 || ios[i].titleID==223 || ios[i].titleID==202 || ios[i].titleID==225) && ios[i].baseIOS == 75) {
				sprintf(ReportBuffer[LAST+lineOffset], "IOS%d[38+37] (rev %d, Info: %s):", ios[i].titleID, ios[i].revision, ios[i].info);
			} else {
				if(ios[i].mloadVersion > 0 && ios[i].baseIOS > 0) {
					int v, s;
					v = ios[i].mloadVersion;
					s = v & 0x0F;
					v = v >> 4;
					if (v == 0 && s == 1) {
						v = 4;
						s = 0;
					}
					sprintf(ReportBuffer[LAST+lineOffset], "IOS%d[%d] (rev %d, Info: hermes-v%d.%d):", ios[i].titleID, ios[i].baseIOS, ios[i].revision, v, s);
				} else if(ios[i].baseIOS > 0) {
					sprintf(ReportBuffer[LAST+lineOffset], "IOS%d[%d] (rev %d, Info: %s):", ios[i].titleID, ios[i].baseIOS, ios[i].revision, ios[i].info);
				} else if (strcmp(ios[i].info, "NULL") != 0 && !ios[i].isStub) {
					sprintf(ReportBuffer[LAST+lineOffset], "IOS%d (rev %d, Info: %s):", ios[i].titleID, ios[i].revision, ios[i].info);
				} else if (ios[i].titleID == 249 && ios[i].revision > 11 && ios[i].revision < 18)  {
					sprintf(ReportBuffer[LAST+lineOffset], "IOS%d[38] (rev %d):", ios[i].titleID, ios[i].revision);
				} else {
					sprintf(ReportBuffer[LAST+lineOffset], "IOS%d (rev %d):", ios[i].titleID, ios[i].revision);
				}
			}

			// Check BootMii As IOS (BootMii As IOS is installed on IOS254 rev 31338)
			if (ios[i].titleID == 254 && (ios[i].revision == 31338 || ios[i].revision == 65281))
				strcat (ReportBuffer[LAST+lineOffset]," BootMii");
			else if (ios[i].titleID == 253 && ios[i].revision == 65535)
				strcat (ReportBuffer[LAST+lineOffset]," NANDEmu");
			else
			{
				if (ios[i].isStub && strcmp(ios[i].info, "NULL") == 0) {
					gprintf("1. titleID: %d %s\n", ios[i].titleID, ios[i].info);
					strcat (ReportBuffer[LAST+lineOffset], TXT_Stub);
				} else if (ios[i].isStub && strcmp(ios[i].info, "NULL") != 0) {
					gprintf("2. titleID: %d %s\n", ios[i].titleID, ios[i].info);
					strcat (ReportBuffer[LAST+lineOffset], ios[i].info);
				} else if(ios[i].titleID == 256 || ios[i].titleID == 257) {
					//Do nothing
				} else {
					if(ios[i].infoFakeSignature) strcat(ReportBuffer[LAST+lineOffset], TXT_Trucha);
					if(ios[i].infoESIdentify) strcat(ReportBuffer[LAST+lineOffset], TXT_ES);
					if(ios[i].infoFlashAccess) strcat(ReportBuffer[LAST+lineOffset], TXT_Flash);
					if(ios[i].infoNANDAccess) strcat(ReportBuffer[LAST+lineOffset], TXT_NAND);
					if(ios[i].infoVersionPatch) strcat(ReportBuffer[LAST+lineOffset], TXT_VersionP);
					if(ios[i].infoBoot2Access) strcat(ReportBuffer[LAST+lineOffset], TXT_Boot2);
					if(ios[i].infoUSB2) strcat(ReportBuffer[LAST+lineOffset], TXT_USB);
					if(!ios[i].infoFakeSignature && !ios[i].infoESIdentify && !ios[i].infoFlashAccess && !ios[i].infoNANDAccess && !ios[i].infoUSB2 && !ios[i].infoVersionPatch) strcat(ReportBuffer[LAST+lineOffset], TXT_NoPatch);

					ReportBuffer[LAST+lineOffset][strlen(ReportBuffer[LAST+lineOffset])-1]='\0';
				}
			}
		}

		sprintf(ReportBuffer[17+lineOffset], TXT_ReportDate);
		int NumLines = 17+lineOffset;

	// Mount the SD Card
	printLoading(MSG_MountSD);
	//usleep(250000);
	MountSD();

	// Initialise the FAT file system
	printLoading(MSG_InitFAT);
	//usleep(250000);
	PauseThread();
	if (!fatInitDefault())
	{
		sprintf(MSG_Buffer, ERR_InitFAT);
		printError(MSG_Buffer);
		sleep(2);
		printEndError(MSG_ReportError);
	} else {
		// Create the report
		FILE *file = fopen(REPORT, "w");

		if(!file)
		{
			printError(ERR_OpenFile);
			sleep(2);
			printEndError(MSG_ReportError);
		} else {
			for (i = 0; i <= NumLines; i++) {
				fprintf(file, ReportBuffer[i]);
				fprintf(file, "\r\n");
			}
			// Close the report
			fclose(file);

			printEndSuccess(MSG_ReportSuccess);
		}

		// Create hash log
		file = fopen(HASHLOG, "w");

		if(!file)
		{
			printError(ERR_OpenFile);
			sleep(2);
			printEndError(MSG_ReportError);
		} else {
			for (i = 0; i <= lines; i++) {
				fprintf(file, HashLogBuffer[i]);
				fprintf(file, "\r\n");
			}
			// Close the report
			fclose(file);

			printEndSuccess(MSG_ReportSuccess);
		}
	}

	int LineNr = 0;

	WPAD_Init();
	bool reportIsDisplayed = false;
	while (1) {
		WPAD_ScanPads();
		wpressed = WPAD_ButtonsHeld(0);

		// Return to the loader
		if (wpressed & WPAD_BUTTON_HOME) {
			// Unmount the SD Card
			UnmountSD();
			deinitGUI();
			if(*LOADER_STUB) exit(0);;
			SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
		}

		// Return to System Menu
		if (wpressed & WPAD_BUTTON_PLUS) {
			// Unmount the SD Card
			UnmountSD();
			deinitGUI();
			SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
		}

		// Shutdown Wii
		if (wpressed & WPAD_BUTTON_MINUS) {
			// Unmount the SD Card
			UnmountSD();
			deinitGUI();
			SYS_ResetSystem(SYS_POWEROFF, 0, 0);
		}

		// Display Report
		if (wpressed & WPAD_BUTTON_A) {
			if (reportIsDisplayed && completeReport) {
				transmitSyscheck(ReportBuffer, &NumLines);
				FILE *file = fopen(REPORT, "a");

				if(file)
				{
					fprintf(file, ReportBuffer[NumLines]);
					fprintf(file, "\r\n");
					// Close the report
					fclose(file);
				}
				completeReport = false;
				printReport(ReportBuffer, LineNr, completeReport);
			} else {
				printReport(ReportBuffer, LineNr, completeReport);
				reportIsDisplayed = true;
			}
		}

		if (wpressed & WPAD_BUTTON_UP) {
			if (LineNr > 0) LineNr--;
			printReport(ReportBuffer, LineNr, completeReport);
		}

		if (wpressed & WPAD_BUTTON_DOWN) {
			if (LineNr < NumLines-14) LineNr++;
			printReport(ReportBuffer, LineNr, completeReport);
		}
		if (wpressed & WPAD_BUTTON_LEFT) {
			if (LineNr > 0) LineNr = LineNr - 15;
			if (LineNr < 0) LineNr = 0;
			printReport(ReportBuffer, LineNr, completeReport);
		}

		if (wpressed & WPAD_BUTTON_RIGHT) {
			if (LineNr < NumLines-14) LineNr = LineNr + 15;
			if (LineNr > NumLines) LineNr = NumLines;
			printReport(ReportBuffer, LineNr, completeReport);
		}

	}
}

