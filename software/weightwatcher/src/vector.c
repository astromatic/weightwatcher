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
*	Last modify:	25/07/2006
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

static void	chsort(crosstruct *ra, int n);

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
   int		i, npoly, nseg,nseg0, nline, ext;

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
        error(EXIT_FAILURE, vector->filename, " is NOT a"
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

    if (str2 && strstr(str2,"image"))
      str2 = strtok(NULL, vectok);

    if (str2 && !(strstr(str2, "polygon")))
	continue;
      

/*-- Decode the POLYGON information as a set of segments */
    seg0 = seg;
    nseg0 = nseg;

    while(1)
      {
      if (!(str2 = strtok(NULL, vectok)) || *str2=='#')
        break;
      if ((++nseg)>=vector->nsegment)
        {
        vector->nsegment = (int)(vector->nsegment*1.6);
        QREALLOC(vector->segment, segstruct, vector->nsegment);
        seg0 = vector->segment + nseg0;
        seg = vector->segment + nseg;
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
  seg = vector->segment;

/* Compute slopes */
  for (i=nseg; i--; seg++)
    seg->slope = (fabs(dy=seg->y2-seg->y1)>0.0)?(seg->x2-seg->x1)/dy: 1.0e9;

/* Save memory */
  vector->npoly = npoly;
  if (nseg && nseg<vector->nsegment)
    {
    vector->nsegment = nseg;
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
   crosstruct	*crosslist;
   FLAGTYPE	*flagbuf, ofmask;
   PIXTYPE	*pixbuf,
		weight;
   float	y;
   int		*plist, *cbt,
		c, i, p,x, w, yh, nsegpoly,nsegpoly2, ncross, pcount;

/* Check that an output map is requested */
  if (!wfield && !ffield)
    return;
  
  w = wfield? wfield->width : ffield->width;
  yh = bufsize/w;
/* Allocate memory for the list of crossing points */
  if (vector->nsegment)
    {QMALLOC(crosslist, crosstruct, vector->nsegment);}
  else
    return;

  if (vector->npoly)
    {QMALLOC(plist, int, vector->npoly);}
  else
    return;

  ofmask = vector->ofmask;
  flagbuf = ffield? (FLAGTYPE *)ffield->strip : NULL;
  weight = vector->weight;
  pixbuf = wfield? (PIXTYPE *)wfield->strip : NULL;

/*---- Scan each line */
  for (y = (float)bufpos/w; yh--; y += 1.0)
    {
/*-- For each polygon */
    segpoly = vector->segment;
    nsegpoly = 0;
    nsegpoly2 = vector->nsegment;
    for (c=0, p=0; p<vector->npoly; p++)
      {
      segpoly += nsegpoly;
      nsegpoly = 0;
      for (seg=segpoly; nsegpoly2-- && seg->poly==p; seg++)
        nsegpoly++;
      nsegpoly2++;
      if (segpoly->ext != ext)
        continue;
      seg = segpoly;
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
            {
            crosslist[c].x = x;
            crosslist[c].npoly = p;
            c++;
            }
          }
      }

    ncross = c;
    if (ncross>1)
      chsort(crosslist, ncross);
    memset(plist, 0, vector->npoly*sizeof(int));
    c = 0;
    pcount = 0;
    cbt = contextbuf;
    if (prefs.intersec)
      for (x=0; x<w; x++)
        {
        for (;c<ncross && crosslist[c].x == x; c++)
          {
          if ((plist[crosslist[c].npoly] ^= 1))
            pcount++;
          else
            pcount--;
          }
        *(cbt++) = pcount;
        }
    else
      for (x=0; x<w; x++)
        {
        for (;c<ncross && crosslist[c].x == x; c++)
          pcount ^=1;
        *(cbt++) = pcount;
        }

    if (ffield && (ffield->flags & FLAG_FIELD))
/*---- First case: update a flag map */
      {
      cbt = contextbuf;
      for (i=w; i--; flagbuf++)
        if ((*(cbt++)))
          *flagbuf |= ofmask;
      }

    if (wfield)
/*--- Second case: update a weight map */
      {
      cbt = contextbuf;
      for (i=w; i--; pixbuf++)
        if ((*(cbt++)))
          *pixbuf *= weight;
      }
    }

  free(crosslist);
  free(plist);

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


/********************************** chsort ************************************/
/*
Heapsort algorithm
*/
void    chsort(crosstruct *ra, int n)

  {
   int          l, j, ir, i;
   crosstruct	rra;

  ra--;
  for(l = ((ir=n)>>1)+1;;)
    {
    if (l>1)
      rra = ra[--l];
    else
      {
      rra = ra[ir];
      ra[ir] = ra[1];
      if (--ir == 1)
        {
        ra[1] = rra;
        return;
        }
      }
    for (j = (i=l)<<1; j <= ir;)
      {
      if (j < ir && ra[j].x < ra[j+1].x)
        ++j;
      if (rra.x < ra[j].x)
        {
        ra[i] = ra[j];
        j += (i=j);
        }
      else
        j = ir + 1;
      }
    ra[i] = rra;
    }
  }
