/* S/KEY v1.1b (skeysubr.c)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *
 * Modifications: 
 *          Scott Chasin <chasin@crimelab.com>
 *
 * S/KEY misc routines.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "skey.h"


/* Crunch a key:
 * concatenate the seed and the password, run through MD4 and
 * collapse to 64 bits. This is defined as the user's starting key.
 */
int
keycrunch(result,seed,passwd)
char *result;	/* 8-byte result */
char *seed;	/* Seed, any length */
char *passwd;	/* Password, any length */
{
  char *buf;
  MDstruct md;
  unsigned int buflen;
#ifndef IS_LITTLE_ENDIAN
  int i;
  register long tmp;
#endif
  
  buflen = strlen(seed) + strlen(passwd);
  if ((buf = (char *)malloc(buflen+1)) == NULL)
    return -1;
  strcpy(buf,seed);
  strcat(buf,passwd);

  /* Crunch the key through MD4 */
  sevenbit(buf);
  MDbegin(&md);
  MDupdate(&md,(unsigned char *)buf,8*buflen);

  free(buf);

  /* Fold result from 128 to 64 bits */
  md.buffer[0] ^= md.buffer[2];
  md.buffer[1] ^= md.buffer[3];

#ifdef IS_LITTLE_ENDIAN
  /* Only works on byte-addressed little-endian machines!! */
  memcpy(result,(char *)md.buffer,8);
#else
  /* Default (but slow) code that will convert to
   * little-endian byte ordering on any machine
   */
  for (i=0; i<2; i++) {
    tmp = md.buffer[i];
    *result++ = tmp;
    tmp >>= 8;
    *result++ = tmp;
    tmp >>= 8;
    *result++ = tmp;
    tmp >>= 8;
    *result++ = tmp;
  }
#endif

  return 0;
}

/* The one-way function f(). Takes 8 bytes and returns 8 bytes in place */
void f (x)
char *x;
{
  MDstruct md;
#ifndef IS_LITTLE_ENDIAN
  register long tmp;
#endif

  MDbegin(&md);
  MDupdate(&md,(unsigned char *)x,64);

  /* Fold 128 to 64 bits */
  md.buffer[0] ^= md.buffer[2];
  md.buffer[1] ^= md.buffer[3];

#ifdef IS_LITTLE_ENDIAN
  /* Only works on byte-addressed little-endian machines!! */
  memcpy(x,(char *)md.buffer,8);

#else
  /* Default (but slow) code that will convert to
   * little-endian byte ordering on any machine
   */
  tmp = md.buffer[0];
  *x++ = tmp;
  tmp >>= 8;
  *x++ = tmp;
  tmp >>= 8;
  *x++ = tmp;
  tmp >>= 8;
  *x++ = tmp;

  tmp = md.buffer[1];
  *x++ = tmp;
  tmp >>= 8;
  *x++ = tmp;
  tmp >>= 8;
  *x++ = tmp;
  tmp >>= 8;
  *x = tmp;
#endif
}

/* Strip trailing cr/lf from a line of text */
void rip (buf)
char *buf;
{
	char *cp;

	if((cp = strchr(buf,'\r')) != NULL)
		*cp = '\0';

	if((cp = strchr(buf,'\n')) != NULL)
		*cp = '\0';
}

char *readpass (buf,n)
char *buf;
int n;
{
    fgets (buf, n, stdin);
    rip (buf);
    printf ("\n\n");
    sevenbit (buf);
    return buf;
}

/* removebackspaced over charaters from the string */
void backspace(char *buf)
{
	char bs = 0x8;
	char *cp = buf;
	char *out = buf;

	while(*cp){
		if( *cp == bs ) {
			if(out == buf){
				cp++;
				continue;
			}
			else {
			  cp++;
			  out--;
			}
		}
		else {
			*out++ = *cp++;
		}

	}
	*out = '\0';
	
}

/* sevenbit ()
 *
 * Make sure line is all seven bits.
 */
 
void sevenbit (char *s)
{
   while (*s) {
     *s = 0x7f & ( *s);
     s++;
   }
}
