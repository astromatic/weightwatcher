 /*
 				field.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP)
*
*	Contents:	Handling of field structures.
*
*	Last modify:	12/01/2003
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/


/*------------------------------ field flags --------------------------------*/

#define		WEIGHT_FIELD	0x01	/* Weights */
#define		FLAG_FIELD	0x02	/* Flagging */

/*-------------------------------- protos -----------------------------------*/
void		endfield(picstruct *field);

picstruct	*newfield(char *filename, int flags, picstruct *mfield,
			int ext, int next);
