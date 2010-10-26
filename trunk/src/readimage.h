/*
*				readimage.h
*
* Include file for readimage.c.
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

/*------------------------------ Definitions --------------------------------*/

#define	KBYTE		1024		/* 1 kbyte! */
#define	MBYTE		(1024*KBYTE)	/* 1 Mbyte! */
#define	GBYTE		(1024*MBYTE)	/* 1 Gbyte! */
#define DATA_BUFSIZE	(4*MBYTE)	/* data buffer size for I/O's */

/*-------------------------------- protos -----------------------------------*/
void		readdata(picstruct *field, PIXTYPE *ptr, int size),
		readidata(picstruct *field, WWFLAGTYPE *ptr, int size),
		readimagehead(picstruct *field);

char		*readfitshead(FILE *file, char *filename, int *nblock);
