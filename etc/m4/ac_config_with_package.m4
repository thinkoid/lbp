dnl -*- Autoconf -*-

AC_DEFUN([AC_CONFIG_WITH_PACKAGE],[

AC_MSG_CHECKING([for $1])

AS_VAR_PUSHDEF([ac_path_Var],[ac_$1_path])
AS_VAR_PUSHDEF([ac_incdir_Var],[ac_$1_incdir])
AS_VAR_PUSHDEF([ac_cppflags_Var],[ac_$1_cppflags])
AS_VAR_PUSHDEF([ac_libdir_Var],[ac_$1_libdir])
AS_VAR_PUSHDEF([ac_ldflags_Var],[ac_$1_ldflags])
AS_VAR_PUSHDEF([ac_libs_Var],[ac_$1_libs])

dnl
dnl Only one needed if the package tree has a normal structure
dnl
AC_ARG_WITH([$1],
    [AC_HELP_STRING([--with-$1],[$1 installation directory])],
    [ac_path_Var=${withval}],
    [])

dnl
dnl Override the include directory
dnl
AC_ARG_WITH([$1-incdir],
    [AC_HELP_STRING([--with-$1-incdir],[$1 include directory])],
    [ac_incdir_Var=${withval}],
    [])

dnl
dnl Override the library directory
dnl
AC_ARG_WITH([$1-libdir],
    [AC_HELP_STRING([--with-$1-libdir],[$1 library directory])],
    [ac_libdir_Var=${withval}],
    [])

dnl
dnl Override the libs
dnl
AC_ARG_WITH([$1-libs],
    [AC_HELP_STRING([--with-$1-libs],[$1 libraries])],
    [ac_libs_Var=${withval}],
    [])

if test "x${ac_incdir_Var}" = x; then
  if test "x${ac_path_Var}" != x; then
    ac_cppflags_Var="-I${ac_path_Var}/include"
  fi
else
  ac_cppflags_Var="-I${ac_incdir_Var}"
fi

dnl
dnl The (one) library should have the same name as the package
dnl

if test "x${ac_libdir_Var}" = x; then
  if test "x${ac_path_Var}" != x; then
    ac_ldflags_Var="-L${ac_path_Var}/lib"
  fi
else
  ac_ldflags_Var="-L${ac_libdir_Var}"
fi

if test "x${ac_libs_Var}" = x; then
   ac_libs_Var="-l$1"
fi

AC_SUBST(AS_TR_SH([AS_TR_CPP([$1])_CPPFLAGS]),[$ac_cppflags_Var])
AC_SUBST(AS_TR_SH([AS_TR_CPP([$1])_LDFLAGS]), [$ac_ldflags_Var])
AC_SUBST(AS_TR_SH([AS_TR_CPP([$1])_LIBS]),    [$ac_libs_Var])

AC_MSG_RESULT([done])

AS_VAR_POPDEF([ac_path_Var])
AS_VAR_POPDEF([ac_incdir_Var])
AS_VAR_POPDEF([ac_libdir_Var])
AS_VAR_POPDEF([ac_libs_Var])
AS_VAR_POPDEF([ac_cppflags_Var])
AS_VAR_POPDEF([ac_ldflags_Var])

])
