 /*
 				preflist.h

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP, Leiden observatory & ESO) C. MARMO (IAP)
*
*	Contents:	Keywords for the configuration file.
*
*	Last modify:	03/05/2007
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#include "key.h"

#ifdef  USE_THREADS
#define	THREADS_PREFMAX	THREADS_NMAX
#else
#define	THREADS_PREFMAX	65535
#endif

/*-------------------------------- initialization ---------------------------*/
int	idummy;

pkeystruct key[] =
 {
  {"WEIGHT_NAMES", P_STRINGLIST, prefs.weight_name, 0,0,0.0,0.0,
    {""},0,MAXLIST, &prefs.nweight_name},
  {"WEIGHT_MIN", P_FLOATLIST, prefs.weight_threshd, 0,0,-BIG,BIG,
    {""}, 0, MAXLIST, &prefs.nweight_threshd},
  {"WEIGHT_MAX", P_FLOATLIST, prefs.weight_threshu, 0,0,-BIG,BIG,
    {""}, 0, MAXLIST, &prefs.nweight_threshu},
  {"WEIGHT_OUTFLAGS", P_INTLIST, prefs.weight_mask, 0,1<<30,0.0,0.0,
    {""}, 0, MAXLIST, &prefs.nweight_mask},
  {"FLAG_NAMES", P_STRINGLIST, prefs.flag_name, 0,0,0.0,0.0,
    {""},0,MAXLIST, &prefs.nflag_name},
  {"FLAG_WMASKS", P_INTLIST, prefs.flag_wmask, 0,1<<30,0.0,0.0,
    {""}, 0, MAXLIST, &prefs.nflag_wmask},
  {"FLAG_MASKS", P_INTLIST, prefs.flag_fmask, 0,1<<30,0.0,0.0,
    {""}, 0, MAXLIST, &prefs.nflag_fmask},
  {"FLAG_OUTFLAGS", P_INTLIST, prefs.flag_ofmask, 0,1<<30,0.0,0.0,
    {""}, 0, MAXLIST, &prefs.nflag_ofmask},
  {"POLY_NAMES", P_STRINGLIST, prefs.vec_name, 0,0,0.0,0.0,
    {""},0,MAXLIST, &prefs.nvec_name},
  {"POLY_OUTFLAGS", P_INTLIST, prefs.vec_mask, 0,1<<30,0.0,0.0,
    {""}, 0, MAXLIST, &prefs.nvec_mask},
  {"POLY_OUTWEIGHTS", P_FLOATLIST, prefs.vec_weight, 0,0,0.0,BIG,
    {""}, 0, MAXLIST, &prefs.nvec_weight},
  {"POLY_INTERSECT", P_BOOL, &prefs.intersec, 0, 0,0.0,0.0,
    {""}, 1, 1, &idummy},
  {"OUTWEIGHT_NAME", P_STRINGLIST, &prefs.oweight_name, 0,0,0.0,0.0,
    {""},0,1, &idummy},
  {"OUTFLAG_NAME", P_STRINGLIST, &prefs.oflag_name, 0,0,0.0,0.0,
    {""},0,1, &idummy},
  {"GETAREA", P_BOOL , &prefs.getarea, 0, 0, 0.0, 0.0,
    {""}, 1, 1, &idummy},
  {"GETAREA_WEIGHT", P_FLOAT , &prefs.weightlim, 0,0, 0.0, BIG, 
    {""}, 0, 1, &idummy},
  {"GETAREA_FLAGS", P_INTLIST , prefs.geta_flags, 0,1<<30 , 0.0, 0.0,
    {""}, 0, MAXLIST, &prefs.ngeta_flags},
  {"MEMORY_BUFSIZE", P_INT, &prefs.mem_bufsize, 1,1<<30},
  {"VERBOSE_TYPE", P_KEY, &prefs.verbose_type, 0,0, 0.0,0.0,
   {"QUIET","NORMAL","FULL",""}},
  {"XML_NAME", P_STRING, prefs.xml_name},
  {"XSL_URL", P_STRING, prefs.xsl_name},
  {"WRITE_XML", P_BOOL, &prefs.xml_flag},
  {"NTHREADS", P_INT, &prefs.nthreads, 0, THREADS_PREFMAX},
  {""}
 };
 
char			keylist[sizeof(key)/sizeof(pkeystruct)][32];
static const char	notokstr[] = {" \t=,;\n\r\""};

char *default_prefs[] =
 {
"# Default configuration file for " BANNER " " MYVERSION,
"# EB CM " DATE,
"#",
" ",
"#--------------------------------- Weights ------------------------------------",
" ",
"WEIGHT_NAMES    weightin.fits   # Filename(s) of the input WEIGHT map(s)",
" ",
"WEIGHT_MIN      0.3             # Pixel below those thresholds will be flagged",
"WEIGHT_MAX      1.30            # Pixels above those thresholds will be flagged",
"WEIGHT_OUTFLAGS 1               # FLAG values for thresholded pixels",
" ",
"#---------------------------------- Flags -------------------------------------",
" ",
"FLAG_NAMES      flagin.fits     # Filename(s) of the input FLAG map(s)",
" ",
"FLAG_WMASKS     0xff            # Bits which will nullify the WEIGHT-map pixels",
"FLAG_MASKS      0x01            # Bits which will be converted as output FLAGs",
"FLAG_OUTFLAGS   2               # Translation of the FLAG_MASKS bits",
" ",
"#---------------------------------- Polygons ----------------------------------",
" ",
"POLY_NAMES      \"\"              # Filename(s) of input DS9 regions",
"POLY_OUTFLAGS                   # FLAG values for polygon masks",
"POLY_OUTWEIGHTS 0.0             # Weight values for polygon masks",
"POLY_INTERSECT  Y               # Use inclusive OR for polygon intersects (Y/N)?",
" ",
"#---------------------------------- Output ------------------------------------",
" ",
"OUTWEIGHT_NAME  weight.fits     # Output WEIGHT-map filename",
"OUTFLAG_NAME    flag.fits       # Output FLAG-map filename",
" ",
"#----------------------------- Miscellaneous ---------------------------------",
" ",
"GETAREA         N               # Compute area for flags and weights (Y/N)?",
"GETAREA_WEIGHT  0.0             # Weight threshold for area computation", 
"GETAREA_FLAGS   1               # Bit mask for flag pixels not counted in area", 
"MEMORY_BUFSIZE  256             # Buffer size in lines",
"VERBOSE_TYPE    NORMAL          # can be QUIET, NORMAL or FULL",
"WRITE_XML       Y               # Write XML file (Y/N)?",
"XML_NAME        ww.xml          # Filename for XML output",
"*XSL_URL         " XSL_URL,
"*                                # Filename for XSL style-sheet",
#ifdef USE_THREADS
"NTHREADS        0               # Number of simultaneous threads for",
"                                # the SMP version of " BANNER,
"                                # 0 = automatic",
#else
"NTHREADS        1               # 1 single thread",
#endif
" ",
""
};

