# Program Makefile for WeightWatcher
# Copyright (C) 2003 Emmanuel Bertin.
SUBDIRS			= fits
bin_PROGRAMS		= ww
ww_SOURCES		= field.c main.c makeit.c prefs.c readimage.c \
			  vector.c \
			  define.h field.h globals.h key.h prefs.h preflist.h \
			  readimage.h types.h vector.h
ww_LDADD		= $(top_builddir)/src/fits/libfits.a
DATE=`date +"%Y-%m-%d"`
