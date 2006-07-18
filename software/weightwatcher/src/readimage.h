 /*
 				readimage.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	Include for readimage.c
*
*	Last modify:	12/01/2003
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/*------------------------------ Definitions --------------------------------*/

#define	KBYTE		1024		/* 1 kbyte! */
#define	MBYTE		(1024*KBYTE)	/* 1 Mbyte! */
#define	GBYTE		(1024*MBYTE)	/* 1 Gbyte! */
#define DATA_BUFSIZE	(4*MBYTE)	/* data buffer size for I/O's */

/*-------------------------------- protos -----------------------------------*/
void		readdata(picstruct *field, PIXTYPE *ptr, int size),
		readidata(picstruct *field, FLAGTYPE *ptr, int size),
		readimagehead(picstruct *field);

char		*readfitshead(FILE *file, char *filename, int *nblock);
