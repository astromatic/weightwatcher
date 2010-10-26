/*
*				xml.h
*
* Include file for xml.c.
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	This file part of:	WeightWatcher
*
*	Copyright:		(C) 2007-2010 Emmanuel Bertin -- IAP/CNRS/UPMC
*				& Chiara Marmo -- IAP/CNRS
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

#ifdef HAVE_CONFIG_H
#include	"config.h"
#endif


#ifndef _FIELD_H_
#include "field.h"
#endif

/*----------------------------- Internal constants --------------------------*/
#ifndef XSL_URL
#define	XSL_URL	"."
#endif
/*--------------------------------- typedefs --------------------------------*/
typedef struct
  {
  char         fieldname[MAXCHAR];          /* name of the file */
  char         fieldtype[2];                /* F flag, W weight */
  int          ext;                         /* extension */
  float        effarea;                     /* effective surface */
  }	xmlstruct;
/*------------------------------- functions ---------------------------------*/

extern int	init_xml(int next),
                update_xml(picstruct *field, int next, char *str),
                write_xml(),
                write_xml_header(FILE *file),
                write_xml_meta(FILE *file, char *error),
                write_xmlconfigparam(FILE *file, char *name, char *unit,
                                char *ucd, char *format),
                end_xml(void);
extern void	write_xmlerror(char *filename, char *error);
