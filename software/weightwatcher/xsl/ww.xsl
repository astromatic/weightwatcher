<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xsl:stylesheet [
        <!ENTITY nbsp "&#160;">
        <!ENTITY deg "&#176;">
        <!ENTITY amin "&#180;">
        <!ENTITY asec "&#168;">
        ]>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<!-- *********************** Global XSL template ************************** -->
 <xsl:template match="/">
  <xsl:variable name="date" select="/VOTABLE/RESOURCE/RESOURCE[@name='MetaData']/PARAM[@name='Date']/@value"/>
  <xsl:variable name="time" select="/VOTABLE/RESOURCE/RESOURCE[@name='MetaData']/PARAM[@name='Time']/@value"/>
  <html>

<!-- HTML head  -->

   <head>

<!-- javascript -->

<!--  <script type="text/javascript" language="javascript"> -->

    <script src="http://terapix.iap.fr/cplt/xsl/sorttable.js"/>

    <style type="text/css">
     p.sansserif {font-family: sans-serif}
     body {background-color: white}
     mono {font-family: monospace}
     elen {font-family: monospace; font-size: 100%; font-weight: bold; color: green }
     elep {font-family: monospace; font-size: 100%; font-weight: bold; color: red }
     el {font-family: monospace; font-size: 100%; color: black}
     a {text-decoration: none}
     table.sortable a.sortheader
      {
      background-color:#FFEECC;
      color: black;
      font-weight: bold;
      font-size: 80%;
      text-decoration: none;
      display: button;
      }
     table.sortable span.sortarrow
      {
      color: black;
      font-weight: bold;
      text-decoration: none;
      }
     table.sortable a.sortheader.sub
      {
      vertical-align: sub;
      }
     </style>

     <title>
      Processing summary on <xsl:value-of select="$date"/> at <xsl:value-of select="$time"/>
     </title>
    </head>

<!-- HTML body -->

    <BODY>
     <TABLE BORDER="0" CELLPADDING="0" CELLSPACING="0" WIDTH="100%">
      <TR>
       <TD ALIGN="LEFT">
        <TABLE BORDER="0">
         <TR>
          <TD ALIGN="CENTER">
           <IMG SRC="http://terapix.iap.fr/cplt/xsl/terapixLogo.png" ALT="Terapix"/>
          </TD>
          <TD ALIGN="CENTER">
           <IMG SRC="http://terapix.iap.fr/cplt/xsl/terapixTitle.png" ALT="Logo"/>
          </TD>
          <TD ALIGN="CENTER">
           <FONT color="#669933">
            <B> Processing summary</B>
           </FONT>
          </TD>
          <TD ALIGN="CENTER">
           <IMG SRC="http://terapix.iap.fr/cplt/xsl/terapixPicture.gif" ALT="Terapix banner"/>
          </TD>
         </TR>
        </TABLE>
       </TD>
      </TR>
      <TR>
       <TD>
        <TABLE BORDER="0" WIDTH="100%" BGCOLOR="#000000">
         <TR>
          <TH BGCOLOR="#000000" ALIGN="LEFT"><FONT SIZE="-1" COLOR="#FFFFFF"> Home > Tools > Data reduction</FONT></TH>
         </TR>
        </TABLE>
       </TD>
      </TR>
     </TABLE>
    <xsl:call-template name="VOTable"/>
   </BODY>
  </html>
 </xsl:template>

<!-- **************** Generic XSL template for VOTables ****************** -->
 <xsl:template name="VOTable">
  <xsl:for-each select="/VOTABLE">
   <xsl:call-template name="Resource"/>
  </xsl:for-each>
 </xsl:template>

<!-- *************** Generic XSL template for Resources ****************** -->
 <xsl:template name="Resource">
  <xsl:for-each select="RESOURCE">
   <xsl:choose>
    <xsl:when test="@ID='WeightWatcher'">
     <xsl:call-template name="ww"/>
    </xsl:when>
   </xsl:choose>
  </xsl:for-each>
 </xsl:template>

<!-- ********************** XSL template for WeightWatcher ************************ -->
 <xsl:template name="ww">
  <xsl:for-each select="RESOURCE[@ID='MetaData']">
   <xsl:call-template name="RunInfo"/>
   <xsl:for-each select="TABLE[@ID='OutFields']">
    <xsl:call-template name="outfields"/>
   </xsl:for-each>
   <xsl:for-each select="RESOURCE[@ID='Config']">
    <xsl:call-template name="Config"/>
   </xsl:for-each>
   <xsl:for-each select="TABLE[@ID='Warnings']">
    <xsl:call-template name="Warnings"/>
   </xsl:for-each>
  </xsl:for-each>
 </xsl:template>

<!-- ************* Generic XSL RunInfo template for MetaData ************* -->
 <xsl:template name="RunInfo">
  <p>
<!-- Software name, version, date, time and number of threads -->
   <a>
    <xsl:attribute name="href">
     <xsl:value-of select="PARAM[@name='Soft_URL']/@value"/>
    </xsl:attribute>
    <b>
     <xsl:value-of select="PARAM[@name='Software']/@value"/>&nbsp;<xsl:value-of select="PARAM[@name='Version']/@value"/>
    </b>
   </a>
   started on
   <b><xsl:value-of select="PARAM[@name='Date']/@value"/></b>
   at
   <b><xsl:value-of select="PARAM[@name='Time']/@value"/></b>
   with
   <b><xsl:value-of select="PARAM[@name='NThreads']/@value"/></b>
   thread<xsl:if test="PARAM[@name='NThreads']/@value &gt; 1">s</xsl:if>

<!-- Run time -->
   <xsl:variable name="duration" select="PARAM[@name='Duration']/@value"/>
   (run time:
    <b>
     <xsl:choose> 
      <xsl:when test="$duration &gt; 3600.0">
       <xsl:value-of
        select='concat(string(floor($duration div 3600)),
        " h ", format-number(floor(($duration div 60) mod 60.0), "00"),
        " min")'/>
      </xsl:when>
      <xsl:otherwise>
       <xsl:choose>
        <xsl:when test="$duration &gt; 60.0">
         <xsl:value-of
          select='concat(format-number(floor($duration div 60),"##"),
          " min ", format-number(floor($duration mod 60.0), "00")," s")'/>
        </xsl:when>
        <xsl:otherwise>
         <xsl:value-of select='concat(string($duration), " s")'/>
        </xsl:otherwise>
       </xsl:choose>
      </xsl:otherwise>
     </xsl:choose>
    </b>)
    <br />
   by user <b><xsl:value-of select="PARAM[@name='User']/@value"/></b>
   from <b><xsl:value-of select="PARAM[@name='Host']/@value"/></b>
   in <b><mono><xsl:value-of select="PARAM[@name='Path']/@value"/></mono></b>
  </p>
  <p>
   <b style="color: red"><xsl:if test="PARAM[@name='Error_Msg']/@value &gt; 0">
    An Error occured!!! </xsl:if>
   <xsl:value-of select="PARAM[@name='Error_Msg']/@value"/></b>
  </p>
  <p>
  <sans-serif><i>click to expand or hide tables</i></sans-serif>
  </p>
 </xsl:template>

<!-- ********************* XSL template for OutFields ******************** -->
  <xsl:template name="outfields">
   <xsl:variable name="imname" select="count(FIELD[@name='Output_Image_Name']/preceding-sibling::FIELD)+1"/>
   <xsl:variable name="imtype" select="count(FIELD[@name='Output_Image_Type']/preceding-sibling::FIELD)+1"/>
   <xsl:variable name="ext" select="count(FIELD[@name='Extension']/preceding-sibling::FIELD)+1"/>
   <xsl:variable name="effarea" select="count(FIELD[@name='Effective_Area']/preceding-sibling::FIELD)+1"/>
   <p>
    <BUTTON type="button" style="background:#CCEECC; font-family: sans-serif; font-weight: bold;" onclick="showhideTable('wwout')">
     Summary Table on Output Files
    </BUTTON>
    <TABLE class="sortable" id="wwout" BORDER="2" style="display: none">
     <TR>
      <TH BGCOLOR="#FFEECC">Output Image Name</TH>
      <TH BGCOLOR="#FFEECC">Output Image Type</TH>
      <TH BGCOLOR="#FFEECC">Extension</TH>
      <TH BGCOLOR="#FFEECC">Effective Area</TH>
     </TR>
     <xsl:for-each select="DATA/TABLEDATA">
      <xsl:for-each select="TR">
       <tr>
        <td align="left" BGCOLOR="#EEEEEE">
         <el><xsl:value-of select="TD[$imname]"/></el>
        </td>
        <td align="center" BGCOLOR="#EEEEEE">
         <el><xsl:value-of select="TD[$imtype]"/></el>
        </td>
        <td align="center" BGCOLOR="#EEEEEE">
         <el><xsl:value-of select="TD[$ext]"/></el>
        </td>
        <td align="right" BGCOLOR="#EEEEEE">
         <el><xsl:value-of select="format-number(TD[$effarea],'##0.0000')"/></el>
        </td>
       </tr>
      </xsl:for-each>
     </xsl:for-each>
    </TABLE>
   </p>
 </xsl:template>

<!-- ********************** XSL template for Config File ********************** -->
  <xsl:template name="Config">
   <p>
    <BUTTON type="button" style="background:#CCEECC; font-family: sans-serif; font-weight: bold;" onclick="showhideTable('config')">
     Configuration File: <xsl:value-of select="PARAM[@name='Prefs_Name']/@value"/>
    </BUTTON>
    <TABLE id="config" class="sortable" style="display: none">
     <TR>
      <TH BGCOLOR="#FFEECC">Config Parameter</TH>
      <TH BGCOLOR="#FFEECC">Value</TH>
     </TR>
     <tr BGCOLOR="#EEEEEE">
      <td><el>WEIGHT_NAMES</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Weight_Names']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>WEIGHT_MIN</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Weight_Min']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>WEIGHT_MAX</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Weight_Max']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>WEIGHT_OUTFLAGS</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Weight_OutFlags']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>FLAG_NAMES</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Flag_Names']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>FLAG_WMASKS</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Flag_Wmasks']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>FLAG_MASKS</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Flag_Masks']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>FLAG_OUTFLAGS</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Flag_Outflags']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>POLY_NAMES</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Poly_Names']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>POLY_OUTFLAGS</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Poly_Outflags']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>POLY_OUTWEIGHTS</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Poly_Outweights']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>POLY_INTERSECT</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Poly_Intersect']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>OUTWEIGHT_NAME</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Outweight_Name']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>OUTFLAG_NAME</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Outflag_Name']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>GETAREA</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Getarea']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>GETAREA_WEIGHT</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Getarea_Weight']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>GETAREA_FLAGS</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Getarea_Flags']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>MEMORY_BUFSIZE</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Memory_Bufsize']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>VERBOSE_TYPE</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Verbose_Type']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>WRITE_XML</el></td>
      <td><el><xsl:value-of select="PARAM[@name='Write_XML']/@value"/></el></td>
     </tr>
     <tr BGCOLOR="#EEEEEE">
      <td><el>NTHREADS</el></td>
      <td><el><xsl:value-of select="PARAM[@name='NThreads']/@value"/></el></td>
     </tr>
    </TABLE>
   </p>
   <p>
    <BUTTON type="button" style="background:#CCEECC; font-family: monospace; font-weight: bold: font-size: 80%;" onclick="showhideTable('commandline')">
     Command Line
    </BUTTON>
    <TABLE id="commandline" style="display: none">
     <TR>
      <TD BGCOLOR="#FFEECC" style="font-size: 80%;"><el>Command Line: <xsl:value-of select="PARAM[@name='Command_Line']/@value"/></el></TD>
     </TR>
    </TABLE>
   </p>
  </xsl:template>

<!-- ********************** XSL template for Warnings ********************** -->
  <xsl:template name="Warnings">
   <xsl:variable name="date" select="count(FIELD[@name='Date']/preceding-sibling::FIELD)+1"/>
   <xsl:variable name="time" select="count(FIELD[@name='Time']/preceding-sibling::FIELD)+1"/>
   <xsl:variable name="msg" select="count(FIELD[@name='Msg']/preceding-sibling::FIELD)+1"/>
   <p>
    <BUTTON type="button" style="background:#CCEECC; font-family: monospace; font-weight: bold: font-size: 80%;" onclick="showhideTable('warnings')">
     Warnings (limited to the last 100)
    </BUTTON>
    <TABLE id="warnings" style="display: none">
     <TR style="font-size: 80%;">
      <TH BGCOLOR="#FFEECC">Date</TH>
      <TH BGCOLOR="#FFEECC">Time</TH>
      <TH BGCOLOR="#FFEECC">Message</TH>
     </TR>
     <xsl:for-each select="DATA/TABLEDATA">
      <xsl:for-each select="TR">
       <tr>
        <td  BGCOLOR="#EEEEEE">
         <el><xsl:value-of select="TD[$date]"/></el>
        </td>
        <td BGCOLOR="#EEEEEE">
         <el><xsl:value-of select="TD[$time]"/></el>
        </td>
        <td align="center" BGCOLOR="#EEEEEE">
         <el><xsl:value-of select="TD[$msg]"/></el>
        </td>
       </tr>
      </xsl:for-each>
     </xsl:for-each>
    </TABLE>
   </p>
 </xsl:template>

 <xsl:template name="Rest">
</xsl:template>
</xsl:stylesheet>
