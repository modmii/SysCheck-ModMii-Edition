/*  http -- http convenience functions

    Copyright (C) 2008 bushing

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 2.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <ogcsys.h>
#include <network.h>
#include <ogc/lwp_watchdog.h>

#include <sys/types.h>
#include <sys/errno.h>
#include <fcntl.h>

#include "http.h"
#include "languages.h"
#include "gui.h"
#include "gecko.h"
#include "ssl.h"

char *http_host;
u16 http_port;
char *http_path;
u32 http_max_size;

http_res result;
u32 http_status;
u32 content_length;
u8 *http_data;


s32 tcp_socket (void) {
	s32 s, res;

	s = net_socket (PF_INET, SOCK_STREAM, 0);
	if (s < 0) return s;

	if(http_port == 80)
	{
		res = net_fcntl (s, F_GETFL, 0);
		if (res < 0) {
			net_close (s);
			return res;
		}
		//set non-blocking
		res = net_fcntl (s, F_SETFL, res | 4);
		if (res < 0) {
			net_close (s);
			return res;
		}
	}

	return s;
}

s32 tcp_connect (char *host, const u16 port) {
	struct hostent *hp;
	struct sockaddr_in sa;
	s32 s, res;
	s64 t;
	gprintf("tcp_connect\n");
	hp = net_gethostbyname (host);
	if (!hp || !(hp->h_addrtype == PF_INET)) return errno;

	s = tcp_socket ();
	gprintf("tcp_socket = %i\n", s);
	if (s < 0)
		return s;

	memset (&sa, 0, sizeof (struct sockaddr_in));
	sa.sin_family= PF_INET;
	sa.sin_len = sizeof (struct sockaddr_in);
	sa.sin_port= htons (port);
	memcpy ((char *) &sa.sin_addr, hp->h_addr_list[0], hp->h_length);

	t = gettime ();
	while (true) {
		if (ticks_to_millisecs (diff_ticks (t, gettime ())) > TCP_CONNECT_TIMEOUT) {
			net_close(s);
			return -ETIMEDOUT;
		}

		res = net_connect (s, (struct sockaddr *) &sa,
							sizeof (struct sockaddr_in));

		if (res < 0) {
			if (res == -EISCONN)
				break;

			if (res == -EINPROGRESS || res == -EALREADY) {
				usleep (20 * 1000);

				continue;
			}
			net_close(s);

			return res;
		}

		break;
	}

	return s;
}

char * tcp_readln (const s32 s, const u16 max_length, const u64 start_time, const u16 timeout) {
	char *buf;
	u16 c;
	s32 res;
	char *ret;

	buf = (char *) memalign (32, max_length);

	c = 0;
	ret = NULL;
	while (true) {
		if (ticks_to_millisecs (diff_ticks (start_time, gettime ())) > timeout)
			break;

		if(http_port == 443)
			res = ssl_read (s, &buf[c], 1);
		else
			res = net_read (s, &buf[c], 1);

		if ((res == 0) || (res == -EAGAIN)) {
			usleep (20 * 1000);

			continue;
		}

		if (res < 0) break;

		if ((c > 0) && (buf[c - 1] == '\r') && (buf[c] == '\n')) {
			if (c == 1) {
				ret = strdup ("");

				break;
			}

			ret = strndup (buf, c - 1);

			break;
		}

		c++;

		if (c == max_length)
			break;
	}

	free (buf);
	return ret;
}

bool tcp_read (const s32 s, u8 **buffer, const u32 length) {
	u8 *p;
	u32 step, left, block, received;
	s64 t;
	s32 res;

	step = 0;
	p = *buffer;
	left = length;
	received = 0;

	t = gettime ();
	while (left) {
		if (ticks_to_millisecs (diff_ticks (t, gettime ())) > TCP_BLOCK_RECV_TIMEOUT) break;

		block = left;
		if (block > 2048)
			block = 2048;

		if(http_port == 443)
			res = ssl_read (s, p, block);
		else
			res = net_read (s, p, block);

		if ((res == 0) || (res == -EAGAIN)) {
			usleep (20 * 1000);

			continue;
		}

		if (res < 0) break;

		received += res;
		left -= res;
		p += res;
		
		printLoadingBar(MSG_Update, (100.0/(length)*(received)));

		if ((received / TCP_BLOCK_SIZE) > step) {
			t = gettime ();
			step++;
		}
	}

	return left == 0;
}

bool tcp_write (const s32 s, const u8 *buffer, const u32 length) {
	const u8 *p;
	u32 step, left, block, sent;
	s64 t;
	s32 res;

	step = 0;
	p = buffer;
	left = length;
	sent = 0;

	t = gettime ();
	while (left) {
		if (ticks_to_millisecs (diff_ticks (t, gettime ())) >
				TCP_BLOCK_SEND_TIMEOUT) {

			break;
		}

		block = left;
		if (block > 2048)
			block = 2048;

		if(http_port == 443)
			res = ssl_write (s, p, block);
		else
			res = net_write (s, p, block);

		if ((res == 0) || (res == -56)) {
			usleep (20 * 1000);
			continue;
		}

		if (res < 0) {
			break;
		}

		sent += res;
		left -= res;
		p += res;

		if ((sent / TCP_BLOCK_SIZE) > step) {
			t = gettime ();
			step++;
		}
	}

	return left == 0;
}
bool http_split_url (char **host, char **path, const char *url) {
	const char *p;
	char *c;

	if (strncasecmp (url, "http://", 7) == 0)
		p = url + 7;
	else if(strncasecmp (url, "https://", 8) == 0)
		p = url + 8;
	else
		return false;

	c = strchr (p, '/');

	if (c[0] == 0)
		return false;

	*host = strndup (p, c - p);
	*path = strdup (c);

	return true;
}

bool http_request (const char *url, const u32 max_size) {
	int linecount;
	int sslcontext = -1;
	if (!http_split_url(&http_host, &http_path, url)) return false;

	if (strncasecmp (url, "http://", 7) == 0)
		http_port = 80;
	else
		http_port = 443;
	gprintf("http_port = %u\n", http_port);
	http_max_size = max_size;

	http_status = 404;
	content_length = 0;
	http_data = NULL;

	int s = tcp_connect (http_host, http_port);
	gprintf("tcp_connect = %i\n", s);
	if (s < 0) {
		result = HTTPR_ERR_CONNECT;
		return false;
	}
	if(http_port == 443)
	{
		//patched out anyways so just to set something
		sslcontext = ssl_new((u8*)http_host,0);
		if(sslcontext < 0)
		{
			gprintf("ssl_new\n");
			result = HTTPR_ERR_CONNECT;
			net_close (s);
			return false;
		}
		//patched out anyways so just to set something
		ssl_setbuiltinclientcert(sslcontext,0);
		if(ssl_connect(sslcontext,s) < 0)
		{
			gprintf("ssl_connect\n");
			result = HTTPR_ERR_CONNECT;
			ssl_shutdown(sslcontext);
			net_close (s);
			return false;
		}
		int ret = ssl_handshake(sslcontext);
		if(ret < 0)
		{
			gprintf("ssl_handshake %i\n", ret);
			result = HTTPR_ERR_STATUS;
			ssl_shutdown(sslcontext);
			net_close (s);
			return false;
		}
	}
	char *request = (char *) memalign (32, 1024*6);
	char *r = request;
	r += sprintf (r, "GET %s HTTP/1.1\r\n", http_path);
	r += sprintf (r, "Host: %s\r\n", http_host);
	r += sprintf (r, "Cache-Control: no-cache\r\n\r\n");

	bool b = tcp_write (http_port == 443 ? sslcontext : s, (u8 *) request, strlen (request));

	free (request);
	linecount = 0;

	for (linecount=0; linecount < 32; linecount++) {
	  char *line = tcp_readln (http_port == 443 ? sslcontext : s, 0xff, gettime(), (u16)HTTP_TIMEOUT);
		if (!line) {
			http_status = 404;
			result = HTTPR_ERR_REQUEST;
			break;
		}

		if (strlen (line) < 1) {
			free (line);
			line = NULL;
			break;
		}

		sscanf (line, "HTTP/1.%*u %u", &http_status);
		sscanf (line, "Content-Length: %u", &content_length);
		gprintf(line);
		gprintf("\n");
		free (line);
		line = NULL;

	}
	if (linecount == 32 || !content_length) http_status = 404;
	if (http_status != 200) {
		result = HTTPR_ERR_STATUS;
		if(http_port == 443)
			ssl_shutdown(sslcontext);
		net_close (s);
		return false;
	}
	if (content_length > http_max_size) {
		result = HTTPR_ERR_TOOBIG;
		if(http_port == 443)
			ssl_shutdown(sslcontext);
		net_close (s);
		return false;
	}
	http_data = (u8 *) memalign (32, content_length);
	b = tcp_read (http_port == 443 ? sslcontext : s, &http_data, content_length);
	if (!b) {
		free (http_data);
		http_data = NULL;
		result = HTTPR_ERR_RECEIVE;
		if(http_port == 443)
			ssl_shutdown(sslcontext);
		net_close (s);
		return false;
	}

	result = HTTPR_OK;

	if(http_port == 443)
		ssl_shutdown(sslcontext);
	net_close (s);


	return true;
}

bool http_post (const char *url, const u32 max_size, const char *postData) {
	int linecount;
	if (!http_split_url(&http_host, &http_path, url)) return false;

	http_port = 80;
	http_max_size = max_size;
	
	http_status = 404;
	content_length = 0;
	http_data = NULL;

	int s = tcp_connect (http_host, http_port);
	gprintf("tcp_connect = %i\n", s);
	if (s < 0) {
		result = HTTPR_ERR_CONNECT;
		return false;
	}

	char *request = (char *) memalign (32, 1024*6);
	char *r = request;
	r += sprintf (r, "POST %s HTTP/1.1\r\n", http_path);
	r += sprintf (r, "Host: %s\r\n", http_host);
	r += sprintf (r, "Content-type: application/x-www-form-urlencoded\r\n");
	r += sprintf (r, "Content-length: %d\r\n\r\n", strlen(postData));
	r += sprintf (r, "%s", postData);
	
	gprintf("\n request:\n");
	//gprintf(request);
	gprintf("\n :request\n");

	bool b = tcp_write (s, (u8 *) request, strlen (request));

	free (request);
	linecount = 0;

	for (linecount=0; linecount < 32; linecount++) {
	  char *line = tcp_readln (s, 0xff, gettime(), (u16)HTTP_TIMEOUT);
		if (!line) {
			http_status = 404;
			result = HTTPR_ERR_REQUEST;
			break;
		}

		if (strlen (line) < 1) {
			free (line);
			line = NULL;
			break;
		}

		sscanf (line, "HTTP/1.%*u %u", &http_status);
		sscanf (line, "Content-Length: %u", &content_length);
		gprintf(line);
		gprintf("\n");
		free (line);
		line = NULL;

	}
	if (linecount == 32 || !content_length) http_status = 404;
	gprintf("http_status = %u\n", http_status);
	if (http_status != 200) {
		result = HTTPR_ERR_STATUS;
		net_close (s);
		return false;
	}
	if (content_length > http_max_size) {
		result = HTTPR_ERR_TOOBIG;
		net_close (s);
		return false;
	}
	http_data = (u8 *) memalign (32, content_length);
	b = tcp_read (s, &http_data, content_length);
	if (!b) {
		free (http_data);
		http_data = NULL;
		result = HTTPR_ERR_RECEIVE;
		net_close (s);
		return false;
	}

	result = HTTPR_OK;

	net_close (s);


	return true;
}

bool http_get_result (u32 *_http_status, u8 **content, u32 *length) {
	if (http_status) *_http_status = http_status;

	if (result == HTTPR_OK) {
		*content = http_data;
		*length = content_length;

	} else {
		*content = NULL;
		*length = 0;
	}

	free (http_host);
	free (http_path);

	return true;
}

