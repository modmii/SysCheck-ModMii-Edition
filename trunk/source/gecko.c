#include <gccore.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* init-globals */
bool geckoinit = false;
bool textVideoInit = false;

#ifndef NO_DEBUG

//using the gprintf from crediar because it is smaller than mine
void gprintf( const char *str, ... )
{
	if (!(geckoinit))return;
	char astr[1024];

	va_list ap;
	va_start(ap,str);

	vsnprintf(astr, sizeof(astr), str, ap);
	usb_sendbuffer_safe(1, astr, strlen(astr));

	va_end(ap);
	return;
	
}

void gsenddata(const u8 *data, int length, const char *filename)
{
	if (!(geckoinit))return;
	
	// First, send a "\x1b[2B]" line (this will tell geckoreader that binary data is comming up next)
	const char *binary_data = "\x1b[2B]\n";
	
	usb_sendbuffer_safe(1, binary_data, strlen(binary_data));
	
	u8 filenamelength = filename == NULL ? 0 : strlen(filename);
	
	// Send the length
	usb_sendbuffer_safe(1, (u8 *) &length, 4);
	usb_sendbuffer_safe(1, (u8 *) &filenamelength, 1);
	usb_sendbuffer_safe(1, data, length);
	if (filename != NULL)
	{
		usb_sendbuffer_safe(1, filename, strlen(filename));
	}
}

char ascii(char s) {
  if(s < 0x20) return '.';
  if(s > 0x7E) return '.';
  return s;
}

bool InitGecko()
{
	if(usb_isgeckoalive(EXI_CHANNEL_1))
	{
		usb_flush(EXI_CHANNEL_1);
		geckoinit = true;
	} else {
		geckoinit = false;
	}
	return geckoinit;
}

#endif /* NO_DEBUG */
