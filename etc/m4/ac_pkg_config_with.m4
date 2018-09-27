# -*- mode: autoconf; -*-

AC_DEFUN([AC_PKG_CONFIG_WITH],[

AC_MSG_CHECKING([for $1]) 

if ! pkg-config --exists $1 &>/dev/null; then
    echo 'missing pkg-config for $1'; exit 1
fi

ac_$1_cppflags="$( pkg-config --cflags $1 )"
ac_$1_libs="$( pkg-config --libs $1 )"

AC_SUBST(AS_TR_CPP([$1_CPPFLAGS]), [$ac_$1_cppflags])
AC_SUBST(AS_TR_CPP([$1_LIBS]), [$ac_$1_libs])

AC_MSG_RESULT([done])
])
