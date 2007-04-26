 /*
 				globals.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP) C. MARMO (IAP)
*
*	Contents:	Global declarations.
*
*	Last modify:	26/04/2007
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include "types.h"

/*----------------------- miscellaneous variables ---------------------------*/
char		gstr[MAXCHAR];

/*------------------------------- functions ---------------------------------*/
extern	void	makeit(void),
                write_error(char *msg1, char *msg2);
