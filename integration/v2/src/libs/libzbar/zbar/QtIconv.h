#ifndef QTICONV_HPP
#define QTICONV_HPP



/* Identifier for conversion method from one codeset to another.  */
typedef void *iconv_t;


/* Allocate descriptor for code conversion from codeset FROMCODE to
   codeset TOCODE.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern iconv_t iconv_open (const char *__tocode, const char *__fromcode);

/* Convert at most *INBYTESLEFT bytes from *INBUF according to the
   code conversion algorithm specified by CD and place up to
   *OUTBYTESLEFT bytes in buffer at *OUTBUF.  */
extern size_t iconv (iconv_t __cd, char **__restrict __inbuf,
					 size_t *__restrict __inbytesleft,
					 char **__restrict __outbuf,
					 size_t *__restrict __outbytesleft);

/* Free resources allocated for descriptor CD for code conversion.

   This function is a possible cancellation point and therefore not
   marked with __THROW.  */
extern int iconv_close (iconv_t __cd);



#endif // QTICONV_HPP
