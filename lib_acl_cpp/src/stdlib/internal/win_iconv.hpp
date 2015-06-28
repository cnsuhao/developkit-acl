#pragma once

#if defined(WIN32) || defined(WIN64)
#ifdef USE_WIN_ICONV

typedef void* iconv_t;

iconv_t iconv_open(const char *tocode, const char *fromcode);
int iconv_close(iconv_t cd);
size_t iconv(iconv_t cd, const char **inbuf, size_t *inbytesleft,
	char **outbuf, size_t *outbytesleft);

#endif
#endif
