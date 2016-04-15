#include "Iconv.hpp"


iconv_t iconv_open (const char* tocode, const char* fromcode)
{
	iconv_t t=0;
	return t;
}

size_t iconv (iconv_t cd, const char* * inbuf, size_t *inbytesleft, char* * outbuf, size_t *outbytesleft)
{
	return 0;
}

int iconv_close (iconv_t cd)
{
	return 0;
}


int iconv_open_into (const char* tocode, const char* fromcode, iconv_allocation_t* resultp)
{
	return 0;
}


/* Control of attributes. */
int iconvctl (iconv_t cd, int request, void* argument)
{
	return 0;
}



/* Listing of locale independent encodings. */
void iconvlist (int (*do_one) (unsigned int namescount,
									  const char * const * names,
									  void* data),
					   void* data)
{
	return ;
}


/* Canonicalize an encoding name.
   The result is either a canonical encoding name, or name itself. */
const char * iconv_canonicalize (const char * name)
{
	return 0;
}


/* Support for relocatable packages.  */

/* Sets the original and the current installation prefix of the package.
   Relocation simply replaces a pathname starting with the original prefix
   by the corresponding pathname with the current prefix instead.  Both
   prefixes should be directory names without trailing slash (i.e. use ""
   instead of "/").  */
void libiconv_set_relocation_prefix (const char *orig_prefix,
											const char *curr_prefix)
{
	return ;
}
