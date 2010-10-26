#
#				Makefile.am
#
# src Makefile. Process this file with automake to generate a Makefile
#
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
#
#	This file part of:	WeightWatcher
#
#	Copyright:		(C) 2003-2010 Emmanuel Bertin -- IAP/CNRS/UPMC
#				& Chiara Marmo -- IAP/CNRS
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
#%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

SUBDIRS			= fits wcs
bin_PROGRAMS		= ww
ww_SOURCES		= field.c main.c makeit.c prefs.c readimage.c \
			  vector.c \
			  define.h field.h globals.h key.h prefs.h preflist.h \
			  readimage.h types.h vector.h fitswcs.c fitswcs.h \
                          wcscelsys.h xml.h xml.c
ww_LDADD		= $(top_builddir)/src/fits/libfits.a $(top_builddir)/src/wcs/libwcs_c.a
DATE=`date +"%Y-%m-%d"`

