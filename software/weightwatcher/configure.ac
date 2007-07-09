# configure.in for WeightWatcher
# (C) E.Bertin C. Marmo 2003-2007
# Process this file with autoconf to produce a configure script.
# First, disable the annoying config.cache
define([AC_CACHE_LOAD],)
define([AC_CACHE_SAVE],)

# This is your standard Bertin source code...
AC_INIT(weightwatcher, 1.8.5, [bertin@iap.fr])
AC_CONFIG_SRCDIR(src/makeit.c)
AC_CONFIG_AUX_DIR(autoconf)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE
date=`date +%Y-%m-%d`
date2=`date +"%a %b %d %Y"`
date3=`date +"%B %Y"`
AC_DEFINE_UNQUOTED(DATE, "$date", [Archive creation date])
AC_SUBST(PACKAGER, "Chiara Marmo")
AC_SUBST(DATE2, "$date2")
AC_SUBST(DATE3, "$date3")

# Include macros
sinclude(acx_prog_cc_optim.m4)
sinclude(acx_urbi_resolve_dir.m4)

# Display pakage and version number
AC_MSG_RESULT([*********** Configuring:  $PACKAGE $VERSION ($date) **********])

# Initialize the list of compilers to consider
cclist="cc gcc"

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

# Checks for programs.
# GCC is chosen last because it is likely to yield less optimized code
AC_LANG(C)
AC_PROG_CC([$cclist])

# C Compiler: Check that it is ANSI C
AM_PROG_CC_STDC
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# C Compiler: Check that it is POSIX-compliant
AC_ISC_POSIX
ACX_PROG_CC_OPTIM
AC_PROG_INSTALL

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
AC_STRUCT_TM
AC_CHECK_TYPES([unsigned long long, long long])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_CHECK_FUNCS([atexit memmove memset munmap pow sqrt strchr strcspn strpbrk strrchr strspn strstr strtol])

# Check support for large files
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

AC_DEFINE(_FILE_OFFSET_BITS, 64, [off_t size])
AC_DEFINE(_LARGEFILE64_SOURCE, 1, [enable 64bit file pointers])

# Set the data directory to a true absolute path
datadir2=$(URBI_RESOLVE_DIR([$datadir]))

# Provide a special option for the default XSLT URL
AC_ARG_WITH(xsl_url,
        [AC_HELP_STRING([--with-xsl_url=<default URL for XSLT filter>],
        [Provide an alternative default URL of the XSLT filter])],
    [xsl_url=$withval],
    [xsl_url="file://$datadir2/$PACKAGE_NAME/ww.xsl"]
    )

AC_DEFINE_UNQUOTED([XSL_URL], "$xsl_url",[Default URL of the XSLT filter])

# Provide special option for gprof profiling
AC_MSG_CHECKING([for gprof profiler mode])
AC_ARG_ENABLE(gprof,
        [AC_HELP_STRING([--enable-gprof],
        [Enable special mode for compilation with the gprof profiler \
(off by default)])],
        use_gprof="yes"
        AC_MSG_RESULT([yes]),
        use_gprof="no"
        AC_MSG_RESULT([no]))

# Enable static linking
AC_MSG_CHECKING([for static linking])
AC_ARG_ENABLE(static,
        [AC_HELP_STRING([--enable-static],
        [Enable static linking (off by default)])],
        use_static="$enableval"
        AC_MSG_RESULT([yes]),
        use_static="no"
        AC_MSG_RESULT([no]))

# Special optimization options for the INTEL C compiler
if test $use_icc = "yes" -a $CC = "icc"; then
  CFLAGS="-O -axWP -ip -unroll"
fi

if test "$use_gprof" = "yes"; then
  if test "$use_icc" = "yes"; then
    CFLAGS="$CFLAGS -pq"
  else
    CFLAGS="$CFLAGS -pg"
  fi
  use_static="no"
fi

if test "$use_static" = "yes"; then
  LDFLAGS="$LDFLAGS -all-static"
fi

# Display compiler and linker flags
AC_MSG_RESULT([***************************************************************])
AC_MSG_RESULT([Compile cmdline:  $CC $CFLAGS])
AC_MSG_RESULT([Link    cmdline:  $CC $LDFLAGS $LIBS])
AC_MSG_RESULT([Default XSLT URL: $xsl_url])
AC_MSG_RESULT([***************************************************************])

AC_CONFIG_FILES([Makefile
                 man/Makefile
                 src/Makefile
                 src/fits/Makefile
                 src/wcs/Makefile
                 weightwatcher.spec
                 man/ww.1])
AC_OUTPUT
