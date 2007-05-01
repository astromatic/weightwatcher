 /*
 				vector.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP, Leiden & ESO) C. MARMO (IAP)
*
*	Contents:	Vector structures.
*
*	Last modify:	24/04/2007
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

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
  FLAGTYPE	ofmask;			/* Bit-mask to be OR'ed in output */
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

