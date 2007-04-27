/*
                                 xml.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP) C.MARMO (IAP)
*
*	Contents:	XML logging.
*
*	Last modify:	02/03/2007
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

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
  char         fieldname[80];               /* name of the file */
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
