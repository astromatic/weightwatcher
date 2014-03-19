/*
*				prefs.h
*
* Include file for prefs.c.
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	This file part of:	WeightWatcher
*
*	Copyright:		(C) 1997-2010 Emmanuel Bertin -- IAP/CNRS/UPMC
*				& Chiara Marmo -- IAP/CNRS
*
*	License:		GNU General Public License
*
*	WeightWatcher is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
* 	(at your option) any later version.
*	WeightWatcher is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*	You should have received a copy of the GNU General Public License
*	along with WeightWatcher. If not, see <http://www.gnu.org/licenses/>.
*
*	Last modified:		26/10/2010
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

#ifndef _PREFS_H_
#define _PREFS_H_

/*----------------------------- Internal constants --------------------------*/

#define		MAXCHARL	16384	/* max. nb of chars in a string list */
#define		MAXLIST		256	/* max. nb of list members */

/*------------------------------- preferences -------------------------------*/
typedef struct
  {
  char		**command_line;			        /* Command line */
  int		ncommand_line;			        /* nb of params */
  char		prefs_name[MAXCHAR];   			/* prefs filename*/
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
  WWFLAGTYPE	weight_mask[MAXNIMAGE];			/* WEIGHT flags */
  int		nweight_mask;				/* number of params */
  WWFLAGTYPE	vec_mask[MAXNIMAGE];			/* POLY flags */
  int		nvec_mask;				/* number of params */
  double	vec_weight[MAXNIMAGE];			/* POLY weights */
  char          *intersec;                              /* flag intersection?*/
  int		nvec_weight;				/* number of params */
  WWFLAGTYPE	flag_wmask[MAXNIMAGE];			/* FLAG weight-mask */
  int		nflag_wmask;				/* number of params */
  WWFLAGTYPE	flag_fmask[MAXNIMAGE];			/* intput FLAG-mask */
  int		nflag_fmask;				/* number of params */
  WWFLAGTYPE	flag_ofmask[32];			/* output FLAGs */
  int		nflag_ofmask;				/* number of params */
  char		*oweight_name;				/* output WEIGHT */
  char		*oflag_name;				/* output FLAG */
  char          *getarea;                               /* compute area? */
  double         weightlim;                             /* weight limit for area */
  WWFLAGTYPE      geta_flags[MAXNIMAGE];                  /* flags for area */
  int           ngeta_flags;                            /* number of params */
  int		mem_bufsize;				/* # of buffer lines */

  enum	{QUIET, NORM, FULL}		verbose_type;	/* display type */
/* Multithreading */
  int		nthreads;			/* Number of active threads */
/* Misc */
  int		xml_flag;			/* Write XML file? */
  char		xml_name[MAXCHAR];              /* XML file name */
  char		xsl_name[MAXCHAR];       	/* XSL file name (or URL) */
  char		sdate_start[12];		/* ww start date */
  char		stime_start[12];		/* ww start time */
  char		sdate_end[12];			/* ww end date */
  char		stime_end[12];			/* ww end time */
  double	time_diff;			/* Execution time */
  }	prefstruct;

  prefstruct	prefs;


/*-------------------------------- protos -----------------------------------*/
extern int	cistrcmp(char *cs, char *ct, int mode);

extern void	dumpprefs(int state),
		endprefs(void),
		readprefs(char *filename,char **argkey,char **argval,int narg),
		useprefs(void);

#endif
