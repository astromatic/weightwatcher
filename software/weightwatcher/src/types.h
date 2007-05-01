 /*
 				types.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP, Leiden observatory & ESO)
*
*	Contents:	global type definitions.
*
*	Last modify:	12/01/2003
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include <stdio.h>

#ifndef _FITSCAT_H_
#include "fits/fitscat.h"
#endif

/*-------------------------------- flags ------------------------------------*/

/*--------------------------------- typedefs --------------------------------*/
typedef	unsigned char	BYTE;			/* a byte */
typedef	int		FLAGTYPE;		/* FLAG pixel type */
typedef	unsigned short	USHORT;			/* 0 to 65535 integers */

typedef	int		LONG;
typedef	unsigned int	ULONG;

/*----------------------------- image parameters ----------------------------*/
typedef struct
  {
  char		filename[MAXCHAR];	/* pointer to the image filename */
  char		*rfilename;		/* pointer to the reduced image name */
  char		ident[MAXCHAR];		/* field identifier (read from FITS)*/
  char		rident[MAXCHAR];	/* field identifier (relative) */
  FILE		*file;			/* pointer the image file structure */
  char		*fitshead;		/* pointer to the FITS header */
  int		fitsheadsize;		/* FITS header size */
/* ---- main image parameters */
  int		bitpix, bytepix;	/* nb of bits and bytes per pixel */
  int		bitsgn;			/* non-zero if signed integer data */
  unsigned long	width, height;		/* x,y size of the field */
  KINGSIZE_T	npix;			/* total number of pixels */
  double	bscale, bzero;		/* FITS scale and offset */
  void		*strip;			/* pointer to the image buffer */
  int		stripheight;		/* height  of a strip (in lines) */
  double        effarea;                /* output effective area (if activated) */
/* ---- image (de-)compression */
  enum {ICOMPRESS_NONE, ICOMPRESS_BASEBYTE, ICOMPRESS_PREVPIX}
		compress_type;		/* image compression type */
  char		*compress_buf;		/* de-compression buffer */
  char		*compress_bufptr;	/* present pixel in buffer */
  int		compress_curval;	/* current pixel or checksum value */
  int		compress_checkval;	/* foreseen pixel or checksum value */
  int		compress_npix;		/* remaining pixels in buffer */
  int		flags;			/* flags defining the field type */
/*---- WEIGHT-stuff */
  PIXTYPE	threshd, threshu;	/* lower and upper thresholds */
  FLAGTYPE	wmask;			/* threshold detection mask */
/*---- FLAG-stuff */
  FLAGTYPE	ofmask[32];		/* output mask (max 1 per bit) */
  FLAGTYPE	fmask;			/* FLAG detection mask */
  FLAGTYPE	fmask2[32];		/* Positions of active FLAG bits */
  int		nfmask;			/* number of active FLAG bits */
  OFF_T		mefpos;			/* Position in a MEF file */
  }	picstruct;


