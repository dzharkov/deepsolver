
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)

bin_PROGRAMS = ds

ds_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_DEPENDENCIES = $(ds_LDADD)

ds_SOURCES=\
Messages.cpp \
ds.cpp
