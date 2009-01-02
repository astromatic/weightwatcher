# Program Makefile for WeightWatcher
# Copyright (C) 2003 Emmanuel Bertin.
SUBDIRS			= fits wcs
bin_PROGRAMS		= ww
ww_SOURCES		= field.c main.c makeit.c prefs.c readimage.c \
			  vector.c \
			  define.h field.h globals.h key.h prefs.h preflist.h \
			  readimage.h types.h vector.h fitswcs.c fitswcs.h \
                          wcscelsys.h xml.h xml.c
ww_LDADD		= $(top_builddir)/src/fits/libfits.a $(top_builddir)/src/wcs/libwcs_c.a
DATE=`date +"%Y-%m-%d"`
