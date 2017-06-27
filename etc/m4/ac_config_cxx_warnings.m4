dnl -*- Autoconf -*-

AC_DEFUN([AC_CONFIG_CXX_WARNINGS],[

CLANG_WARNFLAGS=" \
-Wno-deprecated-register \
-Wno-logical-op-parentheses"

GCC_WARNFLAGS=" \
-Wall \
-Wno-parentheses \
-Wno-strict-aliasing \
-Wno-unused-function \
-Wno-unused-local-typedefs \
-Wno-unused-variable"

test -z "$OSNAME" && OSNAME=$( uname )

case $CXX in
    *clang++)
        CXXFLAGS+=$GCC_WARNFLAGS
        ;;
    *g++)
        if test "$OSNAME" = "Darwin"; then
            CXXFLAGS+=$CLANG_WARNFLAGS
        else
            CXXFLAGS+=$GCC_WARNFLAGS
        fi
        ;;
    *)
        ;;
esac
])
