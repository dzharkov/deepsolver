
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)
LIBS += -lrpm -lz

bin_PROGRAMS = genbasedir ds

genbasedir_LDADD = \
$(top_srcdir)/lib/libdeepsolver.a  \
$(top_srcdir)/lib/rpm/libdeepsolverrpm.a  \
$(top_srcdir)/lib/os/libdeepsolveros.a 
genbasedir_DEPENDENCIES = $(genbasedir_LDADD)

ds_LDADD = \
$(top_srcdir)/lib/libdeepsolver.a  \
$(top_srcdir)/lib/rpm/libdeepsolverrpm.a  \
$(top_srcdir)/lib/os/libdeepsolveros.a 
ds_DEPENDENCIES = $(ds_LDADD)

genbasedir_SOURCES=\
genbasedir.cpp

ds_SOURCES=\
ds.cpp
