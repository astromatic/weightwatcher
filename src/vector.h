/*
*				vector.h
*
* Include file for vector.c.
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

/*--------------------------------- constants -------------------------------*/

#define		MAXPOLYCHAR	4096	/* Max. nb of char per POLYGON line */
#define		NSEGMENTS	64	/* Starting number of segments */

static const char	vectok[] = {";,() \t\n\r"};

/*--------------------------- structure definitions -------------------------*/
/* Polygon vector */
typedef struct vec
  {
  char		filename[MAXCHAR];	/* Name of file containing POLYGONS */
  char		*rfilename;		/* As above, but without dirs */
  char		ident[MAXCHAR];		/* Identificator (a string) */
  FILE		*file;			/* FILE structure */
  struct seg	*segment;		/* Ptr to an array of line segments */
  int		nsegment;		/* Number of line segments */
  int		npoly;			/* Number of polygons involved */
  WWFLAGTYPE	ofmask;			/* Bit-mask to be OR'ed in output */
  PIXTYPE	weight;			/* Weight factor applied to weightmap*/
  }	vecstruct;

/* Line segment */
typedef struct seg
  {
  int		ext;			/* Image extension */
  int           poly;                   /* Id number of current polygon */
  float		x1,y1;			/* Coords of segment's 1st point */
  float		x2,y2;			/* Coords of segment's 2nd point */
  float		slope;			/* (x2-x1)/(y2-y1) */
  }	segstruct;

/* Crossing points */
typedef struct cross
  {
  int		x;			/* x coordinate of the crossing point */
  int		npoly;			/* polygon index */
  }	crosstruct;

/*---------------------------------- protos --------------------------------*/

vecstruct	*newvec(char *filename, catstruct *cat);

void		endvec(vecstruct *vector),
		vec_to_map(vecstruct *vector, picstruct *wfield,
			picstruct *ffield, unsigned long bufpos,
			int bufsize, int *contextbuf, int ext);

