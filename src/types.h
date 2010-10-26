/*
*				types,h
*
* Global type definitions.
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	This file part of:	WeightWatcher
*
*	Copyright:		(C) 1997-2010 Emmanuel Bertin -- IAP/CNRS/UPMC
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

#include <stdio.h>

#ifndef _FITSCAT_H_
#include "fits/fitscat.h"
#endif

/*-------------------------------- flags ------------------------------------*/

/*--------------------------------- typedefs --------------------------------*/
typedef	unsigned char	BYTE;			/* a byte */
typedef	int		WWFLAGTYPE;		/* FLAG pixel type */
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
  WWFLAGTYPE	wmask;			/* threshold detection mask */
/*---- FLAG-stuff */
  WWFLAGTYPE	ofmask[32];		/* output mask (max 1 per bit) */
  WWFLAGTYPE	fmask;			/* FLAG detection mask */
  WWFLAGTYPE	fmask2[32];		/* Positions of active FLAG bits */
  int		nfmask;			/* number of active FLAG bits */
  OFF_T		mefpos;			/* Position in a MEF file */
  }	picstruct;


