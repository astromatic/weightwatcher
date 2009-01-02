/*
                                xml.c

*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*
*	Part of:	WeightWatcher
*
*	Author:		E.BERTIN (IAP) C.MARMO (IAP)
*
*	Contents:	XML logging.
*
*	Last modify:	05/06/2007
*
*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

#ifdef HAVE_CONFIG_H
#include	"config.h"
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "define.h"
#include "globals.h"
#include "fits/fitscat.h"
#include "key.h"
#include "prefs.h"
#include "field.h"
#include "xml.h"

extern time_t		thetime,thetime2;	/* from makeit.c */
extern pkeystruct	key[];			/* from preflist.h */
extern char		keylist[][32];		/* from preflist.h */
 
xmlstruct		*ww_xml = NULL;
int			nxml, nxmlmax;


/****** init_xml ************************************************************
PROTO	int init_xml(int next)
PURPOSE	Initialize a set of meta-data kept in memory before being written to the
        XML file
INPUT	Number of extensions.
OUTPUT	-.
NOTES	-.
AUTHOR	E. Bertin (IAP) C. Marmo (IAP)
VERSION	27/04/2007
 ***/
int	init_xml(int nxml)
  {
  QCALLOC(ww_xml, xmlstruct, nxml);
  nxmlmax = nxml;
  nxml = 0;

  return EXIT_SUCCESS;
  }


/****** end_xml ************************************************************
PROTO	void end_xml(void)
PURPOSE	Free the set of meta-data kept in memory.
INPUT	-.
OUTPUT	.
NOTES	-.
AUTHOR	E. Bertin (IAP)
VERSION	02/03/2007
 ***/
int	end_xml(void)
  {
  free(ww_xml);

  return EXIT_SUCCESS;
  }


/****** update_xml ***********************************************************
PROTO	int update_xml(picstruct *field, int nfields)
PURPOSE	Update a set of meta-data kept in memory before being written to the
        XML file
INPUT	Pointer to the current field,
        number of fields loaded.
OUTPUT	RETURN_OK if everything went fine, RETURN_ERROR otherwise.
NOTES	Global preferences are used.
AUTHOR	C. Marmo (IAP) E. Bertin (IAP) 
VERSION	27/04/2007
 ***/
int	update_xml(picstruct *field, int next, char *str)
  {

    xmlstruct *x = NULL;

  if (nxml < nxmlmax)
    x = &ww_xml[nxml];

  strcpy(x->fieldname,field->filename);
  strcpy(x->fieldtype, str);
  x->ext = next+1;
  if (prefs.getarea)
    x->effarea = field->effarea;

  nxml++;

  return EXIT_SUCCESS;
  }


/****** write_xml ************************************************************
PROTO	int	write_xml(char *filename)
PURPOSE	Save meta-data to an XML file/stream.
INPUT	XML file name.
OUTPUT	RETURN_OK if everything went fine, RETURN_ERROR otherwise.
NOTES	-.
AUTHOR	C. Marmo (IAP) E. Bertin (IAP)
VERSION	27/04/2007
 ***/
int	write_xml()
  {
   FILE		*file;

  if (!(file = fopen(prefs.xml_name, "w")))
    return RETURN_ERROR;

  write_xml_header(file);
  write_xml_meta(file, (char *)NULL);

  fprintf(file, "</RESOURCE>\n");
  fprintf(file, "</VOTABLE>\n");

  fclose(file);

  return RETURN_OK;
  }


/****** write_xml_header ******************************************************
PROTO	int	write_xml_header(FILE *file)
PURPOSE	Save an XML-VOtable header to an XML file/stream
INPUT	file or stream pointer.
OUTPUT	RETURN_OK if everything went fine, RETURN_ERROR otherwise.
NOTES	Global preferences are used.
AUTHOR	E. Bertin (IAP)
VERSION	06/10/2006
 ***/
int	write_xml_header(FILE *file)
  {

  fprintf(file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
  fprintf(file, "<?xml-stylesheet type=\"text/xsl\" href=\"%s\"?>\n",
        prefs.xsl_name);
  fprintf(file, "<VOTABLE "
        "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
        "xsi:noNamespaceSchemaLocation="
        "\"http://www.ivoa.net/xml/VOTable/v1.1\">\n");
  fprintf(file, "<DESCRIPTION>produced by %s</DESCRIPTION>\n", BANNER);
  fprintf(file, "<!-- VOTable description at "
        "http://www.ivoa.net/Documents/latest/VOT.html -->\n");
  fprintf(file, "<RESOURCE ID=\"%s\" name=\"%s\">\n", BANNER, BANNER);
  fprintf(file, " <DESCRIPTION>Data related to %s"
        "</DESCRIPTION>\n", BANNER);
  fprintf(file, " <INFO name=\"QUERY_STATUS\" value=\"OK\" />\n");

  return RETURN_OK;
  }


/****** write_xml_meta ********************************************************
PROTO	int	write_xml_meta(FILE *file, char *error)
PURPOSE	Save meta-data to an XML-VOTable file or stream
INPUT	Pointer to the output file (or stream),
        Pointer to an error msg (or NULL).
OUTPUT	RETURN_OK if everything went fine, RETURN_ERROR otherwise.
NOTES	-.
AUTHOR	C. Marmo (IAP) E. Bertin (IAP)
VERSION	27/04/2007
 ***/
int	write_xml_meta(FILE *file, char *error)
  {
   xmlstruct		*x;
   struct tm		*tm;
   char			*pspath,*psuser, *pshost, *str;
   int			n;

/* Processing date and time if msg error present */
  if (error)
    {
    thetime2 = time(NULL);
    tm = localtime(&thetime2);
    sprintf(prefs.sdate_end,"%04d-%02d-%02d",
        tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday);
    sprintf(prefs.stime_end,"%02d:%02d:%02d",
        tm->tm_hour, tm->tm_min, tm->tm_sec);
    prefs.time_diff = difftime(thetime2, thetime);
    }

/* Username */
  psuser = pspath = pshost = NULL;
#ifdef HAVE_GETENV
  if (!(psuser=getenv("USERNAME")))	/* Cygwin,... */
    psuser = getenv("LOGNAME");		/* Linux,... */
  pspath = getenv("PWD");
  pshost = getenv("HOSTNAME");
#endif

  fprintf(file, " <RESOURCE ID=\"MetaData\" name=\"MetaData\">\n");
  fprintf(file, "  <DESCRIPTION>%s meta-data</DESCRIPTION>\n", BANNER);
  fprintf(file, "  <INFO name=\"QUERY_STATUS\" value=\"OK\" />\n");
  fprintf(file, "  <PARAM name=\"Software\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta.title;meta.software\" value=\"%s\"/>\n",
        BANNER);
  fprintf(file, "  <PARAM name=\"Version\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta.version;meta.software\" value=\"%s\"/>\n",
        MYVERSION);
  fprintf(file, "  <PARAM name=\"Soft_URL\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta.ref.url;meta.software\" value=\"%s\"/>\n",
        WEBSITE);
  fprintf(file, "  <PARAM name=\"Soft_Auth\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta.bib.author;meta.software\" value=\"%s\"/>\n",
        "Emmanuel Bertin");
  fprintf(file, "  <PARAM name=\"Soft_Ref\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta.bib.bibcode;meta.software\" value=\"%s\"/>\n",
        "2006ASPC..351..112B");
  fprintf(file, "  <PARAM name=\"NThreads\" datatype=\"int\""
        " ucd=\"meta.number;meta.software\" value=\"%d\"/>\n",
            prefs.nthreads);
  fprintf(file, "  <PARAM name=\"Date\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"time.event.end;meta.software\" value=\"%s\"/>\n",
        prefs.sdate_end);
  fprintf(file, "  <PARAM name=\"Time\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"time.event.end;meta.software\" value=\"%s\"/>\n",
        prefs.stime_end);
  fprintf(file, "  <PARAM name=\"Duration\" datatype=\"float\""
        " ucd=\"time.event;meta.software\" value=\"%.0f\" unit=\"s\"/>\n",
        prefs.time_diff);

  fprintf(file, "  <PARAM name=\"User\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta.curation\" value=\"%s\"/>\n",
        psuser);
  fprintf(file, "  <PARAM name=\"Host\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta.curation\" value=\"%s\"/>\n",
        pshost);
  fprintf(file, "  <PARAM name=\"Path\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta.dataset\" value=\"%s\"/>\n",
        pspath);


  if (error)
    {
    fprintf(file, "\n  <!-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        "!!!!!!!!!!!!!!!!!!!! -->\n");
    fprintf(file, "  <!-- !!!!!!!!!!!!!!!!!!!!!! an Error occured"
        " !!!!!!!!!!!!!!!!!!!!! -->\n");
    fprintf(file, "  <!-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        "!!!!!!!!!!!!!!!!!!!! -->\n");
    fprintf(file,"  <PARAM name=\"Error_Msg\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta\" value=\"%s\"/>\n", error);
    fprintf(file, "  <!-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        "!!!!!!!!!!!!!!!!!!!! -->\n");
    fprintf(file, "  <!-- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
        "!!!!!!!!!!!!!!!!!!!! -->\n\n");
    }

/* Meta-data for the ww processing */

  fprintf(file, "  <TABLE ID=\"OutFields\" name=\"OutFields\">\n");
  fprintf(file, "   <DESCRIPTION>Metadata about the output images gathered by "
        "%s</DESCRIPTION>\n", BANNER);
  fprintf(file, "   <!-- Nextensions may be 0"
        " if an error occurred early in the processing -->\n");

  fprintf(file, "   <PARAM name=\"NExtensions\" datatype=\"int\""
        " ucd=\"meta.number;meta.dataset\" value=\"%d\"/>\n", nxml);

  fprintf(file, "   <FIELD name=\"Output_Image_Name\" datatype=\"char\""
        " ucd=\"meta.id;meta.file;meta.fits\"/>\n");
  fprintf(file, "   <FIELD name=\"Output_Image_Type\" datatype=\"char\""
        " ucd=\"meta;meta.code.class\"/>\n");
  fprintf(file, "   <FIELD name=\"Extension\" datatype=\"int\""
        " ucd=\"meta.number;meta.dataset\"/>\n");
  fprintf(file, "   <FIELD name=\"Effective_Area\" datatype=\"float\""
        " ucd=\"phys.size;arith.ratio\"/>\n");

  fprintf(file, "   <DATA><TABLEDATA>\n");
  for (n=0; n<nxml; n++)
    {
    x = &ww_xml[n];
    fprintf(file, "    <TR>\n"
        "     <TD>%s</TD><TD>%s</TD><TD>%d</TD><TD>%.8g</TD>\n",
        x->fieldname,
        x->fieldtype,
        x->ext,
        prefs.getarea? x->effarea : 9999);
    fprintf(file, "    </TR>\n");
    }
  fprintf(file, "   </TABLEDATA></DATA>\n");
  fprintf(file, "  </TABLE>\n");

/* Warnings */
  fprintf(file, "  <TABLE ID=\"Warnings\" name=\"Warnings\">\n");
  fprintf(file,
        "   <DESCRIPTION>%s warnings (limited to the last %d)</DESCRIPTION>\n",
        BANNER, WARNING_NMAX);
  fprintf(file, "   <FIELD name=\"Date\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta;time.event.end\"/>\n");
  fprintf(file, "   <FIELD name=\"Time\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta;time.event.end\"/>\n");
  fprintf(file, "   <FIELD name=\"Msg\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"meta\"/>\n");
  fprintf(file, "   <DATA><TABLEDATA>\n");
  for (str = warning_history(); *str; str = warning_history())
    fprintf(file, "    <TR><TD>%10.10s</TD><TD>%8.8s</TD><TD>%s</TD></TR>\n",
        str, str+11, str+22);
  fprintf(file, "   </TABLEDATA></DATA>\n");

  fprintf(file, "  </TABLE>\n");

/* Configuration file */

  fprintf(file, "  <RESOURCE ID=\"Config\" name=\"Config\">\n");
  fprintf(file, "   <DESCRIPTION>%s configuration</DESCRIPTION>\n", BANNER);
  fprintf(file,
        "   <PARAM name=\"Command_Line\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"obs.param\" value=\"%s",
        prefs.command_line[0]);
  for (n=1; n<prefs.ncommand_line; n++)
    fprintf(file, " %s", prefs.command_line[n]);
  fprintf(file, "\"/>\n");
  fprintf(file,
        "   <PARAM name=\"Prefs_Name\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"obs.param;meta.file\" value=\"%s\"/>\n",
        prefs.prefs_name);


  if (!error)
    {
/*-- Input Weight Maps */

    write_xmlconfigparam(file, "Weight_Names", "",
                "meta.id;meta.fits;obs.image","%s");
    write_xmlconfigparam(file, "Weight_Min", "counts",
                "meta;obs.param","%3.1e");
    write_xmlconfigparam(file, "Weight_Max", "counts",
                "meta;obs.param","%3.1e");
    write_xmlconfigparam(file, "Weight_OutFlags", "",
                "meta.code.class","%d");

/*-- Input Flag Maps */

    write_xmlconfigparam(file, "Flag_Names", "",
                "meta.id;meta.fits;obs.image","%s");
    write_xmlconfigparam(file, "Flag_Wmasks", "",
                "meta;obs.param","%d");
    write_xmlconfigparam(file, "Flag_Masks", "",
                "meta;obs.param","%d");
    write_xmlconfigparam(file, "Flag_Outflags", "",
                "meta.code.class","%d");

/*-- Input Polygons Files */

    write_xmlconfigparam(file, "Poly_Names", "",
                "meta.id;meta.file", "%s");
    write_xmlconfigparam(file, "Poly_Outflags", "",
                "meta.code.class", "%d");
    write_xmlconfigparam(file, "Poly_Outweights", "",
                "meta.code.class", "%d");
    write_xmlconfigparam(file, "Poly_Intersect", "",
                "meta;meta.code", "%s");

/*-- Outputs */

    write_xmlconfigparam(file, "Outweight_Name", "",
                "meta.id;meta.fits","%s");
    write_xmlconfigparam(file, "Outflag_Name", "",
                "meta.id;meta.fits","%s");


/*-- Miscellaneous */

    write_xmlconfigparam(file, "Getarea", "", "meta.code","%s");
    write_xmlconfigparam(file, "Getarea_Weight", "", "meta;obs.param","%3.1f");
    write_xmlconfigparam(file, "Getarea_Flags", "",
                "meta;obs.param", "%d");
    write_xmlconfigparam(file, "Memory_Bufsize", "", "meta.cryptic","%d");
    write_xmlconfigparam(file, "Verbose_Type", "", "meta.code","%s");
    write_xmlconfigparam(file, "Write_XML", "", "meta.code","%s");
    write_xmlconfigparam(file, "NThreads", "",
                "meta.number;meta.software", "%d");

    }

  fprintf(file, "  </RESOURCE>\n");

  fprintf(file, " </RESOURCE>\n");

  return RETURN_OK;
  }


/****** write_xmlerror ******************************************************
PROTO	int	write_xmlerror(char *error)
PURPOSE	Save meta-data to a simplified XML file in case of a catched error
INPUT	a character string.
OUTPUT	RETURN_OK if everything went fine, RETURN_ERROR otherwise.
NOTES	-.
AUTHOR	E. Bertin (IAP)
VERSION	26/07/2006
 ***/
void	write_xmlerror(char *filename, char *error)
  {
   FILE			*file;
   int			pipe_flag;

  pipe_flag = 0;
  if (!strcmp(filename, "STDOUT"))
    {
    file = stdout;
    pipe_flag = 1;
    }
  else if (!(file = fopen(filename, "w")))
    return;

  write_xml_header(file);
  write_xml_meta(file, error);

  fprintf(file, "</RESOURCE>\n");
  fprintf(file, "</VOTABLE>\n");

  if (!pipe_flag)
    fclose(file);

  return;
  }


/****** write_xmlconfigparam **************************************************
PROTO	int write_xmlconfigparam(FILE *file, char *name, char *unit,
                char *ucd, char *format)
PURPOSE	Write to a VO-table the configuration parameters.
INPUT	Output stream (file) pointer,
        Name of the parameter keyword,
        unit,
        UCD string,
        printf() format to use in "value".
OUTPUT	RETURN_OK if the keyword exists, RETURN_ERROR otherwise.
NOTES	-.
AUTHOR	E. Bertin (IAP)
VERSION	06/10/2006
 ***/
int	write_xmlconfigparam(FILE *file, char *name, char *unit,
                 char *ucd, char *format)
  {
   char		value[MAXCHAR], uunit[MAXCHAR];
   int		i,j,n;

  for (i=0; key[i].name[0] && cistrcmp(name, key[i].name, FIND_STRICT); i++);
  if (!key[i].name[0])
    return RETURN_ERROR;

  if (*unit)
    sprintf(uunit, " unit=\"%s\"", unit);
  else
    *uunit = '\0';
  switch(key[i].type)
    {
    case P_FLOAT:
      sprintf(value, format, *((double *)key[i].ptr));
      fprintf(file, "   <PARAM name=\"%s\"%s datatype=\"double\""
        " ucd=\"%s\" value=\"%s\"/>\n",
        name, uunit, ucd, value);
      break;
    case P_FLOATLIST:
      n = *(key[i].nlistptr);
      if (n)
        {
        sprintf(value, format, ((double *)key[i].ptr)[0]);
        fprintf(file, "   <PARAM name=\"%s\"%s datatype=\"double\""
                " arraysize=\"%d\" ucd=\"%s\" value=\"%s",
                name, uunit, n, ucd, value);
        for (j=1; j<n; j++)
          {
          sprintf(value, format, ((double *)key[i].ptr)[j]);
          fprintf(file, " %s", value);
          }
        fprintf(file, "\"/>\n");
        }
      else
        fprintf(file, "   <PARAM name=\"%s\"%s datatype=\"double\""
                " ucd=\"%s\" value=\"\"/>\n",
                name, uunit, ucd);
      break;
    case P_INT:
      sprintf(value, format, *((int *)key[i].ptr));
      fprintf(file, "   <PARAM name=\"%s\"%s datatype=\"int\""
        " ucd=\"%s\" value=\"%s\"/>\n",
        name, uunit, ucd, value);
      break;
    case P_INTLIST:
      n = *(key[i].nlistptr);
      if (n)
        {
        sprintf(value, format, ((int *)key[i].ptr)[0]);
        fprintf(file, "   <PARAM name=\"%s\"%s datatype=\"int\""
                " arraysize=\"%d\" ucd=\"%s\" value=\"%s",
                name, uunit, n, ucd, value);
        for (j=1; j<n; j++)
          {
          sprintf(value, format, ((int *)key[i].ptr)[j]);
          fprintf(file, " %s", value);
          }
        fprintf(file, "\"/>\n");
        }
      else
        fprintf(file, "   <PARAM name=\"%s\"%s datatype=\"double\""
                " ucd=\"%s\" value=\"\"/>\n",
                name, uunit, ucd);
      break;
    case P_BOOL:
      sprintf(value, "%c", *((int *)key[i].ptr)? 'T':'F');
      fprintf(file, "   <PARAM name=\"%s\" datatype=\"boolean\""
        " ucd=\"%s\" value=\"%s\"/>\n",
        name, ucd, value);
      break;
    case P_BOOLLIST:
      n = *(key[i].nlistptr);
      if (n)
        {
        sprintf(value, "%c", ((int *)key[i].ptr)[0]? 'T':'F');
        fprintf(file, "   <PARAM name=\"%s\" datatype=\"boolean\""
                " arraysize=\"%d\" ucd=\"%s\" value=\"%s",
                name, n, ucd, value);
        for (j=1; j<n; j++)
          {
          sprintf(value, "%c", ((int *)key[i].ptr)[j]? 'T':'F');
          fprintf(file, " %s", value);
          }
        fprintf(file, "\"/>\n");
        }
      else
        fprintf(file, "   <PARAM name=\"%s\" datatype=\"boolean\""
                " ucd=\"%s\" value=\"\"/>\n",
                name, ucd);
      break;
    case P_STRING:
      sprintf(value, "%s", (char *)key[i].ptr);
      fprintf(file, "   <PARAM name=\"%s\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"%s\" value=\"%s\"/>\n",
        name, ucd, value);
      break;
    case P_STRINGLIST:
      n = *(key[i].nlistptr);
      if (n)
        {
        sprintf(value, ((char **)key[i].ptr)[0]);
        fprintf(file, "   <PARAM name=\"%s\" datatype=\"char\""
                " arraysize=\"*\" ucd=\"%s\" value=\"%s",
                name, ucd, value);
        for (j=1; j<n; j++)
          {
          sprintf(value, "%s", ((char **)key[i].ptr)[j]);
          fprintf(file, ",%s", value);
          }
        fprintf(file, "\"/>\n");
        }
      else
        fprintf(file, "   <PARAM name=\"%s\" datatype=\"char\""
                " arraysize=\"*\" ucd=\"%s\" value=\"\"/>\n",
                name, ucd);
      break;
    case P_KEY:
      sprintf(value, key[i].keylist[*((int *)key[i].ptr)]);
      fprintf(file, "   <PARAM name=\"%s\" datatype=\"char\" arraysize=\"*\""
        " ucd=\"%s\" value=\"%s\"/>\n",
        name, ucd, value);
      break;
    case P_KEYLIST:
      n = *(key[i].nlistptr);
      if (n)
        {
        sprintf(value, key[i].keylist[((int *)key[i].ptr)[0]]);
        fprintf(file, "   <PARAM name=\"%s\" datatype=\"char\""
                " arraysize=\"*\" ucd=\"%s\" value=\"%s",
                name, ucd, value);
        for (j=1; j<n; j++)
          {
          sprintf(value, key[i].keylist[((int *)key[i].ptr)[j]]);
          fprintf(file, ",%s", value);
          }
        fprintf(file, "\"/>\n");
        }
      else
        fprintf(file, "   <PARAM name=\"%s\" datatype=\"char\""
                " arraysize=\"*\" ucd=\"%s\" value=\"\"/>\n",
                name, ucd);
      break;
    default:
        error(EXIT_FAILURE, "*Internal Error*: Type Unknown",
                " in write_xmlconfigparam()");
    }

  return RETURN_OK;
  }
