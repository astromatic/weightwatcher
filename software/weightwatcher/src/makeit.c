/*
                                  makeit.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*       Part of:        WeightWatcher
*
*       Authors:         E.BERTIN & C.MARMO (IAP)
*
*       Contents:       Main program
*
*       Last modify:    04/04/2007
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "define.h"
#include "globals.h"
#include "fits/fitscat.h"
#include "field.h"
#include "prefs.h"
#include "vector.h"
#include "readimage.h"
#include "xml.h"

time_t		thetime, thetime2;

/********************************** makeit ***********************************/
void	makeit(void)
  {
   catstruct	*cat;
   tabstruct	*tab;
   vecstruct	**vec, *vector;
   picstruct	**wfield, **ffield, *field, *owfield, *offield;
   PIXTYPE	*pix, *pixin, *owstrip, *weight,
		val, threshd,threshu;
   FLAGTYPE	*flagin, *flag,
		*pofmask,*ofmask,*fmask2,
		nofmask, flagmask, fmask,wmask,fval, maxbit;
   int		i,j, t, padsize, ext, next, ntab, nxml;
   char		*charpix, *ofstrip, *filename;
   short	*shortpix;
   int		*contextbuf;
   unsigned long area, area0, arposw, arposf, headposw, headposf, width, height;
   float        wwlim;
   double       farea, farea0;
   size_t	spoonful, stripsize, cumspoon;
   KINGSIZE_T	bowl, npix;
   struct tm		*tm;

/* Install the signal-catching routines for temporary file cleanup */
#ifdef USE_THREADS
  install_cleanup(NULL);
#else
  install_cleanup(NULL);
#endif

/* Install error logging */
  error_installfunc(write_error);

/* Processing start date and time */
  thetime = time(NULL);
  tm = localtime(&thetime);
  sprintf(prefs.sdate_start,"%04d-%02d-%02d",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
  sprintf(prefs.stime_start,"%02d:%02d:%02d",
        tm->tm_hour, tm->tm_min, tm->tm_sec);

  NFPRINTF(OUTPUT, "");
  QPRINTF(OUTPUT,
        "----- %s %s started on %s at %s with %d thread%s\n\n",
                BANNER,
                MYVERSION,
                prefs.sdate_start,
                prefs.stime_start,
                prefs.nthreads,
                prefs.nthreads>1? "s":"");


  field = NULL; /* Avoid gcc -Wall warnings */
  owfield = offield = NULL;     /* No output weights or flags is the default */
  maxbit = 0;
  bowl = 0;
  wwlim=0.0;

/* Compute the number of valid input extensions */
  filename = NULL;	/* to avoid gcc -Wall warnings */
  if (prefs.nweight_name)
    filename = prefs.weight_name[0];
  else if (prefs.nflag_name)
    filename = prefs.flag_name[0];
  else
    error(EXIT_FAILURE, "*Error*: no input image available!!","");
  if (!(cat = read_cat(filename)))
    error(EXIT_FAILURE, "*Error*: cannot open ", filename);
  tab = cat->tab;
  next = nxml = 0;
  for (ntab = 0 ; ntab<cat->ntab; ntab++, tab = tab->nexttab)
    {
/*--  Check for the next valid image extension */
    if ((tab->naxis < 2)
        || !strncmp(tab->xtension, "BINTABLE", 8)
        || !strncmp(tab->xtension, "ASCTABLE", 8))
      continue;
    next++;
    }

  if (prefs.xml_flag)
    {
    nxml = (prefs.oweight_name != NULL && prefs.oflag_name != NULL)? 2*next : next ; 
    init_xml(nxml);
    }
/* Open vector images */
  if (prefs.nvec_name)
    QMALLOC(vec, vecstruct *, prefs.nvec_name)
  else
    vec = NULL;
  for (i=0; i<prefs.nvec_name; i++)
    {
    vector = vec[i] = newvec(prefs.vec_name[i],cat);
    vector->ofmask = prefs.vec_mask[i];
    vector->weight = prefs.vec_weight[i];
    if (vector->ofmask > maxbit)
      maxbit = vector->ofmask;
    }
  free_cat(&cat, 1);  
  width = height = 0;	/* Avoid gcc -Wall warnings */
  for (ext=0; ext<next; ext++)
/* Open weight images */
    {
    if (prefs.nweight_name)
      QMALLOC(wfield, picstruct *, prefs.nweight_name)
    else
      wfield = NULL;
    for (i=0; i<prefs.nweight_name; i++)
      {
      wfield[i] = field = newfield(prefs.weight_name[i], WEIGHT_FIELD, NULL,
				ext, next);
      if (i && (field->width!=width || field->height!=height))
        error(EXIT_FAILURE, "*Error*: incompatible image size for ",
		field->filename);
      else
        {
        width = field->width;
        height = field->height;
        }
      field->threshd = (PIXTYPE)prefs.weight_threshd[i];
      field->threshu = (PIXTYPE)prefs.weight_threshu[i];
      if (field->threshd>field->threshu)
        warning("Overlapping thresholds for ", field->rfilename);
      field->ofmask[0] = prefs.weight_mask[i];
      if (field->ofmask[0] > maxbit)
        maxbit = field->ofmask[0];
      }

/*-- Open bitmap (flag) images */
    if (prefs.nflag_name)
      QMALLOC(ffield, picstruct *, prefs.nflag_name)
    else
      ffield = NULL;
    pofmask = prefs.flag_ofmask;
    nofmask = prefs.nflag_ofmask;
    for (i=0; i<prefs.nflag_name; i++)
      {
      ffield[i] = field = newfield(prefs.flag_name[i], FLAG_FIELD, NULL,
				ext, next);
      if (i && (field->width!=width || field->height!=height))
        error(EXIT_FAILURE, "*Error*: incompatible image size for ",
		field->filename);
      else
        {
        width = field->width;
        height = field->height;
        }
      field->wmask = wmask = prefs.flag_wmask[i];

/*---- Examine the FLAG masks */
      field->fmask = fmask = prefs.flag_fmask[i];
      field->nfmask = 0;
      ofmask = field->ofmask;
      fmask2 = field->fmask2;
      for (j=0; j<31;j++)
        if (fmask&(fval = 1<<j))
          {
          field->nfmask++;
          *(fmask2++) = fval;
          if (nofmask--)
            {
            if ((fval=*(ofmask++)=*(pofmask++)) > maxbit)
              maxbit = fval;
            }
          else
            error(EXIT_FAILURE, "*Error*: missing output flag mask for ",
			field->filename);
          }
      }

/*-- Prepare the WEIGHT output-image */
    owstrip = NULL;	/* Avoid gcc -Wall warnings */
    stripsize = 0;	/* Avoid gcc -Wall warnings */
    if (*prefs.oweight_name)
      {
      owfield = newfield(prefs.oweight_name, WEIGHT_FIELD,
		wfield? *wfield : *ffield, ext, next);
      stripsize = owfield->stripheight*owfield->width;
      owstrip = (PIXTYPE *)owfield->strip;
      bowl = owfield->npix;
      }

/*-- Prepare the FLAG output-image */
    ofstrip = NULL;	/* Avoid gcc -Wall warnings */
    if (*prefs.oflag_name)
      {
/*---- We first make a dummy copy of a FLAG input-image as a reference */
      QMALLOC(field, picstruct, 1);
      *field = ffield?**ffield:**wfield;
/*---- Select the output FLAG image # of bits per pixel */
      if (maxbit >= (1<<15))
        field->bitpix = BP_LONG;
      else if (maxbit >= (1<<7))
        field->bitpix = BP_SHORT;
      else
        field->bitpix = BP_BYTE;
      offield = newfield(prefs.oflag_name, FLAG_FIELD, field, ext, next);
      free(field);
      stripsize = offield->stripheight*offield->width;
      ofstrip = (char *)offield->strip;
      bowl = offield->npix;
      }

/*-- Prepare the "context buffer" for polygon masking */
    if (vec)
      {QMALLOC(contextbuf, int, offield?offield->width:owfield->width);}
    else
      contextbuf = NULL;

/*-- Enter the main loop */
    NFPRINTF(OUTPUT, "Processing...");
    cumspoon = 0;
    spoonful = stripsize;
    flagmask = 0;
    area = area0 =0;
    farea = farea0 =0.;
    if (prefs.getarea)
      for (t=0;t<prefs.ngeta_flags;t++)
        flagmask += prefs.geta_flags[t];
    for (; bowl; bowl -= spoonful)
      {
      if (spoonful>bowl)
        spoonful = bowl;

/*---- Init the WEIGHT and FLAG buffers */
      if (owfield)
        for (pix=owfield->strip,npix=spoonful;npix--;)
          *(pix++) = 1.0;
      if (offield)
        memset(offield->strip, 0, (size_t)spoonful*sizeof(FLAGTYPE));

/*---- First step: extract info from input weight maps */
      for (i=0; i<prefs.nweight_name; i++)
        {
        field = wfield[i];
        readdata(field, (PIXTYPE *)field->strip, spoonful);
        if (owfield)
          {
          pixin = (PIXTYPE *)field->strip;
          pix = owfield->strip;
          threshd = field->threshd;
          threshu = field->threshu;
          for (npix = spoonful; npix--;)
            *(pix++) *= (((val=*(pixin++))>threshu || val<threshd)?
                        0.0: (i?1.0:val));
          }

        if (offield && (fmask=field->ofmask[0]))
          {
          pixin = (PIXTYPE *)field->strip;
          flag = offield->strip;
          threshd = field->threshd;
          threshu = field->threshu;
          for (npix = spoonful; npix--; flag++)
            if ((val=*(pixin++))>threshu || val<threshd)
              *flag |= fmask;
          }
        }

/*---- Second step: extract info from input flag maps */
      for (i=0; i<prefs.nflag_name; i++)
        {
        field = ffield[i];
        readidata(field, (FLAGTYPE *)field->strip, spoonful);
        if (owfield && (wmask=field->wmask))
          {
          flagin = (FLAGTYPE *)field->strip;
          pix = owfield->strip;
          for (npix = spoonful; npix--; pix++)
            if (*(flagin++)&wmask)
              *pix = 0.0;
          }

        if (offield && (fmask=field->fmask))
          {
          flagin = (FLAGTYPE *)field->strip;
          flag = offield->strip;
          for (npix = spoonful; npix--; flag++)
            if ((fval=*(flagin++))&fmask)
              {
              ofmask = field->ofmask;
              fmask2 = field->fmask2;
              for(j=field->nfmask; j--; ofmask++)
                if (fval&*(fmask2++))
                  *flag |= *ofmask;
              }
          }
        }

/*---- Third step: extract info from input polygon vectors */
      for (i=0; i<prefs.nvec_name; i++)
        vec_to_map(vec[i], owfield, offield,
		cumspoon, spoonful, contextbuf, ext);
      cumspoon += spoonful;

/*---- Write WEIGHT buffer */

      if (owfield)
        {
	if (prefs.getarea)
          {
	  /* Computing area having zeroes on weight image*/	  
	  weight = (PIXTYPE *)owfield->strip;
          for (npix = spoonful; npix--;)
            area0 += ((*(weight++))>prefs.weightlim);
          farea0 = (double)(area0)/(double)(width*height);
	  }
        if (bswapflag)
          swapbytes(owstrip, 4, spoonful);
        QFWRITE(owstrip, spoonful*sizeof(PIXTYPE), owfield->file,
        offield->rfilename);
        }

/*---- Convert the FLAG 32bits data to the output buffer (8,16 or 32 bits) */
      if (offield)
        {
	if (prefs.getarea)
          {
          /* Computing area having flag on flag image*/
          flag = (FLAGTYPE *)offield->strip;
          for (npix = spoonful; npix--;)
            area += ((*(flag++)&flagmask)!=0);
          }
        if (offield->bitpix!=BP_LONG)
          {
          flag = (FLAGTYPE *)offield->strip;
          switch(offield->bitpix)
            {
            case BP_BYTE:
              charpix = ofstrip;
              for (npix = spoonful; npix--;)
                *(charpix++) = (BYTE)*(flag++);
              break;
            case BP_SHORT:
              shortpix = (short *)ofstrip;
              for (npix = spoonful; npix--;)
                *(shortpix++) = (BYTE)*(flag++);
              break;
            case BP_LONG:
              break;
            default:
              error(EXIT_FAILURE, "*Internal Error*: ",
			"this should not occur!");
              break;
            }
          }

/*------ Write FLAG buffer */
        if (bswapflag)
          swapbytes(ofstrip, offield->bytepix, spoonful);
        QFWRITE(ofstrip, spoonful*offield->bytepix, offield->file,
          offield->rfilename);
        }
      }

    free(contextbuf);

    for (i=0; i<prefs.nweight_name; i++)
      endfield(wfield[i]);
    free(wfield);

    for (i=0; i<prefs.nflag_name; i++)
      endfield(ffield[i]);
    free(ffield);

/* Adding information on effective area */
    if (prefs.getarea)
      {
      NPRINTF(OUTPUT, "\n \n");
      NPRINTF(OUTPUT, "> Total number of pixels = %ld\n",width*height);
      if (owfield)
        {
        farea0 = (double)(area0)/(double)(width*height);
        NPRINTF(OUTPUT, "> Fraction of pixels weighted more than %4.2f = %6.4e\n",prefs.weightlim,farea0);
        arposw=ftell(owfield->file);
        headposw=arposw-(owfield->npix*owfield->bytepix)
             -owfield->fitsheadsize;
        /* -- writing EFF_AREA keyword in weight header  */
        fseek(owfield->file,headposw,SEEK_SET);
        fitswrite(owfield->fitshead, "EFF_AREA",&farea0,H_FLOAT,T_DOUBLE);
        QFWRITE(owfield->fitshead,owfield->fitsheadsize,
               owfield->file, owfield->rfilename);
        fseek(owfield->file,arposw,SEEK_SET);
        owfield->effarea = farea0;
        }
      if (offield)
        {
        farea = 1. - (double)(area)/(double)(width*height);
        NPRINTF(OUTPUT, "> Pixels flagged as ");
        for (t=0;t<prefs.ngeta_flags-1;t++)
	  NPRINTF(OUTPUT, "%d OR ",prefs.geta_flags[t]);
        NPRINTF(OUTPUT, "%d ",prefs.geta_flags[prefs.ngeta_flags-1]);
        NPRINTF(OUTPUT, "= %ld\n",area);
        NPRINTF(OUTPUT, "> Fraction of pixels not flagged= %6.4e\n",farea);
        arposf=ftell(offield->file);
        headposf=arposf-(offield->npix*offield->bytepix)
             -offield->fitsheadsize;
        /* -- writing EFF_AREA keyword in flag header  */
        fseek(offield->file,headposf,SEEK_SET);
        fitswrite(offield->fitshead, "FLAGAREA",&flagmask, H_INT,T_LONG);
        fitswrite(offield->fitshead, "EFF_AREA",&farea,H_FLOAT,T_DOUBLE);
        QFWRITE(offield->fitshead,offield->fitsheadsize,
               offield->file, offield->rfilename);
        fseek(offield->file,arposf,SEEK_SET);
        offield->effarea = farea;
        }
      NPRINTF(OUTPUT, "\n");
      }
    NFPRINTF(OUTPUT, "Closing files...");
    QCALLOC(charpix, char, FBSIZE);
/*-- Pad the written FITS files */
    if (owfield)
      {
      padsize = (FBSIZE -((owfield->npix*sizeof(PIXTYPE))%FBSIZE)) % FBSIZE;
      if (padsize)
        QFWRITE(charpix, padsize, owfield->file, owfield->rfilename);
/*-- Update XML */
      if (prefs.xml_flag)
        update_xml(owfield, ext, "W");
      endfield(owfield);
      }
    if (offield)
      {
      padsize = (FBSIZE -((offield->npix*offield->bytepix)%FBSIZE)) % FBSIZE;
      if (padsize)
        QFWRITE(charpix, padsize, offield->file, offield->rfilename);
/*-- Update XML */
      if (prefs.xml_flag)
        update_xml(offield, ext, "F");
      endfield(offield);

      }
    free(charpix);
    }

/* Processing end date and time */
  thetime2 = time(NULL);
  tm = localtime(&thetime2);
  sprintf(prefs.sdate_end,"%04d-%02d-%02d",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
  sprintf(prefs.stime_end,"%02d:%02d:%02d",
        tm->tm_hour, tm->tm_min, tm->tm_sec);
  prefs.time_diff = difftime(thetime2, thetime);

/* Write XML */

  if (prefs.xml_flag)
    {
    NFPRINTF(OUTPUT, "Writing XML file...");
    write_xml();
    end_xml();
    }


/* Free and close everything */
  for (i=0; i<prefs.nvec_name; i++)
    endvec(vec[i]);
  free(vec);

  return;
  }

/****** write_error ********************************************************
PROTO	void    write_error(char *msg1, char *msg2)
PURPOSE	Manage files in case of a catched error
INPUT	a character string,
        another character string
OUTPUT	RETURN_OK if everything went fine, RETURN_ERROR otherwise.
NOTES	-.
AUTHOR	E. Bertin (IAP)
VERSION	23/02/2007
 ***/
void	write_error(char *msg1, char *msg2)
  {
   char	error[MAXCHAR];

  sprintf(error, "%s%s", msg1,msg2);
  if (prefs.xml_flag)
    write_xmlerror(prefs.xml_name, error);
  end_xml();

  return;
  }
