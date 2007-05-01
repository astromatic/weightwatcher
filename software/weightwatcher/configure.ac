# configure.in for WeightWatcher
# (C) E.Bertin 2003-2005
# Process this file with autoconf to produce a configure script.
# First, disable the annoying config.cache
define([AC_CACHE_LOAD],)
define([AC_CACHE_SAVE],)

# This is your standard Bertin source code...
AC_INIT(weightwatcher, 1.8.0, [bertin@iap.fr])
AC_CONFIG_SRCDIR(src/makeit.c)
AC_CONFIG_AUX_DIR(autoconf)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE
date=`date +%Y-%m-%d`
date2=`date +"%a %b %d %Y"`
date3=`date +"%B %Y"`
AC_DEFINE_UNQUOTED(DATE, "$date", [Archive creation date])
AC_SUBST(PACKAGER, "Emmanuel Bertin")
AC_SUBST(DATE2, "$date2")
AC_SUBST(DATE3, "$date3")

# Include macros
sinclude(acx_prog_cc_optim.m4)

# Display pakage and version number
AC_MSG_RESULT([*********** Configuring:  $PACKAGE $VERSION ($date) **********])

# Initialize the list of compilers to consider
cclist="cc gcc"
use_static="no"

# Backup and reset the input CFLAGS and LDFLAGS
mycflags="$CFLAGS"
CFLAGS=""
myldflags="$LDFLAGS"
LDFLAGS=""

# Provide special option for the Linux Intel C compiler
AC_MSG_CHECKING([for Linux Intel C compiler mode])
AC_ARG_ENABLE(icc,
	[AC_HELP_STRING([--enable-icc],
	[Enable special mode for compilation with the Intel compiler \
(off by default)])],
        use_icc="yes"
        cclist="icc $cclist"
	AC_MSG_RESULT([yes]),
        use_icc="no"
	AC_MSG_RESULT([no]))

# Provide special option for the Assure debugger
AC_MSG_CHECKING([for Assure debugger mode])
AC_ARG_ENABLE(assure,
	[AC_HELP_STRING([--enable-assure],
	[Enable special mode for compilation with the Assure debugger \
(off by default)])],
        use_assure="yes"
        cclist="assuretcc $cclist"
	AC_MSG_RESULT([yes]),
        use_assure="no"
	AC_MSG_RESULT([no]))

# Provide special option for the Insure++ debugger
AC_MSG_CHECKING([for Insure++ debugger mode])
AC_ARG_ENABLE(insure,
	[AC_HELP_STRING([--enable-insure],
	[Enable special mode for compilation with the Insure++ debugger \
(off by default)])],
        use_insure="yes"
        cclist="insure $cclist"
	AC_MSG_RESULT([yes]),
        use_insure="no"
	AC_MSG_RESULT([no]))

# Provide special option for the e-fence malloc checker
AC_MSG_CHECKING([for e-fence mode])
AC_ARG_ENABLE(efence,
	[AC_HELP_STRING([--enable-efence],
	[Enable linking with the -e-fence library (off by default)])],
        use_efence="yes"
	AC_MSG_RESULT([yes]),
        use_efence="no"
	AC_MSG_RESULT([no]))

# Set the data directory to a true absolute path
if test "x${datadir}" = 'x${prefix}/share'; then
  if test "x${prefix}" = "xNONE"; then
    datadir2="${ac_default_prefix}/share"
  else
    datadir2="${prefix}/share"
  fi
elif test "x${datadir}" = 'x${datarootdir}'; then
  if test "x${datarootdir}" = 'x${prefix}/share'; then
    if test "x${prefix}" = "xNONE"; then
      datadir2="${ac_default_prefix}/share"
    else
      datadir2="${prefix}/share"
    fi
  fi
else
  datadir2="${datadir}"
fi

# Provide a special option for the default XSLT URL
AC_ARG_WITH(xsl_url,
        [AC_HELP_STRING([--with-xsl_url=<default URL for XSLT filter>],
        [Provide an alternative default URL of the XSLT filter])],
    [xsl_url=$withval],
    [xsl_url="file://$datadir2/$PACKAGE_NAME/ww.xsl"]
    )

AC_DEFINE_UNQUOTED([XSL_URL], "$xsl_url",[Default URL of the XSLT filter])

# Checks for programs.
# GCC is chosen last because it is likely to yield less optimized code
AC_PROG_CC([$cclist])
# C Compiler: Check that it is ANSI C
AM_PROG_CC_STDC
# C Compiler: Check that it is POSIX-compliant
AC_ISC_POSIX
ACX_PROG_CC_OPTIM
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB(m, sin)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(stdlib.h string.h fcntl.h unistd.h sys/mman.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SIGNAL
AC_TYPE_UID_T
AC_CHECK_TYPES([unsigned long long, long long])

# Checks for library functions.
AC_FUNC_MMAP
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([atexit memmove memset munmap strchr strpbrk strrchr strcspn strtol])

# Check support for large files
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

AC_DEFINE(_FILE_OFFSET_BITS, 64, [off_t size])
AC_DEFINE(_LARGEFILE64_SOURCE, 1, [enable 64bit file pointers])

# Special optimization options for the INTEL C compiler
if test $use_icc = "yes" -a $CC = "icc"; then
  CFLAGS="$CFLAGS -O3 -axiMKW -ipo -ipo_obj -unroll"
fi

# Special options for the Insure debugger
if test $use_insure = "yes" -a $CC = "insure"; then
  CFLAGS="$CFLAGS -g"
  use_static="no"
fi
# Special optimization options for the INTEL C compiler
if test $use_icc = "yes" -a $CC = "icc"; then
  CFLAGS="$CFLAGS -O3 -axiMKW -ipo -ipo_obj -unroll"
fi

# Special options for the Assure debugger
rootsrcdir=`pwd`
if test $use_assure = "yes" -a $CC = "assuretcc"; then
  CFLAGS="-WApname=$rootsrcdir/$PACKAGE"
  LDFLAGS="-WApname=$rootsrcdir/$PACKAGE"
fi

# Link with e-fence option
if test "$use_efence" = "yes"; then
  LIBS="-lefence $LIBS"
fi

if test "$use_static" = "yes"; then
  LDFLAGS="$LDFLAGS -static"
fi

# Override automatic CFLAGS and LDFLAGS with those of user
if test -n "$mycflags"; then
CFLAGS="$mycflags"
fi
if test -n "$myldflags"; then
LDFLAGS="$myldflags"
fi

# Display compiler and linker flags
AC_MSG_RESULT([I will compile using:  $CC $CFLAGS])
AC_MSG_RESULT([I will link using:     $CC $LDFLAGS $LIBS])

AC_CONFIG_FILES(Makefile man/Makefile src/Makefile src/fits/Makefile src/wcs/Makefile weightwatcher.spec man/ww.1)
AC_OUTPUT
