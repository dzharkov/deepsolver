
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)

bin_PROGRAMS = ds ds-repo

ds_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_DEPENDENCIES = $(ds_LDADD)

ds_SOURCES=\
CmdLineParser.cpp \
Messages.cpp \
IndexFetchProgress.cpp \
ds.cpp

ds_repo_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_repo_DEPENDENCIES = $(ds_repo_LDADD)

ds_repo_SOURCES=\
ds-repo.cpp
