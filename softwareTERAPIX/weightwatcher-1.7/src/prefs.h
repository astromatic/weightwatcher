 /*
 				prefs.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	Keywords for the configuration file.
*
*	Last modify:	12/01/2005
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifndef _PREFS_H_
#define _PREFS_H_

/*----------------------------- Internal constants --------------------------*/

#define	MAXLIST		32		/* max. nb of list members */

/* NOTES:
One must have:	MAXLIST >= 1 (preferably >= 16!)
*/
/*------------------------------- preferences -------------------------------*/
typedef struct
  {
  char		prefs_name[MAXCHAR];			/* prefs filename*/
  char		*(weight_name[MAXNIMAGE]);		/* WEIGHT filenames */
  int		nweight_name;				/* number of params */
  char		*(flag_name[MAXNIMAGE]);		/* WEIGHT filenames */
  int		nflag_name;				/* number of params */
  char		*(vec_name[MAXNIMAGE]);			/* POLY filenames */
  int		nvec_name;				/* number of params */
  double	weight_threshd[MAXNIMAGE];		/* minimum WEIGHT */
  int		nweight_threshd;			/* number of params */
  double	weight_threshu[MAXNIMAGE];		/* maximum WEIGHT */
  int		nweight_threshu;			/* number of params */
  FLAGTYPE	weight_mask[MAXNIMAGE];			/* WEIGHT flags */
  int		nweight_mask;				/* number of params */
  FLAGTYPE	vec_mask[MAXNIMAGE];			/* POLY flags */
  int		nvec_mask;				/* number of params */
  double	vec_weight[MAXNIMAGE];			/* POLY weights */
  int		nvec_weight;				/* number of params */
  FLAGTYPE	flag_wmask[MAXNIMAGE];			/* FLAG weight-mask */
  int		nflag_wmask;				/* number of params */
  FLAGTYPE	flag_fmask[MAXNIMAGE];			/* intput FLAG-mask */
  int		nflag_fmask;				/* number of params */
  FLAGTYPE	flag_ofmask[32];			/* output FLAGs */
  int		nflag_ofmask;				/* number of params */
  char		*oweight_name;				/* output WEIGHT */
  char		*oflag_name;				/* output FLAG */
  int		mem_bufsize;				/* # of buffer lines */

  enum	{QUIET, NORM, FULL}		verbose_type;	/* display type */
/* Multithreading */
  int		nthreads;			/* Number of active threads */
  }	prefstruct;

  prefstruct	prefs;


/*-------------------------------- protos -----------------------------------*/
extern int	cistrcmp(char *cs, char *ct, int mode);

extern void	dumpprefs(void),
		readprefs(char *filename,char **argkey,char **argval,int narg),
		useprefs(void);

#endif
