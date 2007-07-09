 /*
 				field.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP) C. Marmo (IAP)
*
*	Contents:	Handling of field structures.
*
*	Last modify:	28/06/2007
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

#include	"define.h"
#include	"globals.h"
#include	"field.h"
#include	"fits/fitscat.h"
#include	"prefs.h"
#include	"readimage.h"

/********************************* newfield **********************************/
/*
Returns a pointer to a new field, ready to go!
*/
picstruct	*newfield(char *filename, int flags, picstruct *mfield,
			int nok, int next)

  {
   catstruct	*cat;
   tabstruct	*tab;
   picstruct	*field;
   double	dval;
   OFF_T	mefpos;
   char		*buf;
   int		ival, nok2, ntab, n;

/* First allocate memory for the new field (and nullify pointers) */
  mefpos = 0; /* Avoid gcc -Wall warnings */
  QCALLOC(field, picstruct, 1);
  if (mfield)
    *field = *mfield;
  field->flags = flags;
  strcpy (field->filename, filename);
/* A short, "relative" version of the filename */
  if (!(field->rfilename=strrchr(field->filename, '/')))
    field->rfilename = field->filename;

  if (mfield)
    {
    if (!nok)
      {
      sprintf(gstr, "Opening %s", field->rfilename);
      NFPRINTF(OUTPUT, gstr);
      if (!(field->file = fopen(field->filename, "wb")))
        error(EXIT_FAILURE,"*Error*: Cannot open for output ",field->filename);
      if (next>1)
        {
/*------ Create a small primary header */
        QCALLOC(buf, char, FBSIZE);
        strcpy(buf, "END     ");
        fitsadd(buf, "SIMPLE  ",  "Standard FITS");
        ival = 1;
        fitswrite(buf, "SIMPLE  ", &ival, H_BOOL, T_LONG);
        fitsadd(buf, "BITPIX  ",  "8 bits per character");
        ival = 8;
        fitswrite(buf, "BITPIX  ", &ival, H_INT, T_LONG);
        fitsadd(buf, "NAXIS  ",  "No image data with primary header");
        ival = 0;
        fitswrite(buf, "NAXIS   ", &ival, H_INT, T_LONG);
        fitsadd(buf, "EXTEND  ",  "File contains extensions");
        fitswrite(buf, "EXTEND  ", &next, H_BOOL, T_LONG);
        fitsadd(buf, "NEXTEND ",  "Number of extensions");
        fitswrite(buf, "NEXTEND ", &next, H_INT, T_LONG);
        QFWRITE(buf, FBSIZE, field->file, field->filename);
        free(buf);
        }
      }
    else
      {
      if (!(field->file = fopen(field->filename, "rb+")))
        error(EXIT_FAILURE,"*Error*: Cannot append to ",field->filename);
      fseek(field->file, 0L, SEEK_END);
      }
    n = 1; /* Avoid gcc -Wall warnings */
/* Looking for copyright Keywords */
    if ((fitsfind(field->fitshead, "SOFTNAME")==RETURN_ERROR))
      n++;
    if ((fitsfind(field->fitshead, "SOFTVERS")==RETURN_ERROR))
      n++;
    if ((fitsfind(field->fitshead, "SOFTDATE")==RETURN_ERROR))
      n++;
    if ((fitsfind(field->fitshead, "SOFTAUTH")==RETURN_ERROR))
      n++;
    if ((fitsfind(field->fitshead, "SOFTINST")==RETURN_ERROR))
      n++;

    if (prefs.getarea)
      n+=2;

    if (fitsfind(field->fitshead, "END     ")+n >= field->fitsheadsize/80)
      {
      QCALLOC(field->fitshead, char, field->fitsheadsize+FBSIZE);
      memcpy(field->fitshead, mfield->fitshead, field->fitsheadsize);
      field->fitsheadsize += FBSIZE;
      }
    else
      {
      QCALLOC(field->fitshead, char, field->fitsheadsize);
      memcpy(field->fitshead, mfield->fitshead, field->fitsheadsize);
      }
    fitsadd(field->fitshead, "SOFTNAME",
	"The software that processed those data");
    fitsadd(field->fitshead, "SOFTVERS", "Version of the software");
    fitsadd(field->fitshead, "SOFTDATE", "Release date of the software");
    fitsadd(field->fitshead, "SOFTAUTH", "Maintainer of the software");
    fitsadd(field->fitshead, "SOFTINST", "Home institute  of the software");
    if (prefs.getarea)
      fitsadd(field->fitshead, "EFF_AREA", "effective area");
    fitswrite(field->fitshead, "SOFTNAME", BANNER, H_STRING, T_STRING);
    fitswrite(field->fitshead, "SOFTVERS", MYVERSION, H_STRING, T_STRING);
    fitswrite(field->fitshead, "SOFTDATE", DATE, H_STRING, T_STRING);
    fitswrite(field->fitshead, "SOFTAUTH", COPYRIGHT, H_STRING, T_STRING);
    fitswrite(field->fitshead, "SOFTINST", INSTITUTE, H_STRING, T_STRING);

/*-- Neutralize possible scaling factors */
    dval = 1.0; fitswrite(field->fitshead, "BSCALE  ",&dval,H_FLOAT,T_DOUBLE);
    dval = 0.0; fitswrite(field->fitshead, "BZERO   ",&dval,H_FLOAT,T_DOUBLE);
    if (field->compress_type != ICOMPRESS_NONE)
      {
      field->compress_type = ICOMPRESS_NONE;
      fitswrite(field->fitshead, "IMAGECOD", "NONE", H_STRING, T_STRING);
      }
    fitswrite(field->fitshead, "ORIGIN  ", BANNER, H_STRING, T_STRING);
    if (flags & FLAG_FIELD)
      {
      ival = 1; fitswrite(field->fitshead, "BITSGN  ",&ival, H_INT, T_LONG);
      fitswrite(field->fitshead, "BITPIX  ", &field->bitpix, H_INT, T_LONG);
      fitswrite(field->fitshead, "OBJECT  ", "FLAG MAP", H_STRING,T_STRING);
      if (prefs.getarea)
        fitsadd(field->fitshead, "FLAGAREA",
            "Bits which will not be accounted in the area");
      }
    else
      {
      ival = 1; fitswrite(field->fitshead, "BITSGN  ",&ival, H_INT, T_LONG);
      fitswrite(field->fitshead, "OBJECT  ", "WEIGHT MAP", H_STRING,T_STRING);
      field->bitpix = BP_FLOAT;
      if (prefs.getarea)
        {
        fitsadd(field->fitshead, "WEIGAREA",
            "Weight inferior limit accounted in the area");
        fitswrite(field->fitshead, "WEIGAREA",&prefs.weightlim,H_FLOAT,
            T_DOUBLE);
        }
      }
    fitswrite(field->fitshead, "BITPIX  ", &field->bitpix, H_INT, T_LONG);
    field->bytepix = (field->bitpix>0?field->bitpix:-field->bitpix)>>3;
    QFWRITE(field->fitshead,field->fitsheadsize,field->file,field->filename);
    }
  else
    {
    if (!nok && prefs.verbose_type != QUIET)
      {
      sprintf(gstr, "Looking for %s", field->rfilename);
      NFPRINTF(OUTPUT, gstr);
      }
/*-- Move to nok'th valid FITS image extension */
    if (!(cat = read_cat(filename)))
      error(EXIT_FAILURE, "*Error*: cannot open ", filename);
    close_cat(cat);
    tab = cat->tab;
    nok++;        /* At least one pass through the loop */
    nok2 = nok;
    for (ntab=cat->ntab; nok && ntab--; tab=tab->nexttab)
      {
      if ((tab->naxis < 2)
        || !strncmp(tab->xtension, "BINTABLE", 8)
        || !strncmp(tab->xtension, "ASCTABLE", 8))
        continue;
      mefpos = tab->headpos;
      nok--;
      }
    if (ntab<0)
      error(EXIT_FAILURE, "Not enough valid FITS image extensions in ",
			filename);
    field->mefpos = mefpos;

/*-- Check that the image exists and read important info (image size, etc...)*/
    readimagehead(field);
    if (cat->ntab>1)
      sprintf(gstr, "[%d/%d]", nok2, cat->ntab-1);
    else if (cat->ntab>9)
      sprintf(gstr, "[%2d/%d]", nok2, cat->ntab-1);
    else if (cat->ntab>99)
      sprintf(gstr, "[%3d/%d]", nok2, cat->ntab-1);
    if (prefs.verbose_type != QUIET)
      fprintf(OUTPUT, "Frame:    \"%.20s\" %s / %ld x %ld / %d bits %s data\n",
	field->ident,
        cat->ntab>1? gstr : "",
	field->width, field->height, field->bytepix*8,
	field->bitpix>0?
		(field->compress_type!=ICOMPRESS_NONE?"COMPRESSED":"INTEGER")
		:"FLOATING POINT");

/*-- Provide a buffer for compressed data */
    if (field->compress_type != ICOMPRESS_NONE)
      QMALLOC(field->compress_buf, char, FBSIZE);
    free_cat(&cat, 1);
    }

  field->stripheight = prefs.mem_bufsize;
  if (field->stripheight>field->height)
    field->stripheight = field->height;
  if (!(field->strip=malloc(field->stripheight*field->width*4)))
    error(EXIT_FAILURE,"Not enough memory for the image buffer in ",
	field->rfilename);

  return field;
  }

/********************************* endfield **********************************/
/*
Free and close everything related to a field structure.
*/
void	endfield(picstruct *field)

  {
  fclose(field->file);

  free(field->fitshead);
  free(field->strip);
  free(field->compress_buf);
  free(field);

  return;
  }

