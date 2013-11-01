#ifndef _TOOLS_H_
#define _TOOLS_H_

#define HAVE_AHBPROT ((*(vu32*)0xcd800064 == 0xFFFFFFFF) ? 1 : 0)
#define MEM_REG_BASE 0xd8b4000
#define MEM_PROT (MEM_REG_BASE + 0x20a)
#define HOLLYWOOD_VERSION (vu32*)0x80003138
#define LOADER_STUB (vu32*)0x80001800

// Turn upper and lower into a full title ID
#define TITLE_ID(x,y)           (((u64)(x) << 32) | (y))
// Get upper or lower half of a title ID
#define TITLE_UPPER(x)          ((u32)((x) >> 32))
// Turn upper and lower into a full title ID
#define TITLE_LOWER(x)          ((u32)(x))

#define FULL_TITLE_ID(titleId) ((u32)(titleId))
#define TITLE_ID2(titleId) ((u32)((titleId) >> 32))

enum {
	APP_TITLE = 0,
	APP_IOS,
	BLANK_LINE,
	TEXT_REGION,
	SYSMENU,
	PRIILOADER,
	DVD,
	HBC,
	HBF,
	HOLLYWOOD,
	CONSOLE_ID,
	COUNTRY,
	BOOT2_VERSION,
	NR_OF_TITLES,
	NR_OF_IOS,
	BLANK,
	LAST
};

typedef struct {
	s32   	revision;
	bool 	isStub;
	bool 	infoFakeSignature;
	bool 	infoESIdentify;
	bool 	infoFlashAccess;
	bool 	infoNANDAccess;
	bool 	infoBoot2Access;
	bool 	infoUSB2;
	bool 	infoVersionPatch;
	s32		baseIOS;
	s32		mloadVersion;
	char	info[0x10];
	u8		infoContent;
	u32		titleID;
	u8		num_contents;
} IOS;

typedef struct {
	bool 	hasInfo;
	u32   	realRevision;
	char	info[0x10];
} sysMenu;

typedef struct _U8Header
{
	u32 fcc;
	u32 rootNodeOffset;
	u32 headerSize;
	u32 dataOffset;
	u8 zeroes[16];
} __attribute__((packed)) U8Header;

typedef struct _U8Entry
{
	struct
	{
		u32 fileType :8;
		u32 nameOffset :24;
	};
	u32 fileOffset;
	union
	{
		u32 fileLength;
		u32 numEntries;
	};
} __attribute__( ( packed ) ) U8Entry;

extern const char *Regions[];
extern u8 sysMenuInfoContent;
extern bool debug;

#ifdef __cplusplus
extern "C"
{
#endif

// Prototypes
char GetSysMenuRegion(u32 sysVersion);
bool GetCertificates(void);
u32 GetSysMenuVersion(void);
float GetSysMenuNintendoVersion(u32 sysVersion);
u32 GetBoot2Version(void);
u32 GetDeviceID(void);
bool CheckFakeSignature(void);
bool CheckESIdentify(void);
bool CheckFlashAccess(void);
bool CheckNANDAccess(void);
bool CheckBoot2Access(void);
bool CheckMload(void);
bool CheckUSB2(u32);
bool IsKnownStub(u32, s32);
s32 GetTMD(u64 TicketID, signed_blob **Output, u32 *Length);
char GetBootFilename(u64 titleId);
s32 read_file_from_nand(char *filepath, u8 **buffer, u32 *filesize);
int NandStartup(void);
void NandShutdown(void);
int checkSysLoader(void);
void transmitSyscheck(char ReportBuffer[200][100], int *lines);
u32 IOSPATCH_Apply(void);
s32 brute_tmd(tmd *p_tmd);
bool checkISFSinRAM(void);
u32 es_set_ahbprot(void);
void *allocate_memory(u32 size);
void logfile(const char *format, ...);
s32 get_miosinfo(char *str);

#ifdef __cplusplus
}
#endif

#endif