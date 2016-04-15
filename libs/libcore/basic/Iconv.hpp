#ifndef ICONV_HPP
#define ICONV_HPP

#include <QtGlobal>

#define _LIBICONV_VERSION 0x010E    /* version number: (major<<8) + minor */

typedef void* iconv_t;


iconv_t iconv_open (const char* tocode, const char* fromcode);
size_t iconv (iconv_t cd, const char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft);
int iconv_close (iconv_t cd);

/* A type that holds all memory needed by a conversion descriptor.
   A pointer to such an object can be used as an iconv_t. */
typedef struct {
  void* dummy1[28];
} iconv_allocation_t;

/* Allocates descriptor for code conversion from encoding ‘fromcode’ to
   encoding ‘tocode’ into preallocated memory. Returns an error indicator
   (0 or -1 with errno set). */
int iconv_open_into (const char* tocode, const char* fromcode,
							iconv_allocation_t* resultp);

/* Control of attributes. */
int iconvctl (iconv_t cd, int request, void* argument);

/* Hook performed after every successful conversion of a Unicode character. */
typedef void (*iconv_unicode_char_hook) (unsigned int uc, void* data);
/* Hook performed after every successful conversion of a wide character. */
typedef void (*iconv_wide_char_hook) (wchar_t wc, void* data);
/* Set of hooks. */
struct iconv_hooks {
  iconv_unicode_char_hook uc_hook;
  iconv_wide_char_hook wc_hook;
  void* data;
};

/* Fallback function.  Invoked when a small number of bytes could not be
   converted to a Unicode character.  This function should process all
   bytes from inbuf and may produce replacement Unicode characters by calling
   the write_replacement callback repeatedly.  */
typedef void (*iconv_unicode_mb_to_uc_fallback)
			 (const char* inbuf, size_t inbufsize,
			  void (*write_replacement) (const unsigned int *buf, size_t buflen,
										 void* callback_arg),
			  void* callback_arg,
			  void* data);
/* Fallback function.  Invoked when a Unicode character could not be converted
   to the target encoding.  This function should process the character and
   may produce replacement bytes (in the target encoding) by calling the
   write_replacement callback repeatedly.  */
typedef void (*iconv_unicode_uc_to_mb_fallback)
			 (unsigned int code,
			  void (*write_replacement) (const char *buf, size_t buflen,
										 void* callback_arg),
			  void* callback_arg,
			  void* data);

/* If the wchar_t type does not exist, these two fallback functions are never
   invoked.  Their argument list therefore does not matter.  */
typedef void (*iconv_wchar_mb_to_wc_fallback) ();
typedef void (*iconv_wchar_wc_to_mb_fallback) ();


/* Set of fallbacks. */
struct iconv_fallbacks {
  iconv_unicode_mb_to_uc_fallback mb_to_uc_fallback;
  iconv_unicode_uc_to_mb_fallback uc_to_mb_fallback;
  iconv_wchar_mb_to_wc_fallback mb_to_wc_fallback;
  iconv_wchar_wc_to_mb_fallback wc_to_mb_fallback;
  void* data;
};

/* Requests for iconvctl. */
#define ICONV_TRIVIALP            0  /* int *argument */
#define ICONV_GET_TRANSLITERATE   1  /* int *argument */
#define ICONV_SET_TRANSLITERATE   2  /* const int *argument */
#define ICONV_GET_DISCARD_ILSEQ   3  /* int *argument */
#define ICONV_SET_DISCARD_ILSEQ   4  /* const int *argument */
#define ICONV_SET_HOOKS           5  /* const struct iconv_hooks *argument */
#define ICONV_SET_FALLBACKS       6  /* const struct iconv_fallbacks *argument */

/* Listing of locale independent encodings. */
void iconvlist (int (*do_one) (unsigned int namescount,
									  const char * const * names,
									  void* data),
					   void* data);

/* Canonicalize an encoding name.
   The result is either a canonical encoding name, or name itself. */
const char * iconv_canonicalize (const char * name);

/* Support for relocatable packages.  */

/* Sets the original and the current installation prefix of the package.
   Relocation simply replaces a pathname starting with the original prefix
   by the corresponding pathname with the current prefix instead.  Both
   prefixes should be directory names without trailing slash (i.e. use ""
   instead of "/").  */
void libiconv_set_relocation_prefix (const char *orig_prefix,
											const char *curr_prefix);



#endif // ICONV_HPP
