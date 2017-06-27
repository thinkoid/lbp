# -*- mode: makefile -*-

EXTRA_DIST = 

include $(top_srcdir)/Makefile.common

LIBS += $(BOOST_UNIT_TEST_FRAMEWORK_LIBS) $(top_srcdir)/src/liblbp.la

if DARWIN
  # Because stupid
  LIBS += -lc++abi
endif

TESTS = placeholder
check_PROGRAMS = placeholder

placeholder_SOURCES = placeholder.cpp
placeholder_LDADD = $(LIBS)

bin_PROGRAMS = lbp_perf
lbp_perf_SOURCES = lbp_perf.cpp
