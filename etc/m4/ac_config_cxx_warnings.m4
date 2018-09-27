dnl -*- Autoconf -*-

AC_DEFUN([AC_CONFIG_CXX_WARNINGS],[

WARNFLAGS=" -W -Wall"

CLANG_WARNFLAGS=" \
-Wno-deprecated-register \
-Wno-logical-op-parentheses"

GCC_WARNFLAGS=" \
-Wno-parentheses \
-Wno-strict-aliasing \
-Wno-unused-function \
-Wno-unused-local-typedefs \
-Wno-unused-variable"

case $CXX in
    *clang++)
        WARNFLAGS+=$CLANG_WARNFLAGS
        ;;
    *g++)
        if test x`uname` = xDarwin; then
            WARNFLAGS+=$CLANG_WARNFLAGS
        else
            WARNFLAGS+=$GCC_WARNFLAGS
        fi
        ;;
    *)
        ;;
esac

CFLAGS+="$WARNFLAGS"
CXXFLAGS+="$WARNFLAGS"

])
