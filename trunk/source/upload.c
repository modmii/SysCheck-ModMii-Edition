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

/* Converts an integer value to its hex character*/
char to_hex(char code) {
  char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str) {
  char *pstr = str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
	if (isalnum((unsigned char) *pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
	  *pbuf++ = *pstr;
	else if (*pstr == ' ')
	  *pbuf++ = '+';
	else
	  *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
	pstr++;
  }
  *pbuf = '\0';
  return buf;
}

void transmitSyscheck(char ReportBuffer[200][100], int *lines) {
	printLoadingBar(TXT_Upload, 0);
	gprintf("TempReport bauen\n");

	int i = 0;
	int strl = 0;
	for (i = 0; i <= *lines; i++) {
		if (i == 9) continue;
		strl += strlen(ReportBuffer[i]);
		strl += strlen("\n");
	}
	printLoadingBar(TXT_Upload, 5);
	char tempReport[strl];
	memset(tempReport, 0, strl);
	for (i = 0; i <= *lines; i++) {
		if (i == 9) continue;
		strcat(tempReport, ReportBuffer[i]);
		strcat(tempReport, "\n");
	}
	printLoadingBar(TXT_Upload, 30);

	net_init();
	printLoadingBar(TXT_Upload, 60);
	gprintf("OK\n");
	char *encodedReport = url_encode(tempReport);
	char bufTransmit[18+strlen(encodedReport)];
	char password[12] = {0};
	gprintf("OK2\n");
	sprintf(bufTransmit, "password=%s&syscheck=%s", password, encodedReport);
	gprintf("bufTransmit: %s ENDE len:%u\n", bufTransmit, strlen(bufTransmit));
	gprintf("OK3\n");
	char host[48] = {"\0"};
	sprintf(host, "http://syscheck.softwii.de/syscheck_receiver.php");
	http_post(host, 1024, bufTransmit);
	printLoadingBar(TXT_Upload, 80);
	gprintf("OK4\n");

	gprintf("\n");

	u32 http_status;
	u8* outbuf;
	u32 lenght;

	http_get_result(&http_status, &outbuf, &lenght);
	printLoadingBar(TXT_Upload, 100);

	(*lines)++;
	memset(ReportBuffer[*lines], 0, 100);
	(*lines)++;
	memcpy(ReportBuffer[*lines], outbuf, lenght);


	free(outbuf);
	gprintf("len: %d, String: %s\n", lenght, ReportBuffer[*lines]);

	u32 wpressed;

	if (!strncmp(ReportBuffer[*lines], "ERROR: ", 7)) {
		char temp[100];
		strncpy(temp, ReportBuffer[*lines]+7, 100);
		printUploadError(temp);
		memset(ReportBuffer[*lines], 0, 100);
		(*lines)--;
		(*lines)--;
		while (1) {
			WPAD_ScanPads();
			wpressed = WPAD_ButtonsHeld(0);

			if (wpressed & WPAD_BUTTON_A) {
				break;
			}
		}
	} else {
		printUploadSuccess(ReportBuffer[*lines]);
		while (1) {
			WPAD_ScanPads();
			wpressed = WPAD_ButtonsHeld(0);

			if (wpressed & WPAD_BUTTON_A) {
				break;
			}
		}
	}

	free(encodedReport);
	net_deinit();
}