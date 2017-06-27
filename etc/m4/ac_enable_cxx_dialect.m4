dnl -*- Autoconf -*-

AC_DEFUN([AC_ENABLE_CXX_DIALECT],[
  CXXFLAGS+=" -std=$1"
])
