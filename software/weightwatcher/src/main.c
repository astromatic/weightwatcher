/*
 				main.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	Parsing of the command line.
*
*	Last modify:	11/12/2004
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "globals.h"
#include "field.h"
#include "fits/fitscat.h"
#include "prefs.h"
#ifdef HAVE_PLPLOT
#include "plplot/plplot.h"
#endif

#define		SYNTAX \
EXECUTABLE " [-c <configuration_file>] [-<keyword> <value>]\n"\
" or, to dump a default configuration file:\n" \
EXECUTABLE " -d \n" \
"> to dump a default extended configuration file: " EXECUTABLE " -dd \n"

extern const char	notokstr[];

/********************************** main ************************************/
int	main(int argc, char *argv[])

  {
   static char	prefsname[MAXCHAR];
   char		**argkey, **argval;
   int		a, narg, opt, opt2;

  if (argc<2)
    {
    fprintf(OUTPUT, "\n         %s  Version %s (%s)\n", BANNER,MYVERSION,DATE);
    fprintf(OUTPUT, "\nby %s\n", COPYRIGHT);
    fprintf(OUTPUT, "visit %s\n", WEBSITE);
    error(EXIT_SUCCESS, "SYNTAX: ", SYNTAX);
    }

  QMALLOC(argkey, char *, argc);
  QMALLOC(argval, char *, argc);

/* Default parameters */
  strcpy(prefsname, "default.ww");
  narg= 0;

  for (a=1; a<argc; a++)
    {
    if (*(argv[a]) == '-')
      {
      opt = (int)argv[a][1];
      if (strlen(argv[a])<4 || opt == '-')
        {
        opt2 = (int)tolower((int)argv[a][2]);
        if (opt == '-')
          {
          opt = opt2;
          opt2 = (int)tolower((int)argv[a][3]);
          }
        switch(opt)
          {
          case 'c':
            if (a<(argc-1))
              strcpy(prefsname, argv[++a]);
            break;
          case 'd':
            dumpprefs(opt2=='d' ? 1 : 0);
            exit(EXIT_SUCCESS);
            break;
          case 'v':
            printf("%s version %s (%s)\n", BANNER,MYVERSION,DATE);
            exit(0);
            break;
          case 'h':
          default:
            error(EXIT_SUCCESS,"SYNTAX: ", SYNTAX);
          }
        }
      else
        {
        argkey[narg] = &argv[a][1];
        argval[narg++] = argv[++a];
        }       
      }
    else
      error(EXIT_SUCCESS,"SYNTAX: ", SYNTAX);
    }

  readprefs(prefsname, argkey, argval, narg);
  useprefs();

  free(argkey);
  free(argval);

  makeit();

  NFPRINTF(OUTPUT, "");
  NPRINTF(OUTPUT, "> All done (in %.0f s)\n", prefs.time_diff);


  exit(EXIT_SUCCESS);
  }

