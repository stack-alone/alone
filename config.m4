dnl $Id$
dnl config.m4 for extension alone

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(alone, for alone support,
dnl Make sure that the comment is aligned:
	[  --with-alone             Include alone support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(alone, whether to enable alone support,
dnl Make sure that the comment is aligned:
	[  --enable-alone           Enable alone support])

if test "$PHP_ALONE" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-alone -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/alone.h"  # you most likely want to change this
  dnl if test -r $PHP_ALONE/$SEARCH_FOR; then # path given as parameter
  dnl   ALONE_DIR=$PHP_ALONE
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for alone files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       ALONE_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$ALONE_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the alone distribution])
  dnl fi

  dnl # --with-alone -> add include path
  dnl PHP_ADD_INCLUDE($ALONE_DIR/include)

  dnl # --with-alone -> check for lib and symbol presence
  dnl LIBNAME=alone # you may want to change this
  dnl LIBSYMBOL=alone # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ALONE_DIR/$PHP_LIBDIR, ALONE_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_ALONELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong alone lib version or lib not found])
  dnl ],[
  dnl   -L$ALONE_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(ALONE_SHARED_LIBADD)
  AC_MSG_CHECKING([for alone headers])
  for i in "$PHP_ALONE" "$prefix" /usr /usr/local; do
    if test -r "$i/include/yaml.h"; then
      PHP_YAML_DIR=$i
      AC_MSG_RESULT([found in $i])
      break
    fi
  done
  if test -z "$PHP_YAML_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please install libyaml])
  fi
  PHP_ADD_INCLUDE($PHP_YAML_DIR/include)
  
  export OLD_CPPFLAGS="$CPPFLAGS"
  export CPPFLAGS="$CPPFLAGS $INCLUDES -DHAVE_YAML"
  AC_CHECK_HEADER([yaml.h], [], AC_MSG_ERROR(['yaml.h' header not found]))
  PHP_SUBST(ALONE_SHARED_LIBADD)
  
  PHP_ADD_LIBRARY_WITH_PATH(yaml, $PHP_YAML_DIR/$PHP_LIBDIR, ALONE_SHARED_LIBADD)
  export CPPFLAGS="$OLD_CPPFLAGS"

  PHP_SUBST(ALONE_SHARED_LIBADD)
  AC_DEFINE(HAVE_YAML, 1, [ ])
  
  PHP_NEW_EXTENSION(alone,
   alone.c		\
   alone_config.c		\ 
   include/yaml/detect.c			\
   include/yaml/parse.c			\
   configs/config_ini.c		\
   configs/config_json.c		\
   configs/config_yaml.c	, 
   $ext_shared)
  
  PHP_ADD_BUILD_DIR([$ext_builddir/configs])
fi

