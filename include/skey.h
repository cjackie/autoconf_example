/*
 * S/KEY v1.1b (skey.h)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *
 * Modifications:
 *          Scott Chasin <chasin@crimelab.com>
 *
 * Main client header
 */

#ifndef _SKEY_H_
#define _SKEY_H_

#include "config.h"

/* MDstruct is the data structure for a message digest computation. */
typedef struct {
	unsigned long buffer[4];/* Holds 4-word result of MD computation */
	unsigned char count[8];	/* Number of bits processed so far */
	unsigned int done;	/* Nonzero means MD computation finished */
} MDstruct, *MDptr;

/* MDbegin(MD)
 * Input: MD -- an MDptr
 * Initialize the MDstruct prepatory to doing a message digest computation.
 */
void MDbegin(MDptr MDp);

/* MDupdate(MD,X,count)
 * Input: MD -- an MDptr
 *        X -- a pointer to an array of unsigned characters.
 *        count -- the number of bits of X to use (an unsigned int).
 * Updates MD using the first ``count'' bits of X.
 * The array pointed to by X is not modified.
 * If count is not a multiple of 8, MDupdate uses high bits of last byte.
 * This is the basic input routine for a user.
 * The routine terminates the MD computation when count < 512, so
 * every MD computation should end with one call to MDupdate with a
 * count less than 512.  Zero is OK for a count.
 */
void MDupdate(MDptr MDp,unsigned char *X,unsigned int count);

/* MDprint(MD)
 * Input: MD -- an MDptr
 * Prints message digest buffer MD as 32 hexadecimal digits.
 * Order is from low-order byte of buffer[0] to high-order byte of buffer[3].
 * Each byte is printed with high-order hexadecimal digit first.
 */
void MDprint(MDptr MDp);

/* End of md4.h */


/* Server-side data structure for reading keys file during login */
struct skey
{
  FILE *keyfile;
  char buf[256];
  char *logname;
  int n;
  char *seed;
  char *val;
  long recstart;		/* needed so reread of buffer is efficient */


};

/* Client-side structure for scanning data stream for challenge */
struct mc
{
  char buf[256];
  int skip;
  int cnt;
};

void f(char *x);
int keycrunch(char *result, char *seed, char *passwd);
char *btoe(char *engout, char *c);
char *put8(char *out, char *s);
int etob(char *out, char *e);
void rip(char *buf);
int skeychallenge(struct skey * mp, char *name, char *ss);
int skeylookup(struct skey * mp, char *name);
int skeyverify(struct skey * mp, char *response);

char *readpass (char *buf, int n);
void backspace(char *buf);
void sevenbit (char *s);

#endif	/* _SKEY_H_ */
