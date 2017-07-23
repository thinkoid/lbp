# -*- mode: makefile -*-

EXTRA_DIST = 

include $(top_srcdir)/Makefile.common

bin_PROGRAMS = cslbp csldp cssiltp2 oclbp olbp siltp varlbp

cslbp_SOURCES = cslbp.cpp options.cpp
cslbp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)

csldp_SOURCES = csldp.cpp options.cpp
csldp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)

cssiltp2_SOURCES = cssiltp2.cpp options.cpp
cssiltp2_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)

oclbp_SOURCES = oclbp.cpp options.cpp
oclbp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)

olbp_SOURCES = olbp.cpp options.cpp
olbp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)

siltp_SOURCES = siltp.cpp options.cpp
siltp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)

varlbp_SOURCES = varlbp.cpp options.cpp
varlbp_LDADD = $(top_srcdir)/src/liblbp.la $(LIBS)
