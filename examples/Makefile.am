# -*- mode: makefile -*-

EXTRA_DIST = 

include $(top_srcdir)/Makefile.common

bin_PROGRAMS = oclbp olbp varlbp

oclbp_SOURCES = oclbp.cpp options.cpp
oclbp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)

olbp_SOURCES = olbp.cpp options.cpp
olbp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)

varlbp_SOURCES = varlbp.cpp options.cpp
varlbp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)
