AC_PREREQ(2.68)

AC_INIT([pnmunpaper], [0.5.4], [https://github.com/ImageProcessing-ElectronicPublications/pnmunpaper/issues], , [https://github.com/ImageProcessing-ElectronicPublications/pnmunpaper])

AM_INIT_AUTOMAKE([1.11 dist-xz no-dist-gzip foreign subdir-objects])
AM_SILENT_RULES([yes])
AM_MAINTAINER_MODE([enable])

AC_PROG_LIBTOOL
AC_PROG_CC_C99
AC_PROG_MKDIR_P

AC_USE_SYSTEM_EXTENSIONS

AC_HEADER_STDBOOL
AC_SEARCH_LIBS([sqrt], [m])

AC_PATH_PROG([PNGTOPNM], [pngtopnm], [false])
AC_PATH_PROG([PNMTOPNG], [pnmtopng], [false])

AC_ARG_VAR([XSLTPROC], [A libxslt-compatible XSLT processor command])
AS_IF([test "x$XSLTPROC" = "x"], [
  AC_CHECK_PROGS([XSLTPROC], [xsltproc])
  AS_IF([test "x$XSLTPROC" = "x"], [
    AC_MSG_ERROR([unable to find xsltproc, you need this to build man pages])
  ])
])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
