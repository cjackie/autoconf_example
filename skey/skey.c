/*
 * S/KEY v1.1b (skey.c)
 *
 * Authors:
 *          Neil M. Haller <nmh@thumper.bellcore.com>
 *          Philip R. Karn <karn@chicago.qualcomm.com>
 *          John S. Walden <jsw@thumper.bellcore.com>
 *          Scott Chasin <chasin@crimelab.com>
 *
 *
 * Stand-alone program for computing responses to S/Key challenges.
 * Takes the iteration count and seed as command line args, prompts
 * for the user's key, and produces both word and hex format responses.
 *
 * Usage example:
 *	>skey 88 ka9q2
 *	Enter password:
 *	OMEN US HORN OMIT BACK AHOY
 *	>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include "skey.h"

static int d_flag;
static FILE *logfile;

void usage (char *s);
int __main (int argc, char *argv[]);

/* wrappers around functions to enable debug */
void _usage(char *s);
int _main (int argc, char *argv[]);
int _getopt(int argc, char * const argv[], const char *optstring);
int _atoi(const char *nptr);
char *_strcpy(char *dest, const char *src);
char *_strchr(const char *s, int c);
char *_readpass (char *buf, int n);
void _rip (char *buf);
int _keycrunch(char *result, char *seed, char *passwd);
char *_btoe(char *engout, char *c);

/* helper functions */


int main (int argc, char *argv[]) {
  int i, l_file_size = 256;
  char l_file[l_file_size];
  int h_flag = 0, v_flag = 0, l_flag = 0;

  logfile = stderr;
  
  while ((i = getopt (argc, argv, "p:n:hvdl:")) != EOF)
  {
    switch (i)
    {
    case 'h':
      h_flag = 1;
      break;
    case 'v':
      v_flag = 1;
      break;
    case 'd':
      d_flag += 1;
      break;
    case 'l':
      l_flag = 1;
      strncpy(l_file, optarg, l_file_size);
      break;
    case 'p':
    case 'n':
      break;
    }
  }

  if (l_flag) {
    logfile = fopen(l_file, "a");
    if (logfile == NULL) {
      fprintf(stderr,  "fail to open the log file: %s\n", l_file);
      exit(1);
    }
  }

  if (h_flag) {
    _usage("[-v] [-d] [-h] [-l]");
    exit(0);
  }

  if (v_flag) {
    fprintf(stderr, "version number: 0.0.1\n");
  }

  _main(argc, argv);

}


int __main (int argc, char *argv[])
{
  int n, cnt, i, pass = 0;
  char passwd[256], key[8], buf[33], *seed, *slash;

  cnt = 1;
  /* reset it */
  optind = 1;
  while ((i = _getopt (argc, argv, "n:p:")) != EOF)
  {
    switch (i)
    {
    case 'n':
      cnt = _atoi (optarg);
      break;
    case 'p':
      _strcpy (passwd, optarg);
      pass = 1;
      break;
    }
  }

  /* could be in the form <number>/<seed> */

  if (argc <= optind + 1)
  {
    /* look for / in it */
    if (argc <= optind)
    {
      _usage (argv[0]);
      exit (1); 
    }

    slash = _strchr (argv[optind], '/');
    if (slash == NULL)
    {
      _usage (argv[0]);
      exit (1);
    }
    *slash++ = '\0';
    seed = slash;

    if ((n = _atoi (argv[optind])) < 0)
    {
      printf ("%s not positive\n", argv[optind]);
      _usage (argv[0]);
      exit (1);
    }
  }
  else
  {

    if ((n = _atoi (argv[optind])) < 0)
    {
      printf ("%s not positive\n", argv[optind]);
      _usage (argv[0]);
      exit (1);
    }
    seed = argv[++optind];
  }

  /* Get user's secret password */
  if (!pass)
  {
    printf ("Enter secret password: ");
    _readpass (passwd, sizeof (passwd));
  }

  _rip (passwd);

  /* Crunch seed and password into starting key */
  if (_keycrunch (key, seed, passwd) != 0)
  {
    fprintf (stderr, "%s: key crunch failed\n", argv[0]);
    exit (1);
  }
  if (cnt == 1)
  {
    while (n-- != 0)
      f (key);
    printf ("%s\n", _btoe (buf, key));
#ifdef	HEXIN
    printf ("%s\n", put8 (buf, key));
#endif
   }
  else
  {
    for (i = 0; i <= n - cnt; i++)
      f (key);
    for (; i <= n; i++)
    {
#ifdef	HEXIN
      printf ("%d: %-29s  %s\n", i, _btoe (buf, key), put8 (buf, key));
#else
      printf ("%d: %-29s\n", i, _btoe (buf, key));
#endif
      f (key);
    }
  }
  exit (0);
}


void usage (char *s) 
{

  printf ("Usage: %s [-n count] [-p password ] <sequence #>[/] <key> \n", s);

}


/* wrapper functions */

int _main (int argc, char *argv[]) {
  int ret;
  int i;

  if (d_flag >= 1)
    fprintf(logfile, "Entering function main.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "argc => %d\n", argc);
    fprintf(logfile, "argv => ");
    for (i = 0; argv[i] != NULL; i++) 
      fprintf(logfile, "%s,", argv[i]);
    fprintf(logfile, "\n");
  }

  ret = __main(argc, argv);

  if (d_flag >= 2)
    fprintf(logfile, "Return is %d.\n", ret);
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function main.\n");
  if (d_flag) 
    fprintf(logfile, "\n");

  return ret;

}

void _usage (char *s) {
  if (d_flag >= 1)
    fprintf(logfile, "Entering function usage.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "s => %s\n", s);
  }

  usage(s);

  if (d_flag >= 2)
    fprintf(logfile, "Return type is void.\n");
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function usage.\n");
  if (d_flag) 
    fprintf(logfile, "\n");
}

int _getopt(int argc, char * const argv[], const char *optstring) {
  int ret, i;
 
  if (d_flag >= 1)
    fprintf(logfile, "Entering function getopt.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "argc => %d\n", argc);
    fprintf(logfile, "argv => ");
    for (i = 0; argv[i] != NULL; i++)
      fprintf(logfile, "%s,", argv[i]);
    fprintf(logfile, "\n");
    fprintf(logfile, "optstring => %s\n", optstring);
  }

  ret = getopt(argc, argv, optstring);

  if (d_flag >= 2)
    fprintf(logfile, "Return is %d.\n", ret);
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function getopt.\n");
  if (d_flag)
    fprintf(logfile, "\n");

  return ret;
}

int _atoi(const char *nptr) {
  int ret;

  if (d_flag >= 1)
    fprintf(logfile, "Entering function atoi.");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "nptr => %s\n", nptr);
  }

  ret = atoi(nptr);

  if (d_flag >= 2)
    fprintf(logfile, "Return is %d.\n", ret);
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function atoi.\n");
  if (d_flag) 
    fprintf(logfile, "\n");

  return ret;
}

char *_strcpy(char *dest, const char *src) {
  char *ret;

  if (d_flag >= 1)
    fprintf(logfile, "Entering function strcpy.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "dest => %s\n", dest);
    fprintf(logfile, "src => %s\n", src);
  }

  ret = strcpy(dest, src);

  if (d_flag >= 2)
    fprintf(logfile, "Return is %s.\n", ret);
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function strcpy.\n");
  if (d_flag) 
    fprintf(logfile, "\n");

  return ret;
}

char *_strchr(const char *s, int c) {
  char *ret;

  if (d_flag >= 1)
    fprintf(logfile, "Entering function strchr.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "s => %s\n", s);
    fprintf(logfile, "c => %d\n", c);
  }

  ret = strchr(s, c);

  if (d_flag >= 2)
    fprintf(logfile, "Return is %s.\n", ret);
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function strchr.\n");
  if (d_flag) 
    fprintf(logfile, "\n");

  return ret;
}

char *_readpass (char *buf, int n) {
  char *ret;

  if (d_flag >= 1)
    fprintf(logfile, "Entering function readpass.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "buf => %s\n", buf);
    fprintf(logfile, "n => %d\n", n);
  }

  ret = readpass(buf, n);

  if (d_flag >= 2)
    fprintf(logfile, "Return is %s.\n", ret);
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function readpass.\n");
  if (d_flag) 
    fprintf(logfile, "\n");

  return ret;
}

void _rip (char *buf) {
  if (d_flag >= 1)
    fprintf(logfile, "Entering function rip.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "buf => %s\n", buf);
  }

  rip(buf);

  if (d_flag >= 2)
    fprintf(logfile, "Return type is void.\n");
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function rip.\n");
  if (d_flag) 
    fprintf(logfile, "\n");
}

int _keycrunch(char *result, char *seed, char *passwd) {
  int ret;

  if (d_flag >= 1)
    fprintf(logfile, "Entering function keycrunch.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "result => %s\n", result);
    fprintf(logfile, "seed => %s\n", seed);
    fprintf(logfile, "passwd => %s\n", passwd);
  }

  ret = keycrunch(result, seed, passwd);

  if (d_flag >= 2)
    fprintf(logfile, "Return is %d.\n", ret);
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function keycrunch.\n");
  if (d_flag) 
    fprintf(logfile, "\n");

  return ret;
}

char *_btoe(char *engout, char *c) {
  char *ret;

  if (d_flag >= 1)
    fprintf(logfile, "Entering function btoe.\n");
  if (d_flag >= 3) {
    fprintf(logfile, "Argument:\n");
    fprintf(logfile, "engout => %s\n", engout);
    fprintf(logfile, "c => %s\n", c);
  }

  ret = btoe(engout, c);

  if (d_flag >= 2)
    fprintf(logfile, "Return is %s.\n", ret);
  if (d_flag >= 1)
    fprintf(logfile, "Exiting function btoe.\n");
  if (d_flag) 
    fprintf(logfile, "\n");

  return ret;
}


