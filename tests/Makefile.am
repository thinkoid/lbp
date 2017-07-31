# -*- mode: makefile -*-

SUBDIRS = perf

EXTRA_DIST = 

include $(top_srcdir)/Makefile.common

LIBS += $(BOOST_UNIT_TEST_FRAMEWORK_LIBS)

if DARWIN
  # Because stupid
  LIBS += -lc++abi
endif

TESTS = placeholder
check_PROGRAMS = placeholder

placeholder_SOURCES = placeholder.cpp
placeholder_LDADD = $(LIBS)
