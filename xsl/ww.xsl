<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE xsl:stylesheet [
        <!ENTITY nbsp "&#160;">
        <!ENTITY deg "&#176;">
        <!ENTITY amin "&#180;">
        <!ENTITY asec "&#168;">
        <!ENTITY copy "&#169;">
        <!ENTITY darr "&#8595;">
        ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<!-- 
#				weightwatcher.xsl
#
# Global XSL template
#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#
#	This file part of:	WeightWatcher
#
#	Copyright:		(C) 2006-2010 Emmanuel Bertin -- IAP/CNRS/UPMC
#				& Chiara Marmo -- IAP/CNRS
#
#	License:		GNU General Public License
#
#	WeightWatcher is free software: you can redistribute it and/or modify
#	it under the terms of the GNU General Public License as published by
#	the Free Software Foundation, either version 3 of the License, or
# 	(at your option) any later version.
#	WeightWatcher is distributed in the hope that it will be useful,
#	but WITHOUT ANY WARRANTY; without even the implied warranty of
#	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#	GNU General Public License for more details.
#	You should have received a copy of the GNU General Public License
#	along with WeightWatcher.  If not, see <http://www.gnu.org/licenses/>.
#
#	Last modified:		26/10/2010
#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% -->

 <xsl:template match="/">
  <xsl:variable name="date" select="/VOTABLE/RESOURCE/RESOURCE[@name='MetaData']/PARAM[@name='Date']/@value"/>
  <xsl:variable name="time" select="/VOTABLE/RESOURCE/RESOURCE[@name='MetaData']/PARAM[@name='Time']/@value"/>
  <HTML>
   <HEAD>
    <link rel="shortcut icon" type="image/x-icon" href="http://astromatic.iap.fr/xsl/favicon.ico" />
    <script type="text/javascript" src="http://astromatic.iap.fr/xsl/sorttable.js"/>

    <style type="text/css">
     p {
      font-family: sans-serif;
      }
     p.italic {font-style: italic}
     body {
      margin: 10px;
      background-color: #e0e0e0;
      background-image: url("http://astromatic.iap.fr/xsl/body_bg.jpg");
      background-repeat: repeat-x;
      background-position: top;
      min-width:662px;
      }
     mono {font-family: monospace}
     el {
      font-family: monospace;
      font-size: 100%;
      color: black;
      }
     elm {
      font-family: monospace;
      font-size: 67%;
      white-space: nowrap;
      }
     a {text-decoration: none; font-style: bold; color: #476674}
     a:hover {text-decoration: underline;}
     #header {
      padding: 5px;
      min-width: 662px;
      background-image: url("http://astromatic.iap.fr/xsl/astromaticleft.png");
      background-repeat: repeat-x;
      background-position: left top;
      text-align: left;
      font-size: 1.2em;
      margin: 0 0 30px 0;
      color:#d3e7f0;
      font-weight: bold;
      }
     th {
      background-color:#d3e7f0;
      border-top: 1px solid white;
      border-left: 1px solid white;
      border-right: 1px solid #476674;
      border-bottom: 1px solid #476674;
      -moz-border-radius: 3px;
      -khtml-border-radius: 3px;
      -webkit-border-radius: 3px;
      border-radius: 3px;
      padding: 2px;
      line-height: 12px;
      }
     td {
      background-color:#f2f4f4;
      padding-left: 2px;
      padding-right: 2px;
      }
     table.sortable {
      border-top: 1px solid #476674;
      border-left: 1px solid #476674;
      border-right: 1px solid white;
      border-bottom: 1px solid white;
      -moz-border-radius: 3px;
      -khtml-border-radius: 3px;
      -webkit-border-radius: 3px;
      border-radius: 3px;
      }
     table.sortable a.sortheader {
      background-color:#d3e7f0;
      font-weight: bold;
      font-size: 80%;
      text-decoration: none;
      display: button;
      }

     table.sortable span.sortarrow {
      color: black;
      font-weight: bold;
      text-decoration: blink;
      }
     table.sortable a.sortheader.sub {vertical-align: sub}
     </style>

     <title>
      Processing summary on <xsl:value-of select="$date"/> at <xsl:value-of select="$time"/>
     </title>
    </HEAD>
    <BODY>
     <div id="header">
      <a href="/"><img style="vertical-align: middle; border:0px" src="http://astromatic.iap.fr/xsl/astromatic.png" title="Astromatic home" alt="Astromatic.net" /></a>  Processing summary
     </div>
     <xsl:call-template name="VOTable"/>
   </BODY>
  </HTML>
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
 </xsl:template>

<!-- ********************* XSL template for OutFields ******************** -->
  <xsl:template name="outfields">
   <xsl:variable name="imname" select="count(FIELD[@name='Output_Image_Name']/preceding-sibling::FIELD)+1"/>
   <xsl:variable name="imtype" select="count(FIELD[@name='Output_Image_Type']/preceding-sibling::FIELD)+1"/>
   <xsl:variable name="ext" select="count(FIELD[@name='Extension']/preceding-sibling::FIELD)+1"/>
   <xsl:variable name="effarea" select="count(FIELD[@name='Effective_Area']/preceding-sibling::FIELD)+1"/>
   <p>
    <BUTTON type="button" title="click to expand" onclick="showhideTable('wwout')">
     Summary Table on Output Files&nbsp;&darr;
    </BUTTON>
    <TABLE class="sortable" id="wwout" style="display: none">
     <TR>
      <TH>Output Image Name</TH>
      <TH>Output Image Type</TH>
      <TH>Extension</TH>
      <TH>Effective Area</TH>
     </TR>
     <xsl:for-each select="DATA/TABLEDATA">
      <xsl:for-each select="TR">
       <tr>
        <td align="left">
         <el><xsl:value-of select="TD[$imname]"/></el>
        </td>
        <td align="center">
         <el><xsl:value-of select="TD[$imtype]"/></el>
        </td>
        <td align="center">
         <el><xsl:value-of select="TD[$ext]"/></el>
        </td>
        <td align="right">
         <el><xsl:value-of select="format-number(TD[$effarea],'##0.000000')"/></el>
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
    <BUTTON type="button" title="click to expand" onclick="showhideTable('config')">
     Configuration File:
      <B><xsl:value-of select="PARAM[@name='Prefs_Name']/@value"/></B>
      &darr;
    </BUTTON>
    <TABLE id="config" class="sortable" style="display: none">
     <TR>
      <TH>Config Parameter</TH>
      <TH>Value</TH>
     </TR>
     <xsl:for-each select="PARAM[position()>2]">
      <tr>
       <td><el><xsl:value-of select="@name"/></el></td>
       <td><el><xsl:value-of select="@value"/></el></td>
      </tr>
     </xsl:for-each>
    </TABLE>
   </p>
   <p>
    <BUTTON type="button" title="click to expand" onclick="showhideTable('commandline')">
     Command Line&nbsp;&darr;
    </BUTTON>
    <TABLE id="commandline" style="display: none">
     <TR>
      <TD style="font-size: 80%;"><el><xsl:value-of select="PARAM[@name='Command_Line']/@value"/></el></TD>
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
    <BUTTON type="button" title="click to expand" onclick="showhideTable('warnings')">
     Warnings (limited to the last 100)&nbsp;&darr;
    </BUTTON>
    <TABLE id="warnings" class="sortable" style="display: none">
     <TR style="font-size: 80%;">
      <TH>Date</TH>
      <TH>Time</TH>
      <TH>Message</TH>
     </TR>
     <xsl:for-each select="DATA/TABLEDATA">
      <xsl:for-each select="TR">
       <tr>
        <td >
         <el><xsl:value-of select="TD[$date]"/></el>
        </td>
        <td>
         <el><xsl:value-of select="TD[$time]"/></el>
        </td>
        <td align="center">
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
