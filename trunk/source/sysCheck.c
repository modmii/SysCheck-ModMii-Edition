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
#include <ogc/pad.h>
#include <wiiuse/wpad.h>
#include <stdlib.h>
#include <stdarg.h>
#include <di/di.h>
#include <CheckRegion.h>

#include "runtimeiospatch.h"
#include "SysMenuInfo.h"
#include "fatMounter.h"
#include "gecko.h"
#include "gui.h"
#include "languages.h"
#include "mload.h"
#include "sha1.h"
#include "sys.h"
#include "title.h"
#include "tmdIdentification.h"
#include "tmd_dat.h"
#include "update.h"
#include "wiibasics.h"

// Filename
#define REPORT			"sd:/sysCheck.csv"
#define HASHLOG			"sd:/IOSsyscheck.log"
#define VERSION_1_1_0	65536


extern void __exception_setreload(int t);
static u64 current_time = 0;

// Main
int main(int argc, char **argv)
{
	__exception_setreload(2);
	arguments.AHB_At_Start = AHB_ACCESS;
	memset(arguments.skipIOSlist, 0, sizeof(arguments.skipIOSlist));
	arguments.skipIOScnt = 0;
	arguments.debug = false;
	arguments.USB = strlen(argv[0]) && (argv[0][0] == 'U' || argv[0][0] == 'u');

	InitGecko();
	gprintf(" \n \n==============================================================================\n");
	if(argc>=1){
		int i;
		for(i=0; i<argc; i++){
			if(CHECK_ARG("--debug=true")) {
				arguments.debug = true;
				gprintf("--debug=true\n");
				logfile("--debug=true\r\n");
			} else if(CHECK_ARG("--forceNoAHBPROT=true")) {
				arguments.AHB_At_Start = false;
				gprintf("--forceNoAHBPROT=true\n");
				logfile("--forceNoAHBPROT=true\r\n");
				IOS_ReloadIOS(IOS_GetVersion());
			} else if(CHECK_ARG("--skipIOS=")) {
				arguments.skipIOSlist[arguments.skipIOScnt] = atoi(CHECK_ARG_VAL("--skipIOS="));
				gprintf("skipIOS[%i] = %i\r\n", arguments.skipIOScnt, arguments.skipIOSlist[arguments.skipIOScnt]);
				logfile("skipIOS[%i] = %i\r\n", arguments.skipIOScnt, arguments.skipIOSlist[arguments.skipIOScnt]);
				arguments.skipIOScnt++;
			}
		}
	}
	SysSettings_t SystemInfo;
	if(arguments.AHB_At_Start)
		SystemInfo.deviceType = IS_WII_U;
	else
		SystemInfo.deviceType = CONSOLE_UNKNOWN;
	memset(SystemInfo.miosInfo, 0, sizeof(SystemInfo.miosInfo));

	if (AHB_ACCESS) IosPatch_RUNTIME(true, false, false, true);
	SystemInfo.nandAccess = CheckNANDAccess();

	// Get and display the current date and time
	struct tm today;
	time_t rawtime;
	time(&rawtime);
	today = *localtime(&rawtime);

	initGUI();
	initLanguages(today);

	// Get the console region
	printLoading(MSG_GetConsoleRegion);
	UpdateTime();
	SystemInfo.systemRegion = CONF_GetRegion();
	CheckTime();

	SystemInfo.shopcode = 0;
	if (!CONF_GetShopCode(&SystemInfo.shopcode)) {
		strcpy(SystemInfo.country, CONF_CountryCodes[SystemInfo.shopcode]);
	} else {
		strcpy(SystemInfo.country, TXT_Unknown);
	}

	// Get the system menu version
	printLoading(MSG_GetSysMenuVer);
	UpdateTime();
	SystemInfo.sysMenuVer = GetSysMenuVersion();
	CheckTime();

	sysMenu_t systemmenu;

	printLoading(MSG_GetHBCVer);
	UpdateTime();

	homebrew_t homebrew;
	homebrew.hbcversion = 0;
	homebrew.hbfversion = 0;
	homebrew.hbc = HBC_NONE;
	homebrew.hbf = HBF_NONE;
	homebrew.hbcIOS = 0;
	SystemInfo.dvdSupport = 0;
	s32 ret = Title_GetVersionNObuf(TITLE_ID(0x00010001, HBC_TID_OPEN));
	if (ret<0) {
	ret = Title_GetVersionNObuf(TITLE_ID(0x00010001, HBC_TID_LULZ));
		if (ret<0) {
			ret = Title_GetVersionNObuf(TITLE_ID(0x00010001, HBC_TID_1_0_7));
			if (ret<0) {
				ret = Title_GetVersionNObuf(TITLE_ID(0x00010001, HBC_TID_JODI));
				if (ret<0) {
					homebrew.hbc = HBC_HAXX;
					ret = Title_GetVersionNObuf(TITLE_ID(0x00010001, HBC_TID_HAXX));
					if (ret<0) {
						homebrew.hbc = HBC_NONE;
					} else {
						homebrew.hbc = HBC_HAXX;
						homebrew.hbcversion = ret;
					}
				} else {
					homebrew.hbc = HBC_JODI;
					homebrew.hbcversion = ret;
				}
			} else {
				homebrew.hbc = HBC_1_0_7;
				homebrew.hbcversion = ret;
				if (homebrew.hbcversion == 0)
					homebrew.hbcversion = VERSION_1_1_0;
			}
		} else {
			homebrew.hbc = HBC_LULZ;
			homebrew.hbcversion = (ret != 257) + 1;
		}
	} else {
		homebrew.hbc = HBC_OPEN;
		homebrew.hbcversion = ret;
	}
	if (homebrew.hbc == HBC_OPEN) {
		homebrew.hbcIOS =  get_title_ios(TITLE_ID(0x00010001, HBC_TID_OPEN)); // OPEN
	} else if (homebrew.hbc == HBC_LULZ) {
		homebrew.hbcIOS =  get_title_ios(TITLE_ID(0x00010001, HBC_TID_LULZ)); // LULZ
	} else if (homebrew.hbc == HBC_1_0_7) {
		homebrew.hbcIOS =  get_title_ios(TITLE_ID(0x00010001, HBC_TID_1_0_7)); // ????
	} else if (homebrew.hbc == HBC_JODI) {
		homebrew.hbcIOS =  get_title_ios(TITLE_ID(0x00010001, HBC_TID_JODI)); // JODI
	} else if (homebrew.hbc == HBC_HAXX) {
		homebrew.hbcIOS = get_title_ios(TITLE_ID(0x00010001, HBC_TID_HAXX)); // HAXX
	}

	ret = Title_GetVersionNObuf(TITLE_ID(0x00010001, 0x48424630)); //HBF0
	if (ret<0) {
		ret = Title_GetVersionNObuf(TITLE_ID(0x00010001, 0x54484246)); //THBF
		if (ret<0) {
			homebrew.hbf = HBF_NONE;
		} else {
			homebrew.hbf = HBF_HBF0;
			homebrew.hbfversion = ret;
		}
	} else {
		homebrew.hbf = HBF_THBF;
		homebrew.hbfversion = ret;
	}

	if (AHB_ACCESS) {
		DI_Init();
		DI_DriveID id;

		if(DI_Identify(&id) == 0) {
			SystemInfo.dvdSupport = id.rel_date;
		}
		DI_Close();
	}

	SystemInfo.sysNinVersion = GetSysMenuNintendoVersion(SystemInfo.sysMenuVer);
	SystemInfo.sysMenuRegion = GetSysMenuRegion(SystemInfo.sysMenuVer);
	CheckTime();

	// Get the running IOS version and revision
	printLoading(MSG_GetRunningIOS);
	UpdateTime();
	SystemInfo.runningIOS = IOS_GetVersion();
	SystemInfo.runningIOSRevision = IOS_GetRevision();

	CheckTime();

	// Get the console ID
	printLoading(MSG_GetConsoleID);
	UpdateTime();
	SystemInfo.deviceID = GetDeviceID();
	CheckTime();

	// Get the boot2 version
	printLoading(MSG_GetBoot2);
	UpdateTime();
	SystemInfo.boot2version = GetBoot2Version();
	CheckTime();

	// Get number of titles
	printLoading(MSG_GetNrOfTitles);
	UpdateTime();

	u32 tempTitles;
	if (ES_GetNumTitles(&tempTitles) < 0) {
		printError(ERR_GetNrOfTitles);
		sleep(5);
		deinitGUI();
		exit(1);
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
	CheckTime();

	// Get list of titles
	printLoading(MSG_GetTitleList);
	UpdateTime();
	if (ES_GetTitles(titles, nbTitles) < 0) {
		printError(ERR_GetTitleList);
		sleep(5);
		deinitGUI();
		exit(1);
	}

	int i;
	int j;
	SystemInfo.countIOS = 0; // Number of IOS
	SystemInfo.countStubs = 0; // Number of IOS Stubs
	SystemInfo.countBCMIOS = 0; //Number of BC and MIOS. Should be 2.
	u32 titleID;
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
		titleID = titles[i] & 0xFFFFFFFF;

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
		SystemInfo.countIOS++;
	}

	// Not the most efficient way to remove argument-skipped IOS's, but it works.
	int tempSkipIOScnt = arguments.skipIOScnt;
	while (tempSkipIOScnt > 0) {
		for (i = nbTitles; i--;) {
			titleID = titles[i] & 0xFFFFFFFF;
			if(arguments.skipIOSlist[tempSkipIOScnt - 1] > 0 && titleID == arguments.skipIOSlist[tempSkipIOScnt - 1]) {
				logfile("Skipped IOS %i, titles[%i] = %i\r\n", arguments.skipIOSlist[tempSkipIOScnt - 1], i, titles[i]);
				titles[i] = 0;
				SystemInfo.countIOS--;
				break;
			}
		}
		tempSkipIOScnt--;
	}

	CheckTime();

	// Sort IOS titles
	printLoading(MSG_SortTitles);
	UpdateTime();

	u64 *newTitles = memalign(32, (SystemInfo.countIOS)*sizeof(u64));
	u32 cnt = 0;
	for (i = 0; i < nbTitles; i++) {
		if (titles[i] > 0) {
			newTitles[cnt] = titles[i];
			cnt++;
		}
	}

	sort(newTitles, SystemInfo.countIOS);
	free(titles);

	IOS_t ios[SystemInfo.countIOS];
	// IOS List Initialization
	for (i = SystemInfo.countIOS; i--;) {
		ios[i].infoContent = 0;
		ios[i].titleID = 0;
		ios[i].mloadVersion = 0;
		ios[i].baseIOS = -1;
		strcpy(ios[i].info, "NULL");
		ios[i].isStub = false;
		ios[i].revision = 0;
		ios[i].infoFakeSignature = false;
		ios[i].infoESIdentify = false;
		ios[i].infoFlashAccess = false;
		ios[i].infoNANDAccess = false;
		ios[i].infoBoot2Access = false;
		ios[i].infoUSB2 = false;
		ios[i].infoVersionPatch = false;
		ios[i].infovIOS = false;
	}

	//MountSD();
	NandStartup();

	// Check Priiloader
	SystemInfo.priiloader = checkSysLoader();

	// Check MIOS
	if (SystemInfo.nandAccess) get_miosinfo(SystemInfo.miosInfo);

	// Check running IOS type so we don't have to reload it later
	SystemInfo.runningIOSType = (SystemInfo.deviceType != CONSOLE_WII) && CheckIOSType(SystemInfo.runningIOS, SystemInfo.runningIOSRevision);

	// For each titles found
	for (i = SystemInfo.countIOS; i--;)
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
		ios[i].infoBeerTicket = CheckBeerTicket(ios[i].titleID);

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
		ios[i].titleSize = Title_GetSize_FromTMD(iosTMD);
		ios[i].revision = iosTMD->title_version;
		ios[i].isStub = false;
		ios[i].infoContent = *(u8 *)((u32)iosTMDBuffer+0x1E7);
		ios[i].num_contents = iosTMD->num_contents;
		gprintf("ios%d rev%d iosTMD->num_contents = %d size=%d\n", ios[i].titleID, ios[i].revision, iosTMD->num_contents, ios[i].titleSize);
		logfile("ios%d rev%d iosTMD->num_contents = %d size=%d\r\n", ios[i].titleID, ios[i].revision, iosTMD->num_contents, ios[i].titleSize);

		// Check if this is an IOS stub (according to WiiBrew.org)
		if (IsKnownStub(ios[i].titleID, ios[i].revision))
			ios[i].isStub = true;
		else
		{
			ios[i].isStub = (ios[i].titleID != TID_BC && ios[i].titleID != TID_MIOS && ios[i].titleID != TID_NAND && ios[i].titleID != TID_WFS && ios[i].titleSize < 0x100000);
			if (ios[i].isStub) {
				gprintf("is stub\n");
				logfile("is stub\r\n");
				usleep(100000); // A little delay so you can see what stubs were scanned
			}
		}

		if ((!ios[i].isStub || ios[i].titleID == TID_CBOOT2) && (SystemInfo.nandAccess) && (!getInfoFromContent(&ios[i]))) {
			// Get the TMD hash
			iosTMD->title_id = ((u64)(1) << 32) | 249;
			brute_tmd(iosTMD);

			sha1 hash;
			SHA1((u8 *)iosTMDBuffer, tmdSize, hash);

			sprintf(HashLogBuffer[lines], "IOS%d base hash: \r\n%02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x, %02x %02x %02x %02x\r\n", \
			ios[i].titleID, hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15], hash[16], hash[17], hash[18], hash[19]);
			lines++;

			for (j = 0;j < base_number;j++)
			{
				// Check hashes
				if (memcmp((void*)hash, (u32*)&iosHash[j].hashes, sizeof(sha1)) == 0)
				{
					if (ios[i].titleID != iosHash[j].base)
						ios[i].baseIOS = iosHash[j].base;
					strcpy(ios[i].info, iosHash[j].info);
					gprintf("is %s\n", ios[i].info);
					logfile("is %s\r\n", ios[i].info);
				}
			}
		}
		free(iosTMDBuffer);

		if (ios[i].titleID == TID_BC || ios[i].titleID == TID_MIOS) SystemInfo.countBCMIOS++;

		if (ios[i].isStub && !(iosTMD->title_version == 31338) && !(iosTMD->title_version == 65281) && !(iosTMD->title_version == 65535)) SystemInfo.countStubs++;
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
	sprintf(filepath, "/title/00000001/00000002/content/%08x.app", sysMenuInfoContent);
	gprintf(filepath);
	gprintf("\n");
	ret = read_file_from_nand(filepath, &buffer, &filesize);

	sysInfo = (iosinfo_t *)(buffer);
	if (ret >= 0 && sysInfo != NULL && sysInfo->magicword == 0x1ee7c105 && sysInfo->magicversion == 1)
	{
		systemmenu.realRevision = sysInfo->version;
		systemmenu.hasInfo = true;
		sprintf(systemmenu.info, "%s%s", sysInfo->name, sysInfo->versionstring);
		if (buffer != NULL) free(buffer);
	} else {
		systemmenu.realRevision = 0;
		systemmenu.hasInfo = false;
		strcpy(systemmenu.info, "NONE");
	}

	NandShutdown();
	//UnmountSD();

	SystemInfo.countTitles = nbTitles;
	nbTitles = SystemInfo.countIOS;
	CheckTime();

	// Get the certificates from the NAND
	printLoading(MSG_GetCertificates);
	UpdateTime();
	if (!GetCertificates()) {
		printError(ERR_GetCertificates);
		sleep(5);
		deinitGUI();
		exit(1);
	}
	CheckTime();
	//Select an IOS to test
	WPAD_Init();
	PAD_Init();
	int selectedIOS = -1;
	u32 wpressed;
	time_t starttime = time(NULL);

	printSelectIOS(MSG_SelectIOS, MSG_All);

	bool completeReport = true;

	while (difftime (time(NULL),starttime) < 15) {

		wpressed = DetectInput(DI_BUTTONS_HELD);
		usleep(80000);

		if (wpressed & WPAD_BUTTON_RIGHT && selectedIOS < (nbTitles-1)){
			selectedIOS++;
			starttime = time(NULL);

			titleID = newTitles[selectedIOS] & 0xFFFFFFFF;

			switch (titleID)
			{
				case TID_BC:
					sprintf(MSG_Buffer, "BC");
					break;
				case TID_MIOS:
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
				titleID = newTitles[selectedIOS] & 0xFFFFFFFF;

				switch (titleID)
				{
					case TID_BC:
						sprintf(MSG_Buffer, "BC");
						break;
					case TID_MIOS:
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
			gprintf("updateApp returned %i\n", ret);
			if (ret == 2) {
				printSuccess(MSG_NoUpdate);
				sleep(5);
				starttime = time(NULL);
				if (selectedIOS > -1)
					printSelectIOS(MSG_SelectIOS, MSG_Buffer);
				else
					printSelectIOS(MSG_SelectIOS, MSG_All);
			} else if (ret >= 0) {
				printSuccess(MSG_UpdateSuccess);
				sleep(5);
				deinitGUI();
				exit(0);
			} else if (ret < 0) {
				printError(MSG_UpdateFail);
				sleep(5);
				starttime = time(NULL);
				if (selectedIOS > -1)
					printSelectIOS(MSG_SelectIOS, MSG_Buffer);
				else
					printSelectIOS(MSG_SelectIOS, MSG_All);
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
	for (i = 0; i < nbTitles; i++)
	{
		if (selectedIOS > -1) i = selectedIOS; //If specific IOS is selected

		if (ios[i].titleID == TID_BC) sprintf(MSG_Buffer2, "BC");
		else if (ios[i].titleID == TID_MIOS) sprintf(MSG_Buffer2, "MIOS");
		else sprintf(MSG_Buffer2, "IOS%d", ios[i].titleID);

		sprintf(MSG_Buffer, MSG_TestingIOS, MSG_Buffer2);
		printLoadingBar(MSG_Buffer, (100.0/(nbTitles-1)*(i+1)));

		if (ios[i].isStub ||
			ios[i].titleID == TID_BC ||
			ios[i].titleID == TID_MIOS ||
			ios[i].titleID == TID_NAND ||
			ios[i].titleID == TID_WFS)
		{
			ios[i].infoFakeSignature = false;
			ios[i].infoESIdentify = false;
			ios[i].infoFlashAccess = false;
			ios[i].infoNANDAccess = false;
			ios[i].infoBoot2Access = false;
			ios[i].infoUSB2 = false;
			if (ios[i].titleID == TID_NAND || ios[i].titleID == TID_WFS) ios[i].infovIOS = true;
		}
		else
		{
			// Reload IOS
			gprintf("IOS_ReloadIOS(%d)\n", ios[i].titleID);
			logfile("IOS_ReloadIOS(%d)\r\n", ios[i].titleID);

			if (SystemInfo.deviceType == CONSOLE_WII_U)
				IosPatch_FULL(false, false, false, false, ios[i].titleID);
			else
				IOS_ReloadIOS(ios[i].titleID);

			// Test IOS type
			gprintf("// Test IOS type\n");
			logfile("// Test IOS type\r\n");
			if(SystemInfo.deviceType != CONSOLE_WII) ios[i].infovIOS = CheckIOSType(ios[i].titleID, ios[i].revision);

			// Test fake signature
			gprintf("// Test fake signature\n");
			logfile("// Test fake signature\r\n");
			ios[i].infoFakeSignature = CheckFakeSignature();

			// Test ES Identify
			gprintf("// Test ES Identify\n");
			logfile("// Test ES Identify\r\n");
			ios[i].infoESIdentify = CheckESIdentify();

			// Test Flash Access
			gprintf("// Test Flash Access\n");
			logfile("// Test Flash Access\r\n");
			ios[i].infoFlashAccess = CheckFlashAccess();

			// Test NAND Access
			gprintf("// Test NAND Access\n");
			logfile("// Test NAND Access\r\n");
			ios[i].infoNANDAccess = CheckNANDAccess();

			// Test Boot2 Access
			gprintf("// Test Boot2 Access\n");
			logfile("// Test Boot2 Access\r\n");
			ios[i].infoBoot2Access = CheckBoot2Access();

			// Test USB 2.0
			gprintf("// Test USB 2.0\n");
			logfile("// Test USB 2.0\r\n");
			ios[i].infoUSB2 = CheckUSB2(ios[i].titleID);

			// Set sysMenuIOSVersion and sysMenuIOSType if currently running the System Menu IOS
			if (ios[i].titleID == SystemInfo.sysMenuIOS) {
				SystemInfo.sysMenuIOSVersion = ios[i].revision;
				SystemInfo.sysMenuIOSType = ios[i].infovIOS;
			}

			// Check Priiloader
			if (!SystemInfo.nandAccess && SystemInfo.priiloader == -2 && ios[i].infoNANDAccess) {
				SystemInfo.priiloader = checkSysLoader();
			}

			// Check Base IOS
			if (!SystemInfo.nandAccess && ios[i].infoNANDAccess) {
				NandStartup();
				int k = 0;
				for (k = 0; k < nbTitles; k++) {
					if ((ios[i].isStub || ios[i].titleID == TID_BC || ios[i].titleID == TID_MIOS) && ios[i].titleID != TID_CBOOT2) continue;
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

	//--Generate Report--
	UpdateTime();
	printLoading(MSG_GenerateReport);


	char ReportBuffer[200][100] = {{0}}; // The maximum display length is actually 73

	if (SystemInfo.dvdSupport > 0)
		formatDate(SystemInfo.dvdSupport, ReportBuffer);
	else
		sprintf(ReportBuffer[DVD], TXT_NoDVD);

	// Display Title
	sprintf(ReportBuffer[APP_TITLE], TXT_AppTitle, TXT_AppVersion);
	sprintf(ReportBuffer[APP_IOS], TXT_AppIOS, SystemInfo.runningIOSType ? "v" : "", SystemInfo.runningIOS, SystemInfo.runningIOSRevision);
	SystemInfo.validregion = SystemInfo.systemRegion >= CONF_REGION_JP && SystemInfo.systemRegion <= CONF_REGION_CN;

	// Display the console region
	if (SystemInfo.sysNinVersion != 0.0f) {
		sprintf(ReportBuffer[TEXT_REGION], "%s: %s", TXT_Region, SystemInfo.validregion ? Regions[SystemInfo.systemRegion] : "");
		if (SystemInfo.validregion)
			sprintf(ReportBuffer[SYSMENU], TXT_SysMenu, SystemInfo.sysNinVersion, SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
		else
			strcat(ReportBuffer[SYSMENU], TXT_Unknown);

	} else if (systemmenu.hasInfo) {
		SystemInfo.sysNinVersion = GetSysMenuNintendoVersion(systemmenu.realRevision);
		SystemInfo.sysMenuRegion = GetSysMenuRegion(SystemInfo.sysMenuVer);
		sprintf(ReportBuffer[TEXT_REGION], "%s: %s", TXT_Region, SystemInfo.validregion ? Regions[SystemInfo.systemRegion] : "");
		if (SystemInfo.validregion)
			sprintf(ReportBuffer[SYSMENU], TXT_SysMenu3, SystemInfo.sysNinVersion, SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer, systemmenu.realRevision, systemmenu.info);
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
			SystemInfo.sysNinVersion = 4.1f;
			sprintf(ReportBuffer[TEXT_REGION], "%s: PAL", TXT_Region);
			sprintf(ReportBuffer[SYSMENU], TXT_SysMenu, SystemInfo.sysNinVersion, "E", SystemInfo.sysMenuVer);
		} else {
			SystemInfo.sysMenuIOS = get_title_ios(TITLE_ID(0x00000001, 0x00000002));

			switch (SystemInfo.systemRegion)
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

			switch (SystemInfo.sysMenuIOS)
			{
				case 9:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "1.0", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
				case 11:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "2.0/2.1", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
				case 20:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "2.2", SystemInfo.sysMenuRegion);
					break;
				case 30:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "3.0/3.1/3.2/3.3", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
				case 40:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "3.3", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
				case 50:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "3.4", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
				case 60:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "4.0/4.1", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
				case 70:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "4.2", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
				case 80:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "4.3", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
				default:
					sprintf(ReportBuffer[SYSMENU], TXT_SysMenu2, "0.0", SystemInfo.sysMenuRegion, SystemInfo.sysMenuVer);
					break;
			}
		}
	}

	switch (CONF_GetSerialCode(NULL)) {
		case CONF_CODE_JPN:
		case CONF_CODE_VJPNI:
		case CONF_CODE_VJPNO:
			// JAP
			if (SystemInfo.systemRegion != CONF_REGION_JP) {
				strcat(ReportBuffer[TEXT_REGION], TXT_OriginalRegion);
				strcat(ReportBuffer[TEXT_REGION], "JAP)");
			}
			break;
		case CONF_CODE_USA:
		case CONF_CODE_USAK:
		case CONF_CODE_VUSAI:
		case CONF_CODE_VUSAO:
			// USA
			if (SystemInfo.systemRegion != CONF_REGION_US) {
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
			if (SystemInfo.systemRegion != CONF_REGION_EU) {
				strcat(ReportBuffer[TEXT_REGION], TXT_OriginalRegion);
				strcat(ReportBuffer[TEXT_REGION], "PAL)");
			}
			break;
		case CONF_CODE_KOR:
			// KOR
			if (SystemInfo.systemRegion != CONF_REGION_KR) {
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

	if (SystemInfo.priiloader == 1)
		sprintf(ReportBuffer[PRIILOADER], TXT_Priiloader);
	else if (SystemInfo.priiloader == 2)
		sprintf(ReportBuffer[PRIILOADER], TXT_PreFiix);

	if (homebrew.hbc == HBC_NONE || homebrew.hbcversion == 0)
		sprintf(ReportBuffer[HBC], TXT_NO_HBC);
	else if (homebrew.hbcIOS == 0)
		sprintf(ReportBuffer[HBC], TXT_HBC_STUB);
	else if (homebrew.hbc == HBC_LULZ)
		sprintf(ReportBuffer[HBC], TXT_HBC_112, homebrew.hbcversion, homebrew.hbcIOS);
	else if (homebrew.hbcversion == VERSION_1_1_0)
		sprintf(ReportBuffer[HBC], TXT_HBC_NEW, homebrew.hbcIOS);
	else if (homebrew.hbcversion > 0)
		sprintf(ReportBuffer[HBC], TXT_HBC, homebrew.hbcversion, homebrew.hbcIOS);

	if (homebrew.hbf > HBF_NONE)
		sprintf(ReportBuffer[HBF], TXT_HBF, homebrew.hbfversion);

	// If console type is unknown (because no AHB access), try to determine it by the IOS80 version.  Less reliable
	if ((SystemInfo.deviceType == CONSOLE_UNKNOWN) && (SystemInfo.sysMenuIOSType == IOS_WII_U) && (SystemInfo.sysMenuIOS == 80) && (SystemInfo.sysMenuIOSVersion == 7200))
		SystemInfo.deviceType = CONSOLE_WII_U;
	
	const char *device_types[] = {"Wii", "vWii", TXT_Unknown};
	
	sprintf(ReportBuffer[HOLLYWOOD], TXT_Hollywood, HOLLYWOOD_VERSION);
	sprintf(ReportBuffer[CONSOLE_ID], TXT_ConsoleID, SystemInfo.deviceID);
	sprintf(ReportBuffer[CONSOLE_TYPE], TXT_ConsoleType, device_types[SystemInfo.deviceType]);
	sprintf(ReportBuffer[COUNTRY], TXT_ShopCountry, (strlen(SystemInfo.country)) ? SystemInfo.country : TXT_Unknown, SystemInfo.shopcode);
	sprintf(ReportBuffer[BOOT2_VERSION], TXT_vBoot2, SystemInfo.boot2version);
	sprintf(ReportBuffer[NR_OF_TITLES], TXT_NrOfTitles, SystemInfo.countTitles);
	sprintf(ReportBuffer[NR_OF_IOS], TXT_NrOfIOS, (SystemInfo.countIOS - SystemInfo.countBCMIOS), SystemInfo.countStubs);


	// Display IOS vulnerabilities
	int lineOffset = 0;
	int skippedOffset = 0;
	int lastIOS = 0;
	for (i = 0; i < nbTitles; i++)
	{
		lineOffset = i + LAST;
		if (arguments.skipIOScnt > 0) {
			for(j = 0; j < arguments.skipIOScnt; j++) {
				if (arguments.skipIOSlist[j] > lastIOS && arguments.skipIOSlist[j] < ios[i].titleID) {
					snprintf(ReportBuffer[skippedOffset + lineOffset], MAX_ELEMENTS(ReportBuffer[0]), "%sIOS%d: %s", (SystemInfo.deviceType == CONSOLE_WII_U) ? "v" : "", arguments.skipIOSlist[j], TXT_IOSSkipped);
					skippedOffset++;
					break;
				}
			}
		}
		
		if (selectedIOS > -1) i = selectedIOS; //If specific IOS is selected

		if (ios[i].titleID == TID_BC) {
			sprintf(ReportBuffer[skippedOffset + lineOffset], "BC v%d", ios[i].revision);
		} else if (ios[i].titleID == TID_MIOS) {
			sprintf(ReportBuffer[skippedOffset + lineOffset], "MIOS v%d%s", ios[i].revision, SystemInfo.miosInfo);
		} else if (ios[i].baseIOS == 75 && (ios[i].titleID==222 || ios[i].titleID==224 || ios[i].titleID==223 || ios[i].titleID==202 || ios[i].titleID==225)) {
			sprintf(ReportBuffer[skippedOffset + lineOffset], "%sIOS%d[38+37] (rev %d, Info: %s):", ios[i].infovIOS ? "v" : "", ios[i].titleID, ios[i].revision, ios[i].info);
		} else if (ios[i].baseIOS == 98 && (ios[i].titleID==222 || ios[i].titleID==224 || ios[i].titleID==223 || ios[i].titleID==202 || ios[i].titleID==225)) {
			sprintf(ReportBuffer[skippedOffset + lineOffset], "%sIOS%d[38+60] (rev %d, Info: %s):", ios[i].infovIOS ? "v" : "", ios[i].titleID, ios[i].revision, ios[i].info);
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
				sprintf(ReportBuffer[skippedOffset + lineOffset], "%sIOS%d[%d] (rev %d, Info: hermes-v%d.%d):", ios[i].infovIOS ? "v" : "", ios[i].titleID, ios[i].baseIOS, ios[i].revision, v, s);
			} else if(ios[i].baseIOS > 0) {
				snprintf(ReportBuffer[skippedOffset + lineOffset], MAX_ELEMENTS(ReportBuffer[0]), "%sIOS%d[%d] (rev %d, Info: %s):", ios[i].infovIOS ? "v" : "", ios[i].titleID, ios[i].baseIOS, ios[i].revision, ios[i].info);
			} else if (strcmp(ios[i].info, "NULL") != 0 && !ios[i].isStub) {
				snprintf(ReportBuffer[skippedOffset + lineOffset], MAX_ELEMENTS(ReportBuffer[0]), "%sIOS%d (rev %d, Info: %s):", ios[i].infovIOS ? "v" : "", ios[i].titleID, ios[i].revision, ios[i].info);
			} else if (ios[i].titleID == 249 && ios[i].revision > 11 && ios[i].revision < 18)  {
				sprintf(ReportBuffer[skippedOffset + lineOffset], "%sIOS%d[38] (rev %d):", ios[i].infovIOS ? "v" : "", ios[i].titleID, ios[i].revision);
			} else {
				snprintf(ReportBuffer[skippedOffset + lineOffset], MAX_ELEMENTS(ReportBuffer[0]), "%sIOS%d (rev %d):", ios[i].infovIOS ? "v" : "", ios[i].titleID, ios[i].revision);
			}
		}

		// Check BootMii As IOS (BootMii As IOS is installed on IOS254 rev 31338)
		if (ios[i].titleID == TID_BOOTMII && (ios[i].revision == 31338 || ios[i].revision == 65281))
			strcat (ReportBuffer[skippedOffset + lineOffset]," BootMii");
		else if (ios[i].titleID == TID_NANDEMU && ios[i].revision == 65535)
			strcat (ReportBuffer[skippedOffset + lineOffset]," NANDEmu");
		else
		{
			if (ios[i].isStub && strcmp(ios[i].info, "NULL") == 0) {
				gprintf("1. titleID: %d %s\n", ios[i].titleID, ios[i].info);
				strcat (ReportBuffer[skippedOffset + lineOffset], TXT_Stub);
			} else if (ios[i].isStub && strcmp(ios[i].info, "NULL") != 0) {
				gprintf("2. titleID: %d %s\n", ios[i].titleID, ios[i].info);
				strcat (ReportBuffer[skippedOffset + lineOffset], ios[i].info);
			} else if(ios[i].titleID != TID_BC && ios[i].titleID != TID_MIOS) {
				if(ios[i].infoFakeSignature) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_Trucha);
				if(ios[i].infoESIdentify) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_ES);
				if(ios[i].infoFlashAccess) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_Flash);
				if(ios[i].infoNANDAccess) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_NAND);
				if(ios[i].infoVersionPatch) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_VersionP);
				if(ios[i].infoBoot2Access) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_Boot2);
				if(ios[i].infoUSB2) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_USB);
				if(ios[i].infoBeerTicket) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_BeerTicket);
				if(!ios[i].infoFakeSignature && !ios[i].infoESIdentify && !ios[i].infoFlashAccess && !ios[i].infoNANDAccess && !ios[i].infoUSB2 && !ios[i].infoVersionPatch && !ios[i].infoBeerTicket) strcat(ReportBuffer[skippedOffset + lineOffset], TXT_NoPatch);

				ReportBuffer[skippedOffset + lineOffset][strlen(ReportBuffer[skippedOffset + lineOffset])-1]='\0';
			}
		}
		lastIOS = ios[i].titleID;
	}

	int NumLines = lineOffset + skippedOffset + 1;
	sprintf(ReportBuffer[NumLines], TXT_ReportDate);
	CheckTime();

	// Mount the SD Card
	UpdateTime();
	printLoading(MSG_MountSD);
	//if(arguments.USB)
	//	MountUSB();
	//else
		MountSD();
	CheckTime();

	// Initialise the FAT file system
	UpdateTime();
	printLoading(MSG_InitFAT);
	if (!fatInitDefault())
	{
		sprintf(MSG_Buffer, ERR_InitFAT);
		printError(MSG_Buffer);
		sleep(2);
		printEndError(MSG_ReportError);
	} else {
		// Create the report
		CheckTime();
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
				fflush(file);
			}
			// Close the report
			fclose(file);

			printEndSuccess(MSG_ReportSuccess);
			CheckTime();
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
				fflush(file);
			}
			// Close the report
			fclose(file);
			UpdateTime();
			printEndSuccess(MSG_ReportSuccess);
			CheckTime();
		}
	}

	int LineNr = 0;

	WPAD_Init();
	bool reportIsDisplayed = false;
	while (1) {
		wpressed = DetectInput(DI_BUTTONS_HELD);

		// Return to the loader
		if (wpressed & WPAD_BUTTON_HOME) {
			// Unmount the SD Card
			UnmountSD();
			//UnmountUSB();
			deinitGUI();
			exit(0);
		}

		// Return to System Menu
		if (wpressed & WPAD_BUTTON_PLUS) {
			// Unmount the SD Card
			UnmountSD();
			//UnmountUSB();
			deinitGUI();
			SYS_ResetSystem(SYS_RETURNTOMENU, 0, 0);
		}

		// Shutdown Wii
		if (wpressed & WPAD_BUTTON_MINUS) {
			// Unmount the SD Card
			UnmountSD();
			//UnmountUSB();
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
				usleep(500000); // A little pause to decrease the chance of accidental upload
				reportIsDisplayed = true;
			}
		}
		if (NumLines > 14) { // Just a safety measure in case the report is less than 14 lines for some reason
			if (wpressed & WPAD_BUTTON_UP) {
				if (LineNr > 0) LineNr--;
				printReport(ReportBuffer, LineNr, completeReport);
				usleep(50000);
			}

			if (wpressed & WPAD_BUTTON_DOWN) {
				if (LineNr < NumLines-14) LineNr++;
				printReport(ReportBuffer, LineNr, completeReport);
				usleep(50000);
			}
			if (wpressed & WPAD_BUTTON_LEFT) {
				if (LineNr > 0) LineNr = LineNr - 15;
				if (LineNr < 0) LineNr = 0;
				printReport(ReportBuffer, LineNr, completeReport);
				usleep(100000);
			}

			if (wpressed & WPAD_BUTTON_RIGHT) {
				if (LineNr < NumLines-14) LineNr = LineNr + 15;
				if (LineNr + 14 > NumLines) LineNr = NumLines-14;
				printReport(ReportBuffer, LineNr, completeReport);
				usleep(100000);
			}
		}
	}
}
