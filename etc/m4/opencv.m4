# -*- mode: autoconf; -*-
# opencv.m4: Locate Opencv headers and libraries for autoconf-based projects.
#
# Based on http://github.com/tsuna/boost.m4
# Copyright (C) 2007-2011, 2014  Benoit Sigoure <tsuna@lrde.epita.fr>
# Copyright (C) 2015-2016  Liviu Nicoara <nikkoara at hates dot ms>

m4_define([_OPENCV_SERIAL], [m4_translit([
# serial 1
], [#
], [])])

m4_pattern_forbid([^_?(OPENCV|Opencv)_])

m4_define([AC_SED_CPP],
  [AC_LANG_PUSH([C++])dnl
   AC_LANG_PREPROC_REQUIRE()dnl
   AC_REQUIRE([AC_PROG_SED])dnl

   AC_PREPROC_IFELSE(
     [AC_LANG_SOURCE([$2
value:$1])],
     [ppval=`$SED -n "s,value:,,p" conftest.i`; $3],
     [ppval=''; $4])
   AC_LANG_POP([C++])dnl
])# AC_SED_CPP

# OPENCV_REQUIRE([VERSION], [ACTION-IF-NOT-FOUND])
# -----------------------------------------------
# Look for Opencv.  If version is given, it must either be a literal of the form
# "X.Y.Z" where X, Y and Z are integers (the ".Z" part being optional) or a
# variable "$var".
# Defines the value OPENCV_CPPFLAGS.  This macro only checks for headers with
# the required version, it does not check for any of the Opencv libraries.
# On # success, defines HAVE_OPENCV.  On failure, calls the optional
# ACTION-IF-NOT-FOUND action if one was supplied.
# Otherwise aborts with an error message.
AC_DEFUN_ONCE([OPENCV_REQUIRE],
[AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_PROG_GREP])dnl
echo "$as_me: this is opencv.m4[]_OPENCV_SERIAL" >&AS_MESSAGE_LOG_FD
opencv_save_IFS=$IFS
opencv_version_req=$1
IFS=.
set x $opencv_version_req 0 0 0
IFS=$opencv_save_IFS
shift
opencv_version_req=`expr "$[1]" '*' 100000 + "$[2]" '*' 100 + "$[3]"`
opencv_version_req_string=$[1].$[2].$[3]
AC_ARG_WITH([opencv],
   [AS_HELP_STRING([--with-opencv=DIR],
                   [prefix of Opencv $1 @<:@guess@:>@])])dnl
AC_ARG_VAR([OPENCV_ROOT],[Location of Opencv installation])dnl
# If OPENCV_ROOT is set and the user has not provided a value to
# --with-opencv, then treat OPENCV_ROOT as if it the user supplied it.
if test x"$OPENCV_ROOT" != x; then
  if test x"$with_opencv" = x; then
    AC_MSG_NOTICE([Detected OPENCV_ROOT; continuing with --with-opencv=$OPENCV_ROOT])
    with_opencv=$OPENCV_ROOT
  else
    AC_MSG_NOTICE([Detected OPENCV_ROOT=$OPENCV_ROOT, but overridden by --with-opencv=$with_opencv])
  fi
fi
AC_SUBST([DISTCHECK_CONFIGURE_FLAGS],
         ["$DISTCHECK_CONFIGURE_FLAGS '--with-opencv=$with_opencv'"])dnl
opencv_save_CPPFLAGS=$CPPFLAGS
  AC_CACHE_CHECK([for Opencv headers version >= $opencv_version_req_string],
    [opencv_cv_inc_path],
    [opencv_cv_inc_path=no
AC_LANG_PUSH([C++])dnl
m4_pattern_allow([^OPENCV_VERSION$])dnl
    AC_LANG_CONFTEST([AC_LANG_PROGRAM([[@%:@include <opencv2/core/version.hpp>
#if !defined (CV_VERSION)
# error CV_VERSION is not defined
#elif $opencv_version_req > (CV_VERSION_MAJOR * 100000 + CV_VERSION_MINOR * 100 + CV_VERSION_MINOR)
# error Opencv headers version < $opencv_version_req_string
#endif
]])])
    # If the user provided a value to --with-opencv, use it and only it.
    case $with_opencv in #(
      ''|yes) set x '' /opt/local/include /usr/local/include /opt/include \
                 /usr/include C:/Opencv/include;; #(
      *)      set x "$with_opencv/include" "$with_opencv";;
    esac
    shift
    for opencv_dir
    do
    # Without --layout=system, Opencv (or at least some versions) installs
    # itself in <prefix>/include/opencv-<version>.  This inner loop helps to
    # find headers in such directories.
    #
    # Any ${opencv_dir}/opencv-x_xx directories are searched in reverse version
    # order followed by ${opencv_dir}.  The final '.' is a sentinel for
    # searching $opencv_dir" itself.  Entries are whitespace separated.
    #
    # I didn't indent this loop on purpose (to avoid over-indented code)
    opencv_layout_system_search_list=`cd "$opencv_dir" 2>/dev/null \
        && ls -1 | "${GREP}" '^opencv-' | sort -rn -t- -k2 \
        && echo .`
    for opencv_inc in $opencv_layout_system_search_list
    do
      if test x"$opencv_inc" != x.; then
        opencv_inc="$opencv_dir/$opencv_inc"
      else
        opencv_inc="$opencv_dir" # Uses sentinel in opencv_layout_system_search_list
      fi
      if test x"$opencv_inc" != x; then
        # We are going to check whether the version of Opencv installed
        # in $opencv_inc is usable by running a compilation that
        # #includes it.  But if we pass a -I/some/path in which Opencv
        # is not installed, the compiler will just skip this -I and
        # use other locations (either from CPPFLAGS, or from its list
        # of system include directories).  As a result we would use
        # header installed on the machine instead of the /some/path
        # specified by the user.  So in that precise case (trying
        # $opencv_inc), make sure the version.hpp exists.
        #
        # Use test -e as there can be symlinks.
        test -e "$opencv_inc/opencv2/core/version.hpp" || continue
        CPPFLAGS="$CPPFLAGS -I$opencv_inc"
      fi
      AC_COMPILE_IFELSE([], [opencv_cv_inc_path=yes], [opencv_cv_version=no])
      if test x"$opencv_cv_inc_path" = xyes; then
        if test x"$opencv_inc" != x; then
          opencv_cv_inc_path=$opencv_inc
        fi
        break 2
      fi
    done
    done
AC_LANG_POP([C++])dnl
    ])
    case $opencv_cv_inc_path in #(
      no)
        opencv_errmsg="cannot find Opencv headers version >= $opencv_version_req_string"
        m4_if([$2], [], [AC_MSG_ERROR( [$opencv_errmsg])],
                        [AC_MSG_NOTICE([$opencv_errmsg])])
        $2
        ;;#(
      yes)
        OPENCV_CPPFLAGS=
        ;;#(
      *)
        AC_SUBST([OPENCV_CPPFLAGS], ["-I$opencv_cv_inc_path"])dnl
        ;;
    esac
  if test x"$opencv_cv_inc_path" != xno; then
    AC_DEFINE([HAVE_OPENCV], [1],
      [Defined if the requested minimum OPENCV version is satisfied])
  
    AC_CACHE_CHECK([for Opencv's major version],
      [opencv_cv_major_version],
      [AC_SED_CPP([CV_VERSION_MAJOR],
         [@%:@include <opencv2/core/version.hpp>],
         [opencv_cv_major_version=$ppval],
         [AC_MSG_ERROR([not found])])])
      
    AC_CACHE_CHECK([for Opencv's minor version],
      [opencv_cv_minor_version],
      [AC_SED_CPP([CV_VERSION_MINOR],
         [@%:@include <opencv2/core/version.hpp>],
         [opencv_cv_minor_version=$ppval],
         [AC_MSG_ERROR([not found])])])
      
    opencv_cv_lib_version=${opencv_cv_major_version}_${opencv_cv_minor_version}
  fi
  CPPFLAGS=$opencv_save_CPPFLAGS
])# OPENCV_REQUIRE


# OPENCV_STATIC()
# --------------
# Add the "--enable-static-opencv" configure argument. If this argument is given
# on the command line, static versions of the libraries will be looked up.
AC_DEFUN([OPENCV_STATIC],
  [AC_ARG_ENABLE([static-opencv],
     [AS_HELP_STRING([--enable-static-opencv],
               [Prefer the static opencv libraries over the shared ones [no]])],
     [enable_static_opencv=yes],
     [enable_static_opencv=no])])# OPENCV_STATIC


# OPENCV_FIND_HEADER([HEADER-NAME], [ACTION-IF-NOT-FOUND], [ACTION-IF-FOUND])
# --------------------------------------------------------------------------
# Wrapper around AC_CHECK_HEADER for Opencv headers.  Useful to check for
# some parts of the Opencv library which are only made of headers and don't
# require linking (such as Opencv.Foreach).
#
# Default ACTION-IF-NOT-FOUND: Fail with a fatal error unless Opencv couldn't be
# found in the first place, in which case by default a notice is issued to the
# user.  Presumably if we haven't died already it's because it's OK to not have
# Opencv, which is why only a notice is issued instead of a hard error.
#
# Default ACTION-IF-FOUND: define the preprocessor symbol HAVE_<HEADER-NAME> in
# case of success # (where HEADER-NAME is written LIKE_THIS, e.g.,
# HAVE_OPENCV_FOREACH_HPP).
AC_DEFUN([OPENCV_FIND_HEADER],
[AC_REQUIRE([OPENCV_REQUIRE])dnl
if test x"$opencv_cv_inc_path" = xno; then
  m4_default([$2], [AC_MSG_NOTICE([Opencv not available, not searching for $1])])
else
AC_LANG_PUSH([C++])dnl
opencv_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $OPENCV_CPPFLAGS"
AC_CHECK_HEADER([$1],
  [m4_default([$3], [AC_DEFINE(AS_TR_CPP([HAVE_$1]), [1],
                               [Define to 1 if you have <$1>])])],
  [m4_default([$2], [AC_MSG_ERROR([cannot find $1])])])
CPPFLAGS=$opencv_save_CPPFLAGS
AC_LANG_POP([C++])dnl
fi
])# OPENCV_FIND_HEADER


# OPENCV_FIND_LIBS([COMPONENT-NAME], [CANDIDATE-LIB-NAMES],
#                 [PREFERRED-RT-OPT], [HEADER-NAME], [CXX-TEST],
#                 [CXX-PROLOGUE])
# --------------------------------------------------------------
# Look for the Opencv library COMPONENT-NAME (e.g., `thread', for
# libopencv_thread) under the possible CANDIDATE-LIB-NAMES (e.g.,
# "thread_win32 thread").  Check that HEADER-NAME works and check that
# libopencv_LIB-NAME can link with the code CXX-TEST.  The optional
# argument CXX-PROLOGUE can be used to include some C++ code before
# the `main' function.
#
# Invokes OPENCV_FIND_HEADER([HEADER-NAME]) (see above).
#
# Opencv libraries typically come compiled with several flavors (with different
# runtime options) so PREFERRED-RT-OPT is the preferred suffix.  A suffix is one
# or more of the following letters: sgdpn (in that order).  s = static
# runtime, d = debug build, g = debug/diagnostic runtime, p = STLPort build,
# n = (unsure) STLPort build without iostreams from STLPort (it looks like `n'
# must always be used along with `p').  Additionally, PREFERRED-RT-OPT can
# start with `mt-' to indicate that there is a preference for multi-thread
# builds.  Some sample values for PREFERRED-RT-OPT: (nothing), mt, d, mt-d, gdp
# ...  If you want to make sure you have a specific version of Opencv
# (eg, >= 1.33) you *must* invoke OPENCV_REQUIRE before this macro.
AC_DEFUN([OPENCV_FIND_LIBS],
[AC_REQUIRE([OPENCV_REQUIRE])dnl
AC_REQUIRE([_OPENCV_FIND_COMPILER_TAG])dnl
AC_REQUIRE([OPENCV_STATIC])dnl
AC_REQUIRE([_OPENCV_GUESS_WHETHER_TO_USE_MT])dnl
if test x"$opencv_cv_inc_path" = xno; then
  AC_MSG_NOTICE([Opencv not available, not searching for the Opencv $1 library])
else
dnl The else branch is huge and wasn't intended on purpose.
AC_LANG_PUSH([C++])dnl
AS_VAR_PUSHDEF([Opencv_lib], [opencv_cv_lib_$1])dnl
AS_VAR_PUSHDEF([Opencv_lib_LDFLAGS], [opencv_cv_lib_$1_LDFLAGS])dnl
AS_VAR_PUSHDEF([Opencv_lib_LDPATH], [opencv_cv_lib_$1_LDPATH])dnl
AS_VAR_PUSHDEF([Opencv_lib_LIBS], [opencv_cv_lib_$1_LIBS])dnl
OPENCV_FIND_HEADER([$4])
opencv_save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $OPENCV_CPPFLAGS"
AC_CACHE_CHECK([for the Opencv $1 library], [Opencv_lib],
               [_OPENCV_FIND_LIBS($@)])
case $Opencv_lib in #(
  (no) _AC_MSG_LOG_CONFTEST
    AC_MSG_ERROR([cannot find the flags to link with Opencv $1])
    ;;
esac
AC_SUBST(AS_TR_CPP([OPENCV_$1_LDFLAGS]), [$Opencv_lib_LDFLAGS])dnl
AC_SUBST(AS_TR_CPP([OPENCV_$1_LDPATH]), [$Opencv_lib_LDPATH])dnl
AC_SUBST([OPENCV_LDPATH], [$Opencv_lib_LDPATH])dnl
AC_SUBST(AS_TR_CPP([OPENCV_$1_LIBS]), [$Opencv_lib_LIBS])dnl
CPPFLAGS=$opencv_save_CPPFLAGS
AS_VAR_POPDEF([Opencv_lib])dnl
AS_VAR_POPDEF([Opencv_lib_LDFLAGS])dnl
AS_VAR_POPDEF([Opencv_lib_LDPATH])dnl
AS_VAR_POPDEF([Opencv_lib_LIBS])dnl
AC_LANG_POP([C++])dnl
fi
])


# OPENCV_FIND_LIB([LIB-NAME],
#                [PREFERRED-RT-OPT], [HEADER-NAME], [CXX-TEST],
#                [CXX-PROLOGUE])
# --------------------------------------------------------------
# Backward compatibility wrapper for OPENCV_FIND_LIBS.
AC_DEFUN([OPENCV_FIND_LIB],
[OPENCV_FIND_LIBS([$1], $@)])


# _OPENCV_FIND_LIBS([LIB-NAME], [CANDIDATE-LIB-NAMES],
#                 [PREFERRED-RT-OPT], [HEADER-NAME], [CXX-TEST],
#                 [CXX-PROLOGUE])
# --------------------------------------------------------------
# Real implementation of OPENCV_FIND_LIBS: rely on these local macros:
# Opencv_lib, Opencv_lib_LDFLAGS, Opencv_lib_LDPATH, Opencv_lib_LIBS
#
# The algorithm is as follows: first look for a given library name
# according to the user's PREFERRED-RT-OPT.  For each library name, we
# prefer to use the ones that carry the tag (toolset name).  Each
# library is searched through the various standard paths were Opencv is
# usually installed.  If we can't find the standard variants, we try
# to enforce -mt (for instance on MacOSX, libopencv_thread.dylib
# doesn't exist but there's -obviously- libopencv_thread-mt.dylib).
AC_DEFUN([_OPENCV_FIND_LIBS],
[opencv_lib=no
  case "$3" in #(
    (mt | mt-) opencv_mt=-mt; opencv_rtopt=;; #(
    (mt* | mt-*) opencv_mt=-mt; opencv_rtopt=`expr "X$3" : 'Xmt-*\(.*\)'`;; #(
    (*) opencv_mt=; opencv_rtopt=$3;;
  esac
  if test $enable_static_opencv = yes; then
    opencv_rtopt="s$opencv_rtopt"
  fi
  # Find the proper debug variant depending on what we've been asked to find.
  case $opencv_rtopt in #(
    (*d*) opencv_rt_d=$opencv_rtopt;; #(
    (*[[sgpn]]*) # Insert the `d' at the right place (in between `sg' and `pn')
      opencv_rt_d=`echo "$opencv_rtopt" | sed 's/\(s*g*\)\(p*n*\)/\1\2/'`;; #(
    (*) opencv_rt_d='-d';;
  esac
  # If the PREFERRED-RT-OPT are not empty, prepend a `-'.
  test -n "$opencv_rtopt" && opencv_rtopt="-$opencv_rtopt"
  $opencv_guess_use_mt && opencv_mt=-mt
  # Look for the abs path the static archive.
  # $libext is computed by Libtool but let's make sure it's non empty.
  test -z "$libext" &&
    AC_MSG_ERROR([the libext variable is empty, did you invoke Libtool?])
  opencv_save_ac_objext=$ac_objext
  # Generate the test file.
  AC_LANG_CONFTEST([AC_LANG_PROGRAM([@%:@include <$4>
$6], [$5])])
dnl Optimization hacks: compiling C++ is slow, especially with Opencv.  What
dnl we're trying to do here is guess the right combination of link flags
dnl (LIBS / LDFLAGS) to use a given library.  This can take several
dnl iterations before it succeeds and is thus *very* slow.  So what we do
dnl instead is that we compile the code first (and thus get an object file,
dnl typically conftest.o).  Then we try various combinations of link flags
dnl until we succeed to link conftest.o in an executable.  The problem is
dnl that the various TRY_LINK / COMPILE_IFELSE macros of Autoconf always
dnl remove all the temporary files including conftest.o.  So the trick here
dnl is to temporarily change the value of ac_objext so that conftest.o is
dnl preserved accross tests.  This is obviously fragile and I will burn in
dnl hell for not respecting Autoconf's documented interfaces, but in the
dnl mean time, it optimizes the macro by a factor of 5 to 30.
dnl Another small optimization: the first argument of AC_COMPILE_IFELSE left
dnl empty because the test file is generated only once above (before we
dnl start the for loops).
  AC_COMPILE_IFELSE([],
    [ac_objext=do_not_rm_me_plz],
    [AC_MSG_ERROR([cannot compile a test that uses Opencv $1])])
  ac_objext=$opencv_save_ac_objext
  opencv_failed_libs=
# Don't bother to ident the following nested for loops, only the 2
# innermost ones matter.
for opencv_lib_ in $2; do
for opencv_tag_ in '' -$opencv_cv_lib_tag; do
for opencv_ver_ in '' -$opencv_cv_lib_version; do
for opencv_mt_ in '' $opencv_mt -mt; do
for opencv_rtopt_ in $opencv_rtopt '' -d; do
  for opencv_lib in \
    opencv_$opencv_lib_$opencv_tag_$opencv_mt_$opencv_rtopt_$opencv_ver_ \
    opencv_$opencv_lib_$opencv_tag_$opencv_rtopt_$opencv_ver_ \
    opencv_$opencv_lib_$opencv_tag_$opencv_mt_$opencv_ver_ \
    opencv_$opencv_lib_$opencv_tag_$opencv_ver_
  do
    # Avoid testing twice the same lib
    case $opencv_failed_libs in #(
      (*@$opencv_lib@*) continue;;
    esac
    # If with_opencv is empty, we'll search in /lib first, which is not quite
    # right so instead we'll try to a location based on where the headers are.
    opencv_tmp_lib=$with_opencv
    test x"$with_opencv" = x && opencv_tmp_lib=${opencv_cv_inc_path%/include}
    for opencv_ldpath in "$opencv_tmp_lib/lib" '' \
             /opt/local/lib* /usr/local/lib* /opt/lib* /usr/lib* \
             "$with_opencv" C:/Opencv/lib /lib*
    do
      # Don't waste time with directories that don't exist.
      if test x"$opencv_ldpath" != x && test ! -e "$opencv_ldpath"; then
        continue
      fi
      opencv_save_LDFLAGS=$LDFLAGS
      # Are we looking for a static library?
      case $opencv_ldpath:$opencv_rtopt_ in #(
        (*?*:*s*) # Yes (Non empty opencv_ldpath + s in rt opt)
          Opencv_lib_LIBS="$opencv_ldpath/lib$opencv_lib.$libext"
          test -e "$Opencv_lib_LIBS" || continue;; #(
        (*) # No: use -lopencv_foo to find the shared library.
          Opencv_lib_LIBS="-l$opencv_lib";;
      esac
      opencv_save_LIBS=$LIBS
      LIBS="$Opencv_lib_LIBS $LIBS"
      test x"$opencv_ldpath" != x && LDFLAGS="$LDFLAGS -L$opencv_ldpath"
dnl First argument of AC_LINK_IFELSE left empty because the test file is
dnl generated only once above (before we start the for loops).
      _OPENCV_AC_LINK_IFELSE([],
                            [Opencv_lib=yes], [Opencv_lib=no])
      ac_objext=$opencv_save_ac_objext
      LDFLAGS=$opencv_save_LDFLAGS
      LIBS=$opencv_save_LIBS
      if test x"$Opencv_lib" = xyes; then
        # Check or used cached result of whether or not using -R or
        # -rpath makes sense.  Some implementations of ld, such as for
        # Mac OSX, require -rpath but -R is the flag known to work on
        # other systems.  https://github.com/tsuna/opencv.m4/issues/19
        AC_CACHE_VAL([opencv_cv_rpath_link_ldflag],
          [case $opencv_ldpath in
           '') # Nothing to do.
             opencv_cv_rpath_link_ldflag=
             opencv_rpath_link_ldflag_found=yes;;
           *)
            for opencv_cv_rpath_link_ldflag in -Wl,-R, -Wl,-rpath,; do
              LDFLAGS="$opencv_save_LDFLAGS -L$opencv_ldpath $opencv_cv_rpath_link_ldflag$opencv_ldpath"
              LIBS="$opencv_save_LIBS $Opencv_lib_LIBS"
              _OPENCV_AC_LINK_IFELSE([],
                [opencv_rpath_link_ldflag_found=yes
                break],
                [opencv_rpath_link_ldflag_found=no])
            done
            ;;
          esac
          AS_IF([test "x$opencv_rpath_link_ldflag_found" != "xyes"],
            [AC_MSG_ERROR([Unable to determine whether to use -R or -rpath])])
          LDFLAGS=$opencv_save_LDFLAGS
          LIBS=$opencv_save_LIBS
          ])
        test x"$opencv_ldpath" != x &&
          Opencv_lib_LDFLAGS="-L$opencv_ldpath $opencv_cv_rpath_link_ldflag$opencv_ldpath"
        Opencv_lib_LDPATH="$opencv_ldpath"
        break 7
      else
        opencv_failed_libs="$opencv_failed_libs@$opencv_lib@"
      fi
    done
  done
done
done
done
done
done # opencv_lib_
rm -f conftest.$ac_objext
])



# --------------------------------------- #
# Checks for the various Opencv libraries. #
# --------------------------------------- #

# List of opencv libraries: http://www.opencv.org/libs/libraries.htm
# The page http://beta.opencv.org/doc/libs is useful: it gives the first release
# version of each library (among other things).

# OPENCV_DEFUN(LIBRARY, CODE)
# --------------------------
# Define OPENCV_<LIBRARY-UPPERCASE> as a macro that runs CODE.
#
# Use indir to avoid the warning on underquoted macro name given to AC_DEFUN.
m4_define([OPENCV_DEFUN],
[m4_indir([AC_DEFUN],
          m4_toupper([OPENCV_$1]),
[m4_pushdef([OPENCV_Library], [$1])dnl
$2
m4_popdef([OPENCV_Library])dnl
])
])

OPENCV_DEFUN([Core],
  [opencv_core_save_LIBS=$LIBS
   opencv_core_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_CORE_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_CORE_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_CORE_LDFLAGS"
   OPENCV_FIND_LIB([core], [$1], [opencv2/core.hpp], [cv::Mat ignore;])
   LIBS=$opencv_core_save_LIBS
   LDFLAGS=$opencv_core_save_LDFLAGS
])# OPENCV_CORE

OPENCV_DEFUN([Features2d],
  [opencv_features2d_save_LIBS=$LIBS
   opencv_features2d_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_FEATURES2D_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_FEATURES2D_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_FEATURES2D_LDFLAGS"
   OPENCV_FIND_LIB([features2d], [$1], [opencv2/features2d.hpp], [cv::todo fixme;])
   LIBS=$opencv_features2d_save_LIBS
   LDFLAGS=$opencv_features2d_save_LDFLAGS
])# OPENCV_FEATURES2D

OPENCV_DEFUN([Flann],
  [opencv_flann_save_LIBS=$LIBS
   opencv_flann_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_FLANN_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_FLANN_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_FLANN_LDFLAGS"
   OPENCV_FIND_LIB([flann], [$1], [opencv2/flann.hpp], [cv::todo fixme;])
   LIBS=$opencv_flann_save_LIBS
   LDFLAGS=$opencv_flann_save_LDFLAGS
])# OPENCV_FLANN

OPENCV_DEFUN([Highgui],
  [opencv_highgui_save_LIBS=$LIBS
   opencv_highgui_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_HIGHGUI_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_HIGHGUI_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_HIGHGUI_LDFLAGS"
   OPENCV_FIND_LIB([highgui], [$1],
     [opencv2/highgui.hpp],
     [return !&cv::moveWindow |
             !&cv::namedWindow |
             !&cv::updateWindow |
             !&cv::destroyWindow;])
   LIBS=$opencv_highgui_save_LIBS
   LDFLAGS=$opencv_highgui_save_LDFLAGS
])# OPENCV_HIGHGUI

OPENCV_DEFUN([Imgcodecs],
  [opencv_imgcodecs_save_LIBS=$LIBS
   opencv_imgcodecs_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_IMGCODECS_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_IMGCODECS_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_IMGCODECS_LDFLAGS"
   OPENCV_FIND_LIB([imgcodecs], [$1],
     [opencv2/imgcodecs.hpp],
     [return !&cv::imread | !&cv::imwrite;])
   LIBS=$opencv_imgcodecs_save_LIBS
   LDFLAGS=$opencv_imgcodecs_save_LDFLAGS
])# OPENCV_IMGCODECS

OPENCV_DEFUN([Imgproc],
  [opencv_imgproc_save_LIBS=$LIBS
   opencv_imgproc_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_IMGPROC_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_IMGPROC_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_IMGPROC_LDFLAGS"
   OPENCV_FIND_LIB([imgproc], [$1], [opencv2/imgproc.hpp], [return !&cv::Sobel;])
   LIBS=$opencv_imgproc_save_LIBS
   LDFLAGS=$opencv_imgproc_save_LDFLAGS
])# OPENCV_IMGPROC

OPENCV_DEFUN([Ml],
  [opencv_ml_save_LIBS=$LIBS
   opencv_ml_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_ML_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_ML_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_ML_LDFLAGS"
   OPENCV_FIND_LIB([ml], [$1], [opencv2/ml.hpp], [cv::todo fixme;])
   LIBS=$opencv_ml_save_LIBS
   LDFLAGS=$opencv_ml_save_LDFLAGS
])# OPENCV_ML

OPENCV_DEFUN([Objdetect],
  [opencv_objdetect_save_LIBS=$LIBS
   opencv_objdetect_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_OBJDETECT_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_OBJDETECT_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_OBJDETECT_LDFLAGS"
   OPENCV_FIND_LIB([objdetect], [$1], [opencv2/objdetect.hpp], [cv::todo fixme;])
   LIBS=$opencv_objdetect_save_LIBS
   LDFLAGS=$opencv_objdetect_save_LDFLAGS
])# OPENCV_OBJDETECT

OPENCV_DEFUN([Photo],
  [opencv_photo_save_LIBS=$LIBS
   opencv_photo_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_PHOTO_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_PHOTO_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_PHOTO_LDFLAGS"
   OPENCV_FIND_LIB([photo], [$1], [opencv2/photo.hpp], [cv::todo fixme;])
   LIBS=$opencv_photo_save_LIBS
   LDFLAGS=$opencv_photo_save_LDFLAGS
])# OPENCV_PHOTO

OPENCV_DEFUN([Shape],
  [opencv_shape_save_LIBS=$LIBS
   opencv_shape_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_SHAPE_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_SHAPE_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_SHAPE_LDFLAGS"
   OPENCV_FIND_LIB([shape], [$1], [opencv2/shape.hpp], [cv::todo fixme;])
   LIBS=$opencv_shape_save_LIBS
   LDFLAGS=$opencv_shape_save_LDFLAGS
])# OPENCV_SHAPE

OPENCV_DEFUN([Stitching],
  [opencv_stitching_save_LIBS=$LIBS
   opencv_stitching_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_STITCHING_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_STITCHING_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_STITCHING_LDFLAGS"
   OPENCV_FIND_LIB([stitching], [$1], [opencv2/stitching.hpp], [cv::todo fixme;])
   LIBS=$opencv_stitching_save_LIBS
   LDFLAGS=$opencv_stitching_save_LDFLAGS
])# OPENCV_STITCHING

OPENCV_DEFUN([Superres],
  [opencv_superres_save_LIBS=$LIBS
   opencv_superres_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_SUPERRES_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_SUPERRES_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_SUPERRES_LDFLAGS"
   OPENCV_FIND_LIB([superres], [$1], [opencv2/superres.hpp], [cv::todo fixme;])
   LIBS=$opencv_superres_save_LIBS
   LDFLAGS=$opencv_superres_save_LDFLAGS
])# OPENCV_SUPERRES

OPENCV_DEFUN([Video],
  [opencv_video_save_LIBS=$LIBS
   opencv_video_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_VIDEO_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_VIDEO_LIBS $OPENCV_CORE_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_VIDEO_LDFLAGS"
   OPENCV_FIND_LIB([video], [$1], [opencv2/video.hpp], [cv::KalmanFilter ignore;])
   LIBS=$opencv_video_save_LIBS
   LDFLAGS=$opencv_video_save_LDFLAGS
])# OPENCV_VIDEO

OPENCV_DEFUN([Videoio],
  [opencv_videoio_save_LIBS=$LIBS
   opencv_videoio_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_VIDEOIO_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_VIDEOIO_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_VIDEOIO_LDFLAGS"
   OPENCV_FIND_LIB([videoio], [$1], [opencv2/videoio.hpp], [cv::VideoCapture ignore;])
   LIBS=$opencv_videoio_save_LIBS
   LDFLAGS=$opencv_videoio_save_LDFLAGS
])# OPENCV_VIDEOIO

OPENCV_DEFUN([Videostab],
  [opencv_videostab_save_LIBS=$LIBS
   opencv_videostab_save_LDFLAGS=$LDFLAGS
   m4_pattern_allow([^OPENCV_VIDEOSTAB_(LIBS|LDFLAGS)$])dnl
   LIBS="$LIBS $OPENCV_VIDEOSTAB_LIBS"
   LDFLAGS="$LDFLAGS $OPENCV_VIDEOSTAB_LDFLAGS"
   OPENCV_FIND_LIB([videostab], [$1], [opencv2/videostab.hpp], [cv::todo fixme;])
   LIBS=$opencv_videostab_save_LIBS
   LDFLAGS=$opencv_videostab_save_LDFLAGS
])# OPENCV_VIDEOSTAB


# ----------------- #
# Internal helpers. #
# ----------------- #

# _OPENCV_PTHREAD_FLAG()
# ---------------------
# Internal helper for OPENCV_THREAD.  Computes opencv_cv_pthread_flag
# which must be used in CPPFLAGS and LIBS.
#
# Yes, we *need* to put the -pthread thing in CPPFLAGS because with GCC3,
# opencv/thread.hpp will trigger a #error if -pthread isn't used:
#   opencv/config/requires_threads.hpp:47:5: #error "Compiler threading support
#   is not turned on. Please set the correct command line options for
#   threading: -pthread (Linux), -pthreads (Solaris) or -mthreads (Mingw32)"
#
# Based on ACX_PTHREAD: http://autoconf-archive.cryp.to/acx_pthread.html
AC_DEFUN([_OPENCV_PTHREAD_FLAG],
[AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_LANG_PUSH([C++])dnl
AC_CACHE_CHECK([for the flags needed to use pthreads], [opencv_cv_pthread_flag],
[ opencv_cv_pthread_flag=
  # The ordering *is* (sometimes) important.  Some notes on the
  # individual items follow:
  # (none): in case threads are in libc; should be tried before -Kthread and
  #       other compiler flags to prevent continual compiler warnings
  # -lpthreads: AIX (must check this before -lpthread)
  # -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
  # -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
  # -llthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
  # -pthread: GNU Linux/GCC (kernel threads), BSD/GCC (userland threads)
  # -pthreads: Solaris/GCC
  # -mthreads: MinGW32/GCC, Lynx/GCC
  # -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
  #      doesn't hurt to check since this sometimes defines pthreads too;
  #      also defines -D_REENTRANT)
  #      ... -mt is also the pthreads flag for HP/aCC
  # -lpthread: GNU Linux, etc.
  # --thread-safe: KAI C++
  case $host_os in #(
    *solaris*)
      # On Solaris (at least, for some versions), libc contains stubbed
      # (non-functional) versions of the pthreads routines, so link-based
      # tests will erroneously succeed.  (We need to link with -pthreads/-mt/
      # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
      # a function called by this macro, so we could check for that, but
      # who knows whether they'll stub that too in a future libc.)  So,
      # we'll just look for -pthreads and -lpthread first:
      opencv_pthread_flags="-pthreads -lpthread -mt -pthread";; #(
    *)
      opencv_pthread_flags="-lpthreads -Kthread -kthread -llthread -pthread \
                           -pthreads -mthreads -lpthread --thread-safe -mt";;
  esac
  # Generate the test file.
  AC_LANG_CONFTEST([AC_LANG_PROGRAM([@%:@include <pthread.h>],
    [pthread_t th; pthread_join(th, 0);
    pthread_attr_init(0); pthread_cleanup_push(0, 0);
    pthread_create(0,0,0,0); pthread_cleanup_pop(0);])])
  for opencv_pthread_flag in '' $opencv_pthread_flags; do
    opencv_pthread_ok=false
dnl Re-use the test file already generated.
    opencv_pthreads__save_LIBS=$LIBS
    LIBS="$LIBS $opencv_pthread_flag"
    AC_LINK_IFELSE([],
      [if grep ".*$opencv_pthread_flag" conftest.err; then
         echo "This flag seems to have triggered warnings" >&AS_MESSAGE_LOG_FD
       else
         opencv_pthread_ok=:; opencv_cv_pthread_flag=$opencv_pthread_flag
       fi])
    LIBS=$opencv_pthreads__save_LIBS
    $opencv_pthread_ok && break
  done
])
AC_LANG_POP([C++])dnl
])# _OPENCV_PTHREAD_FLAG


# _OPENCV_gcc_test(MAJOR, MINOR)
# -----------------------------
# Internal helper for _OPENCV_FIND_COMPILER_TAG.
m4_define([_OPENCV_gcc_test],
["defined __GNUC__ && __GNUC__ == $1 && __GNUC_MINOR__ == $2 && !defined __ICC @ gcc$1$2"])dnl

# _OPENCV_mingw_test(MAJOR, MINOR)
# -----------------------------
# Internal helper for _OPENCV_FIND_COMPILER_TAG.
m4_define([_OPENCV_mingw_test],
["defined __GNUC__ && __GNUC__ == $1 && __GNUC_MINOR__ == $2 && !defined __ICC && \
  (defined WIN32 || defined WINNT || defined _WIN32 || defined __WIN32 \
         || defined __WIN32__ || defined __WINNT || defined __WINNT__) @ mgw$1$2"])dnl


# _OPENCV_FIND_COMPILER_TAG()
# --------------------------
# Internal.  When Opencv is installed without --layout=system, each library
# filename will hold a suffix that encodes the compiler used during the
# build.  The Opencv build system seems to call this a `tag'.
AC_DEFUN([_OPENCV_FIND_COMPILER_TAG],
[AC_REQUIRE([AC_PROG_CXX])dnl
AC_REQUIRE([AC_CANONICAL_HOST])dnl
AC_CACHE_CHECK([for the toolset name used by Opencv for $CXX],
               [opencv_cv_lib_tag],
[opencv_cv_lib_tag=unknown
if test x$opencv_cv_inc_path != xno; then
  AC_LANG_PUSH([C++])dnl
  # The following tests are mostly inspired by opencv/config/auto_link.hpp
  # The list is sorted to most recent/common to oldest compiler (in order
  # to increase the likelihood of finding the right compiler with the
  # least number of compilation attempt).
  # Beware that some tests are sensible to the order (for instance, we must
  # look for MinGW before looking for GCC3).
  # I used one compilation test per compiler with a #error to recognize
  # each compiler so that it works even when cross-compiling (let me know
  # if you know a better approach).
  # Known missing tags (known from Opencv's tools/build/v2/tools/common.jam):
  #   como, edg, kcc, bck, mp, sw, tru, xlc
  # I'm not sure about my test for `il' (be careful: Intel's ICC pre-defines
  # the same defines as GCC's).
  for i in \
    _OPENCV_mingw_test(5, 2) \
    _OPENCV_gcc_test(5, 2) \
    _OPENCV_mingw_test(5, 1) \
    _OPENCV_gcc_test(5, 1) \
    _OPENCV_mingw_test(5, 0) \
    _OPENCV_gcc_test(5, 0) \
    _OPENCV_mingw_test(4, 10) \
    _OPENCV_gcc_test(4, 10) \
    _OPENCV_mingw_test(4, 9) \
    _OPENCV_gcc_test(4, 9) \
    _OPENCV_mingw_test(4, 8) \
    _OPENCV_gcc_test(4, 8) \
    _OPENCV_mingw_test(4, 7) \
    _OPENCV_gcc_test(4, 7) \
    _OPENCV_mingw_test(4, 6) \
    _OPENCV_gcc_test(4, 6) \
    _OPENCV_mingw_test(4, 5) \
    _OPENCV_gcc_test(4, 5) \
    _OPENCV_mingw_test(4, 4) \
    _OPENCV_gcc_test(4, 4) \
    _OPENCV_mingw_test(4, 3) \
    _OPENCV_gcc_test(4, 3) \
    _OPENCV_mingw_test(4, 2) \
    _OPENCV_gcc_test(4, 2) \
    _OPENCV_mingw_test(4, 1) \
    _OPENCV_gcc_test(4, 1) \
    _OPENCV_mingw_test(4, 0) \
    _OPENCV_gcc_test(4, 0) \
    "defined __GNUC__ && __GNUC__ == 3 && !defined __ICC \
     && (defined WIN32 || defined WINNT || defined _WIN32 || defined __WIN32 \
         || defined __WIN32__ || defined __WINNT || defined __WINNT__) @ mgw" \
    _OPENCV_gcc_test(3, 4) \
    _OPENCV_gcc_test(3, 3) \
    "defined _MSC_VER && _MSC_VER >= 1500 @ vc90" \
    "defined _MSC_VER && _MSC_VER == 1400 @ vc80" \
    _OPENCV_gcc_test(3, 2) \
    "defined _MSC_VER && _MSC_VER == 1310 @ vc71" \
    _OPENCV_gcc_test(3, 1) \
    _OPENCV_gcc_test(3, 0) \
    "defined __BORLANDC__ @ bcb" \
    "defined __ICC && (defined __unix || defined __unix__) @ il" \
    "defined __ICL @ iw" \
    "defined _MSC_VER && _MSC_VER == 1300 @ vc7" \
    _OPENCV_gcc_test(2, 95) \
    "defined __MWERKS__ && __MWERKS__ <= 0x32FF @ cw9" \
    "defined _MSC_VER && _MSC_VER < 1300 && !defined UNDER_CE @ vc6" \
    "defined _MSC_VER && _MSC_VER < 1300 && defined UNDER_CE @ evc4" \
    "defined __MWERKS__ && __MWERKS__ <= 0x31FF @ cw8"
  do
    opencv_tag_test=`expr "X$i" : 'X\([[^@]]*\) @ '`
    opencv_tag=`expr "X$i" : 'X[[^@]]* @ \(.*\)'`
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if $opencv_tag_test
/* OK */
#else
# error $opencv_tag_test
#endif
]])], [opencv_cv_lib_tag=$opencv_tag; break], [])
  done
AC_LANG_POP([C++])dnl
fi])dnl end of AC_CACHE_CHECK
])# _OPENCV_FIND_COMPILER_TAG


# _OPENCV_GUESS_WHETHER_TO_USE_MT()
# --------------------------------
# Compile a small test to try to guess whether we should favor MT (Multi
# Thread) flavors of Opencv.  Sets opencv_guess_use_mt accordingly.
AC_DEFUN([_OPENCV_GUESS_WHETHER_TO_USE_MT],
[# Check whether we do better use `mt' even though we weren't ask to.
AC_LANG_PUSH([C++])dnl
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#if defined _REENTRANT || defined _MT || defined __MT__
/* use -mt */
#else
# error MT not needed
#endif
]])], [opencv_guess_use_mt=:], [opencv_guess_use_mt=false])
AC_LANG_POP([C++])dnl
])

# _OPENCV_AC_LINK_IFELSE(PROGRAM, [ACTION-IF-TRUE], [ACTION-IF-FALSE])
# -------------------------------------------------------------------
# Fork of _AC_LINK_IFELSE that preserves conftest.o across calls.  Fragile,
# will break when Autoconf changes its internals.  Requires that you manually
# rm -f conftest.$ac_objext in between to really different tests, otherwise
# you will try to link a conftest.o left behind by a previous test.
# Used to aggressively optimize OPENCV_FIND_LIB (see the big comment in this
# macro).
#
# Don't use "break" in the actions, as it would short-circuit some code
# this macro runs after the actions.
m4_define([_OPENCV_AC_LINK_IFELSE],
[m4_ifvaln([$1], [AC_LANG_CONFTEST([$1])])dnl
rm -f conftest$ac_exeext
opencv_save_ac_ext=$ac_ext
opencv_use_source=:
# If we already have a .o, re-use it.  We change $ac_ext so that $ac_link
# tries to link the existing object file instead of compiling from source.
test -f conftest.$ac_objext && ac_ext=$ac_objext && opencv_use_source=false &&
  _AS_ECHO_LOG([re-using the existing conftest.$ac_objext])
AS_IF([_AC_DO_STDERR($ac_link) && {
         test -z "$ac_[]_AC_LANG_ABBREV[]_werror_flag" ||
         test ! -s conftest.err
       } && test -s conftest$ac_exeext && {
         test "$cross_compiling" = yes ||
         $as_executable_p conftest$ac_exeext
dnl FIXME: use AS_TEST_X instead when 2.61 is widespread enough.
       }],
      [$2],
      [if $opencv_use_source; then
         _AC_MSG_LOG_CONFTEST
       fi
       $3])
ac_objext=$opencv_save_ac_objext
ac_ext=$opencv_save_ac_ext
dnl Delete also the IPA/IPO (Inter Procedural Analysis/Optimization)
dnl information created by the PGI compiler (conftest_ipa8_conftest.oo),
dnl as it would interfere with the next link command.
rm -f core conftest.err conftest_ipa8_conftest.oo \
      conftest$ac_exeext m4_ifval([$1], [conftest.$ac_ext])[]dnl
])# _OPENCV_AC_LINK_IFELSE

# Local Variables:
# mode: autoconf
# End:
