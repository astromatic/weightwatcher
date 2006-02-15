 /*
 				vector.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP, Leiden & ESO)
*
*	Contents:	Vector structures.
*
*	Last modify:	13/02/2006
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
  FLAGTYPE	ofmask;			/* Bit-mask to be OR'ed in output */
  PIXTYPE	weight;			/* Weight factor to be applied to weight-map */
  }	vecstruct;

/* Line segment */
typedef struct seg
  {
  int		ext;			/* Image extension */
  int           ord;                    /* Id number of vector */
  float		x1,y1;			/* Coords of segment's 1st point */
  float		x2,y2;			/* Coords of segment's 2nd point */
  float		slope;			/* (x2-x1)/(y2-y1) */
  }	segstruct;

/*---------------------------------- protos --------------------------------*/

vecstruct	*newvec(char *filename);

void		endvec(vecstruct *vector),
		vec_to_map(vecstruct *vector, picstruct *field, int bufpos,
		int bufsize, int *contextbuf, int ext);

