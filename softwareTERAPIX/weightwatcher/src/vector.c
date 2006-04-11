 /*
 				vector.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP) C. MARMO (IAP)
*
*	Contents:	Handling of vector structures.
*
*	Last modify:	11/04/2006
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "globals.h"
#include "field.h"
#include "fits/fitscat.h"
#include "prefs.h"
#include "vector.h"

/********************************** newvec **********************************/
/*
Returns a pointer to a new polygon, and initialize local context buffer:
ready to go!
*/
vecstruct	*newvec(char *filename)

  {
   vecstruct	*vector;
   segstruct	*seg, *seg0;
   static char	str[MAXPOLYCHAR];
   float	dy;
   char		*str2;
   int		i, npoly, nseg, nline, ext;

/* First allocate memory for the new vector (and nullify pointers) */
  QCALLOC(vector, vecstruct, 1);
  strcpy(vector->filename, filename);

/* A short, "relative" version of the filename */
  if (!(vector->rfilename=strrchr(vector->filename, '/')))
    vector->rfilename = vector->filename;

  sprintf(gstr, "Looking for %s", vector->rfilename);
  NFPRINTF(OUTPUT, gstr);

/* Open file */
  if (!(vector->file = fopen(vector->filename, "r")))
    error(EXIT_FAILURE,"*Error*: cannot open ", vector->filename);

/* Allocate an arbitrary amount of memory to start with */
  vector->nsegment = NSEGMENTS;
  QMALLOC(vector->segment, segstruct, vector->nsegment);
  seg = vector->segment;
  npoly = nseg = nline = 0;

/* Read data */
  while (fgets(str, MAXPOLYCHAR, vector->file))
    {
/* Make almost sure from first line that we are accessing the right data */
    if (!nline)
      {
      if (*str != (char)'#')
        error(EXIT_FAILURE, vector->filename, " is NOT an"
		" DS9/SAOimage vector-file");
      if (*str)
        str[strlen(str)-1] = (char)'\0';
      strncpy(vector->ident, str+2, MAXCHAR);
      nline = 1;
      }

/*-- Examine current input line (discard empty and comment lines) */
    if (strstr(str, "tile"))
      {
      str2=strtok(str, vectok);
      ext=atoi(strtok(NULL, vectok))-1;
      str2 = strtok(NULL, vectok);
      }
    else
      {
      ext = 0;
      str2=strtok(str, vectok);
      }

    if (strstr(str2,"image"))
      {
      if (!(strstr(strtok(NULL, vectok), "polygon")))
	continue;
      }
    else
      continue;

      

/*-- Decode the POLYGON information as a set of segments */
    seg0 = seg;
    while(1)
      {
      if (!(str2 = strtok(NULL, vectok)))
        break;
      if ((++nseg)>=vector->nsegment)
        {
        vector->nsegment = (int)(vector->nsegment*1.6);
        QREALLOC(vector->segment, segstruct, vector->nsegment);
        seg = vector->segment+nseg;
        }
      else
        seg++;
      seg->x1 = (seg-1)->x2 = atof(str2) - 1.0;	/* 1st pixel is "1" in FITS */
      if (!(str2 = strtok(NULL, vectok)))
        error(EXIT_FAILURE, "Malformed POLYGON in ", vector->filename);
      seg->y1 = (seg-1)->y2 = atof(str2) - 1.0;	/* 1st pixel is "1" in FITS */
      seg->ext = ext;
      seg->poly = npoly;
      }

/*-- POLYGONs are closed figures */
    seg0->x1 = seg->x1;
    seg0->y1 = seg->y1;
    seg0->ext = ext;
    seg0->poly = npoly;
    npoly++;
    }

/* We now have all the points: forget the last "segment" */
  seg--;

/* Compute slopes */
  for (i=nseg; i--; seg--)
    seg->slope = (fabs(dy=seg->y2-seg->y1)>0.0)?(seg->x2-seg->x1)/dy:0.0;

/* Save memory */
  vector->nsegment = nseg;
  vector->npoly = npoly;
  if (nseg && nseg<vector->nsegment)
    {
    QREALLOC(vector->segment, segstruct, vector->nsegment);
    }

  if (prefs.verbose_type != QUIET)
    fprintf(OUTPUT, "Polygons: \"%.20s\" / %d polygons\n",
		vector->ident, npoly);

/* Close file and exit */
  fclose(vector->file);

  return vector;
  }


/********************************* vec_to_map *******************************/
/*
Update a field image buffer, according to vector information found in the
vector polygon.
*/
void	vec_to_map(vecstruct *vector, picstruct *wfield, picstruct *ffield,
		int bufpos, int bufsize, int *contextbuf, int ext)

  {
   segstruct	*seg, *segpoly;
   FLAGTYPE	*flagbuf, ofmask;
   PIXTYPE	*pixbuf,
		weight;
   float	y;
   int		*cbt, *orbuf, *obt,
		i, p,x, w, yh, pendown, nsegpoly,nsegpoly2;

/* Check that an output map is requested */
  if (!wfield && !ffield)
    return;
  
  w = wfield? wfield->width : ffield->width;
  yh = bufsize/w;
  QMALLOC(orbuf, int, w);
/*---- Scan each line */
  for (y = (float)bufpos/w; yh--; y += 1.0)
    {
    memset(orbuf, 0, w*sizeof(int));
/*-- For each polygon */
    segpoly = vector->segment;
    nsegpoly = 0;
    nsegpoly2 = vector->nsegment;
    for (p=0; p<vector->npoly; p++)
      {
      segpoly += nsegpoly;
      nsegpoly = 0;
      for (seg=segpoly; nsegpoly2-- && seg->poly==p; seg++)
        nsegpoly++;
      nsegpoly2++;
      if (segpoly->ext != ext)
        continue;
      seg = segpoly;
/*---- Clean past history */
      memset(contextbuf, 0, w*sizeof(int));
/*---- Find segments that intersect the current scanline */
      for (i=nsegpoly; i--; seg++)
	if ((y>seg->y1)^(y>seg->y2))
          {
          x = (int)(seg->x1 + (y-seg->y1)*seg->slope + 0.49999);
/*-------- Pile cases where intersection is at the left of the frame ... */
          if (x<0)
            x = 0;
/*-------- ...and forget those where intersection is at the right */
          if (x<w)
	    contextbuf[x] ^= 1; /* XOR enable us to handle x<0 cases */
          }
/*---- "Integrate" contextbuf */
      cbt = contextbuf;
      obt = orbuf;
      pendown = 0;

/*---- polygon intersection are flagged */
      if(prefs.intersec)
        for (i=w; i--; obt++)
          {
          if ((*(cbt++)))
            pendown ^= 1;          
          *obt |= pendown; /* OR option */
          }
/*---- polygon intersection are NOT flagged */
      else
        for (i=w; i--; obt++)
          {
          if ((*(cbt++)))
            pendown ^= 1;          	
	  *obt ^= pendown; /*  XOR option */
	  }

      }

    if (ffield && (ffield->flags & FLAG_FIELD))
/*---- First case: update a flag map */
      {
      ofmask = vector->ofmask;
      flagbuf = (FLAGTYPE *)ffield->strip;
      obt = orbuf;
      for (i=w; i--; flagbuf++)
        if ((*(obt++)))
          *flagbuf |= ofmask;
      }
    if (wfield)
/*--- Second case: update a weight map */
      {
      weight = vector->weight;
      pixbuf = (PIXTYPE *)wfield->strip;
      obt = orbuf;
      for (i=w; i--; pixbuf++)
        if ((*(obt++)))
          *pixbuf *= weight;
      }
    }

  free(orbuf);

  return;
  }


/********************************** endvec **********************************/
/*
Free everything related to a vector structure.
*/
void	endvec(vecstruct *vector)

  {
  free(vector->segment);
  free(vector);

  return;
  }

