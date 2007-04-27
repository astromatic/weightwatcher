/*
 				readimage.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	functions for input of image data.
*
*	Last modify:	12/01/2003
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
#include	"fits/fitscat.h"
#include	"readimage.h"

/******************************** readdata **********************************/
/*
read and convert input data stream in PIXTYPE (float) format.
*/
void	readdata(picstruct *field, PIXTYPE *ptr, int size)
  {
  static char	bufdata0[DATA_BUFSIZE];
  char		*bufdata;
  short		val16;
  int		i, bowl, spoonful, npix, curval, dval;
  PIXTYPE	bs,bz;

  bs = (PIXTYPE)field->bscale;
  bz = (PIXTYPE)field->bzero;
  switch(field->compress_type)
    {
/*-- Uncompressed image */
    case ICOMPRESS_NONE:
      bowl = DATA_BUFSIZE/field->bytepix;
      spoonful = size<bowl?size:bowl;
      for(; size>0; size -= spoonful)
        {
        if (spoonful>size)
          spoonful = size;
        bufdata = bufdata0;
        QFREAD(bufdata, spoonful*field->bytepix, field->file, field->filename);
        switch(field->bitpix)
          {
          case BP_BYTE:
            if (field->bitsgn)
              for (i=spoonful; i--;)
                *(ptr++) = *(bufdata++)*bs + bz;
            else
              for (i=spoonful; i--;)
                *(ptr++) = *((unsigned char *)bufdata++)*bs + bz;
            break;

          case BP_SHORT:
            if (bswapflag)
              swapbytes(bufdata, 2, spoonful);
            if (field->bitsgn)
              for (i=spoonful; i--; bufdata += sizeof(short))
                *(ptr++) = *((short *)bufdata)*bs + bz;
            else
              for (i=spoonful; i--; bufdata += sizeof(USHORT))
                *(ptr++) = *((USHORT *)bufdata)*bs + bz;
            break;

          case BP_LONG:
            if (bswapflag)
              swapbytes(bufdata, 4, spoonful);
            if (field->bitsgn)
              for (i=spoonful; i--; bufdata += sizeof(LONG))
                *(ptr++) = *((LONG *)bufdata)*bs + bz;
            else
              for (i=spoonful; i--; bufdata += sizeof(ULONG))
                *(ptr++) = *((ULONG *)bufdata)*bs + bz;
              break;

          case BP_FLOAT:
            if (bswapflag)
              swapbytes(bufdata, 4, spoonful);
            for (i=spoonful; i--; bufdata += sizeof(float))
              *(ptr++) = ((0x7f800000&*(unsigned int *)bufdata) == 0x7f800000)?
			-BIG : *((float *)bufdata)*bs + bz;
              /* *(ptr++) = *((float *)bufdata)*bs + bz; */
            break;

          case BP_DOUBLE:
            if (bswapflag)
	      {
              swapbytes(bufdata, 8, spoonful);
	      for (i=spoonful; i--; bufdata += sizeof(double))
		/* *(ptr++) = *((double *)bufdata)*bs + bz; */
                *(ptr++) = ((0x7ff00000 & *(unsigned int *)(bufdata+4))
			== 0x7ff00000)?
			-BIG : *((double *)bufdata)*bs + bz;
              }
            else
              {
              for (i=spoonful; i--; bufdata += sizeof(double))
                *(ptr++) = ((0x7ff00000 & *(unsigned int *)bufdata)
			== 0x7ff00000)?
			-BIG : *((double *)bufdata)*bs + bz;
	      }
            break;

          default:
            error(EXIT_FAILURE,"*FATAL ERROR*: unknown BITPIX type in ",
				"readdata()");
            break;
          }
        }
      break;

/*-- Compressed image */
    case ICOMPRESS_BASEBYTE:
      bufdata = field->compress_bufptr;
      curval = field->compress_curval;
      npix = field->compress_npix;
      while (size--)
        {
        if (!(npix--))
          {
          if (curval != field->compress_checkval)
            error(EXIT_FAILURE, "*Error*: invalid BASEBYTE checksum in ",
		field->filename);
          bufdata = field->compress_buf;
          QFREAD(bufdata, FBSIZE, field->file, field->filename);
          curval = 0;
          if (bswapflag)
            swapbytes(bufdata, 4, 1);
          field->compress_checkval = *((int *)bufdata);
          bufdata += 4;
         if (bswapflag)
           swapbytes(bufdata, 2, 1);
          npix = (int)(*((short *)bufdata))-1;
          bufdata+=2;
          }
        if ((dval=(int)*(bufdata++))==-128)
          {
          if (bswapflag)
            swapbytes(bufdata, 2, 1);
          memcpy(&val16, bufdata, 2);
          dval = (int)val16;
          if (dval==-32768)
            {
            bufdata += 2;
            if (bswapflag)
              swapbytes(bufdata, 4, 1);
            memcpy(&dval,bufdata,4);
            bufdata += 4;
            }
          else
            bufdata += 2;
          }
        *(ptr++) = dval*bs + bz;
        curval += dval;
        }
      field->compress_curval = curval;
      field->compress_bufptr = bufdata;
      field->compress_npix = npix;
      break;

    case ICOMPRESS_PREVPIX:
      bufdata = field->compress_bufptr;
      curval = field->compress_curval;
      npix = field->compress_npix;
      while (size--)
        {
        if (!(npix--))
          {
          if (curval != field->compress_checkval)
            error(EXIT_FAILURE, "*Error*: invalid PREV_PIX checksum in ",
		field->filename);
          bufdata = field->compress_buf;
          QFREAD(bufdata, FBSIZE, field->file, field->filename);
          if (bswapflag)
            swapbytes(bufdata, 2, 3);
          curval = (int)*(short *)bufdata;
          npix = (int)*(short *)(bufdata+=2)-1;
          field->compress_checkval = (int)(*(short *)(bufdata+=2));
          bufdata+=4;
          }
        if ((dval=(int)*(bufdata++))==-128)
          {
          if (bswapflag)
            swapbytes(bufdata, 2, 1);
          memcpy(&val16, bufdata, 2);
          curval = (int)val16;
          bufdata += 2;
          }
        else
          curval += dval;
        *(ptr++) = curval*bs + bz;
        }
      field->compress_curval = curval;
      field->compress_bufptr = bufdata;
      field->compress_npix = npix;
      break;

    default:
      error(EXIT_FAILURE,"*Internal Error*: unknown compression mode in ",
				"readdata()");
    }

  return;
  }


/******************************** readidata *********************************/
/*
read and convert input data stream in int format.
*/
void	readidata(picstruct *field, FLAGTYPE *ptr, int size)
  {
  static char	bufdata0[DATA_BUFSIZE];
  char		*bufdata;
  short		val16;
  int		i, bowl, spoonful, npix;
  FLAGTYPE	curval, dval;
  switch(field->compress_type)
    {
/*-- Uncompressed image */
    case ICOMPRESS_NONE:
      bowl = DATA_BUFSIZE/field->bytepix;
      spoonful = size<bowl?size:bowl;
      for(; size>0; size -= spoonful)
        {
        if (spoonful>size)
          spoonful = size;
        bufdata = bufdata0;
        QFREAD(bufdata, spoonful*field->bytepix, field->file, field->filename);
        switch(field->bitpix)
          {
          case BP_BYTE:
            if (field->bitsgn)
              for (i=spoonful; i--;)
                *(ptr++) = *(bufdata++);
            else
              for (i=spoonful; i--;)
                *(ptr++) = *((unsigned char *)bufdata++);
            break;

          case BP_SHORT:
            if (bswapflag)
              swapbytes(bufdata, 2, spoonful);
            if (field->bitsgn)
              for (i=spoonful; i--; bufdata += sizeof(short))
                *(ptr++) = *((short *)bufdata);
            else
              for (i=spoonful; i--; bufdata += sizeof(USHORT))
                *(ptr++) = *((USHORT *)bufdata);
            break;

          case BP_LONG:
            if (bswapflag)
              swapbytes(bufdata, 4, spoonful);
            if (field->bitsgn)
              for (i=spoonful; i--; bufdata += sizeof(LONG))
                *(ptr++) = *((LONG *)bufdata);
            else
              for (i=spoonful; i--; bufdata += sizeof(ULONG))
                *(ptr++) = *((ULONG *)bufdata);
              break;

          case BP_FLOAT:
          case BP_DOUBLE:
            error(EXIT_FAILURE,"*Error*: No integer data in ",
				field->filename);
            break;

          default:
            error(EXIT_FAILURE,"*FATAL ERROR*: unknown BITPIX type in ",
				"readdata()");
            break;
          }
        }
      break;

/*-- Compressed image */
    case ICOMPRESS_BASEBYTE:
      bufdata = field->compress_bufptr;
      curval = field->compress_curval;
      npix = field->compress_npix;
      while (size--)
        {
        if (!(npix--))
          {
          if (curval != field->compress_checkval)
            error(EXIT_FAILURE, "*Error*: invalid BASEBYTE checksum in ",
		field->filename);
          bufdata = field->compress_buf;
          QFREAD(bufdata, FBSIZE, field->file, field->filename);
          curval = 0;
          if (bswapflag)
            swapbytes(bufdata, 4, 1);
          field->compress_checkval = *((int *)bufdata);
         bufdata += 4;
         if (bswapflag)
           swapbytes(bufdata, 2, 1);
          npix = (int)(*((short *)bufdata))-1;
          bufdata+=2;
          }
        if ((dval=(int)*(bufdata++))==-128)
          {
          if (bswapflag)
            swapbytes(bufdata, 2, 1);
          memcpy(&val16, bufdata, 2);
          dval = (int)val16;
          if (dval==-32768)
            {
            bufdata += 2;
            if (bswapflag)
              swapbytes(bufdata, 4, 1);
            memcpy(&dval,bufdata,4);
            bufdata += 4;
            }
          else
            bufdata += 2;
          }
        *(ptr++) = dval;
        curval += dval;
        }
      field->compress_curval = curval;
      field->compress_bufptr = bufdata;
      field->compress_npix = npix;
      break;

    case ICOMPRESS_PREVPIX:
      bufdata = field->compress_bufptr;
      curval = field->compress_curval;
      npix = field->compress_npix;
      while (size--)
        {
        if (!(npix--))
          {
          if (curval != field->compress_checkval)
            error(EXIT_FAILURE, "*Error*: invalid PREV_PIX checksum in ",
		field->filename);
          bufdata = field->compress_buf;
          QFREAD(bufdata, FBSIZE, field->file, field->filename);
          if (bswapflag)
            swapbytes(bufdata, 2, 3);
          curval = (int)*(short *)bufdata;
          npix = (int)*(short *)(bufdata+=2)-1;
          field->compress_checkval = (int)(*(short *)(bufdata+=2));
          bufdata+=4;
          }
        if ((dval=(int)*(bufdata++))==-128)
          {
          if (bswapflag)
            swapbytes(bufdata, 2, 1);
          memcpy(&val16, bufdata, 2);
          curval = (int)val16;
          bufdata += 2;
          }
        else
          curval += dval;
        *(ptr++) = curval;
        }
      field->compress_curval = curval;
      field->compress_bufptr = bufdata;
      field->compress_npix = npix;
      break;

    default:
      error(EXIT_FAILURE,"*Internal Error*: unknown compression mode in ",
				"readdata()");
    }

  return;
  }


/******************************* readimagehead *******************************/
/*
extract some data from the FITS-file header
*/
void	readimagehead(picstruct *field)
  {
#define	FITSTOF(k, def) \
			(st[0]=0,((point = fitsnfind(buf, k, n))? \
				 fixexponent(point), \
				atof(strncat(st, &point[10], 70)) \
				:(def)))

#define	FITSTOI(k, def) \
			(st[0]=0,(point = fitsnfind(buf, k, n))? \
				 atoi(strncat(st, &point[10], 70)) \
				:(def))

#define	FITSTOS(k, str, def) \
			{ if ((point = fitsnfind(buf, k, n))) \
				{ \
				for (i=0,point+=11; (*point)!='\'';) \
					str[i++] = *(point++); \
				str[i] = '\0'; \
				} \
			  else\
				strcpy(str, def); \
			}

   int		i, n;
   char		*buf, st[80], *point;

/* Open the file */
  if (!(field->file = fopen(field->filename, "rb")))
    error(EXIT_FAILURE,"*Error*: cannot open ", field->filename);

/* Go directly to the right extension */
  if (field->mefpos)
    {
    QFSEEK(field->file, field->mefpos, SEEK_SET, field->filename);
    }
 
  buf = readfitshead(field->file, field->filename, &n);
  if(FITSTOI("NAXIS   ", 0) < 2)
    error(EXIT_FAILURE, field->filename, " does NOT contain 2D-data!");

/*---------------------------- Basic keywords ------------------------------*/
  field->bitpix = FITSTOI("BITPIX  ", 0);
  if (field->bitpix != BP_BYTE
	&& field->bitpix != BP_SHORT
	&& field->bitpix != BP_LONG
	&& field->bitpix != BP_FLOAT
	&& field->bitpix != BP_DOUBLE)
    error(EXIT_FAILURE, "Sorry, I don't know that kind of data.", "");

  field->bytepix = (field->bitpix>0?field->bitpix:-field->bitpix)>>3;
  field->width = FITSTOI("NAXIS1  ", 0);
  field->height = FITSTOI("NAXIS2  ", 0);
  field->npix = (KINGSIZE_T)field->width*field->height;
  field->bscale = FITSTOF("BSCALE  ", 1.0);

  field->bzero = FITSTOF("BZERO   ", 0.0);
  field->bitsgn = FITSTOI("BITSGN  ", 1);


  FITSTOS("OBJECT  ", field->ident, "Unnamed");

/*----------------------------- Compression --------------------------------*/
  if ((fitsread(buf, "IMAGECOD", st, H_STRING, T_STRING)==RETURN_OK))
    {
    if (!strncmp(st, "NONE", 4))
      field->compress_type = ICOMPRESS_NONE;
    else if (!strncmp(st, "BASEBYTE", 8))
      field->compress_type = ICOMPRESS_BASEBYTE;
    else if (!strncmp(st, "PREV_PIX", 8))
      field->compress_type = ICOMPRESS_PREVPIX;
    else
      warning("Compression skipped: unknown IMAGECOD parameter:", st);
    }


/*---------------------------------------------------------------------------*/

  field->fitshead = buf;
  field->fitsheadsize = n*FBSIZE;

  return;
  }


/******************************* readfitshead ********************************/
/*
read data from the FITS-file header
*/
char    *readfitshead(FILE *file, char *filename, int *nblock)

  {
   int     n;
   char    *buf;

  if (!(buf=(char *)malloc((size_t)FBSIZE)))
    error(EXIT_FAILURE, "*Error*: Not enough memory in ", "readfitshead()");

/* Find the number of FITS blocks of the header while reading it */
  QFREAD(buf, FBSIZE, file, filename);

  if (strncmp(buf, "SIMPLE  ", 8))
    {
/* Ugly but necessary patch to handle this stupid DeNIS compressed format! */
    if (strncmp(buf, "XTENSION", 8))
      error(EXIT_FAILURE, filename, " is NOT a FITS file!");
/*
    else
      {
      memset(buf, ' ', 80);
      strncpy(buf,
	"SIMPLE  =                    T / Decompressed by WeightWatch", 60);
      }
*/
    }

  for (n=1; !fitsnfind(buf,"END     ", n); n++)
    {
    if (!(buf=(char *)realloc(buf, (size_t)(FBSIZE*(n+1)))))
      error(EXIT_FAILURE, "*Error*: Not enough memory in ", "readfitshead()");
    QFREAD(buf+n*FBSIZE, FBSIZE, file, filename);
    }

  *nblock = n;
  return  buf;
  }

