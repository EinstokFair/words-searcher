AC_PREREQ([2.69])
AC_INIT([words-searcher], [0.0.8], [])
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


# Проверка инструментов локализации
AC_ARG_VAR([LUPDATE], [Qt6 lupdate compiler])
AS_IF([test -z "$LUPDATE"], [AC_PATH_PROG([LUPDATE], [lupdate-qt6], [no])])
AS_IF([test "x$LUPDATE" = "xno"], [AC_PATH_PROG([LUPDATE], [lupdate], [no])])
# В Debian trixie прямой бинарник Qt6 лежит в /usr/lib/qt6/bin
AS_IF([test "x$LUPDATE" = "xno" -a -x /usr/lib/qt6/bin/lupdate], [LUPDATE=/usr/lib/qt6/bin/lupdate])
AC_MSG_NOTICE([using lupdate: $LUPDATE])
AC_ARG_VAR([LRELEASE], [Qt6 lrelease compiler])
AS_IF([test -z "$LRELEASE"], [AC_PATH_PROG([LRELEASE], [lrelease-qt6], [no])])
AS_IF([test "x$LRELEASE" = "xno"], [AC_PATH_PROG([LRELEASE], [lrelease], [no])])
# В Debian trixie прямой бинарник Qt6 лежит в /usr/lib/qt6/bin
AS_IF([test "x$LRELEASE" = "xno" -a -x /usr/lib/qt6/bin/lrelease], [LRELEASE=/usr/lib/qt6/bin/lrelease])
AC_MSG_NOTICE([using lrelease: $LRELEASE])


AC_ARG_VAR([RCC], [Qt6 rcc compiler])
AS_IF([test -z "$RCC"], [AC_PATH_PROG([RCC], [rcc-qt6], [no])])
AS_IF([test "x$RCC" = "xno"], [AC_PATH_PROG([RCC], [rcc], [no])])
AS_IF([test "x$RCC" = "xno" -a -x /usr/lib/qt6/libexec/rcc], [RCC=/usr/lib/qt6/libexec/rcc])
AC_MSG_NOTICE([using rcc: $RCC])

AC_CONFIG_FILES([Makefile])

# LINGUAS: выбор языков (ru всегда, en — опционально)
AC_ARG_VAR([LINGUAS], [Language(s) to install (space-separated)])
if test -z "$LINGUAS"; then
    LINGUAS="ru en"
fi
AC_SUBST([LINGUAS])
have_en=no
for lang in $LINGUAS; do
    if test "x$lang" = "xen"; then have_en=yes; fi
done
AM_CONDITIONAL([HAVE_EN], [test "x$have_en" = "xyes"])

AC_OUTPUT
