AC_PREREQ([2.69])
AC_INIT([words-searcher], [0.0.1], [])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([foreign subdir-objects -Wall -Wno-portability])
AM_PROG_AR
LT_INIT

AC_PROG_CXX
AC_PROG_INSTALL

PKG_CHECK_MODULES([QT6], [Qt6Widgets Qt6Gui Qt6Core >= 6.0.0])
AC_SUBST([QT6_CFLAGS])
AC_SUBST([QT6_LIBS])

AC_ARG_VAR([MOC], [Qt6 moc compiler])
if test -z "$MOC"; then
  if test -x /usr/lib/qt6/libexec/moc; then
    MOC=/usr/lib/qt6/libexec/moc
  else
    AC_PATH_PROG([MOC], [moc-qt6], [no])
  fi
fi
if test "x$MOC" = "xno" || test -z "$MOC"; then
  AC_MSG_ERROR([Qt6 moc not found; set MOC=/path/to/moc])
fi
AC_MSG_NOTICE([using moc: $MOC])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
