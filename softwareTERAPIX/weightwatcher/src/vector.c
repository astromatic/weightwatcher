 /*
 				vector.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	Handling of vector structures.
*
*	Last modify:	13/02/2006
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
    npoly++;
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
      seg0->ext = ext;
      seg->ord = npoly;
      seg0->ord = seg->ord;
      }

/*-- POLYGONs are closed figures */
    seg0->x1 = seg->x1;
    seg0->y1 = seg->y1;
    }

/* We now have all the points: forget the last "segment" */
  seg--;

/* Compute slopes */
  for (i=nseg; i--; seg--)
    seg->slope = (fabs(dy=seg->y2-seg->y1)>0.0)?(seg->x2-seg->x1)/dy:0.0;

/* Save memory */
  vector->nsegment = nseg;
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
void	vec_to_map(vecstruct *vector, picstruct *field, int bufpos,
		int bufsize, int *contextbuf, int ext)

  {
   segstruct	*seg;
   FLAGTYPE	*flagbuf, ofmask;
   PIXTYPE	*pixbuf,
		weight;
   float	y;
   int		*cbt, *ns,
		i,j, x,w,yh, pendown, tt;

  w = field->width;
  yh = bufsize/w;
  ofmask = vector->ofmask;
  flagbuf = (FLAGTYPE *)field->strip;
  pixbuf = (PIXTYPE *)field->strip;
  weight = vector->weight;
/* Scan each line */
  for (y = (float)bufpos/w; yh--; y += 1.0)
    {
    seg = vector->segment;
/*-- Clean past history */
    /* memset(contextbuf, 0, w*sizeof(int)); */
    QCALLOC(contextbuf,int,w);
/*-- Find segments that intersect the current scanline */
    QCALLOC(ns,int,vector->nsegment);
    tt=0;
    for (i=vector->nsegment; i--; seg++)
      if (seg->ext==ext)
	if ((y>seg->y1)^(y>seg->y2))
          {
          x = (int)(seg->x1 + (y-seg->y1)*seg->slope + 0.49999);
	  ns[i] = seg->ord;
/*-------- Pile cases where intersection is at the left of the frame ... */
          if (x<0)
            x = 0;
/*-------- ...and forget those where intersection is at the right */
          if (x<w)
	    {
	    for (j=vector->nsegment; j>i; j--)
	      {
	      if (ns[j])
		if (ns[i]!=ns[j])
		  tt++;
		else
		  tt--;
	      /* printf("%d %d %d\n",j,i,tt); */
	      }
	    if(tt)
	      contextbuf[x] |= 1;
	    else
	      contextbuf[x] ^= 1; /* XOR enable us to handle x<0 cases */
	    }
          }
    cbt = contextbuf;
    pendown = 0;
    if (field->flags & FLAG_FIELD)
/*---- First case: update a flag map */
      {
      for (i=w; i--; flagbuf++)
        {
        if (*(cbt++))
	  {
          pendown ^= 1;          
	  /* printf("%d %d\n",cbt,pendown); */
	  } 
        if (pendown)
          *flagbuf |= ofmask;
        }
      }
    else
/*---- Second case: update a weight map */
      {
      for (i=w; i--; pixbuf++)
        {
        if (*(cbt++))
          pendown ^= 1;
        if (pendown)
          *pixbuf *= weight;
        }
      }
    }

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

