
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)

bin_PROGRAMS = ds ds-repo ds-patch ds-provides

ds_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_DEPENDENCIES = $(ds_LDADD)

ds_SOURCES=\
CliParser.cpp \
CmdLineParser.cpp \
Messages.cpp \
IndexFetchProgress.cpp \
ds.cpp

ds_repo_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_repo_DEPENDENCIES = $(ds_repo_LDADD)

ds_repo_SOURCES=\
CliParser.cpp \
ds-repo.cpp

ds_patch_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_patch_DEPENDENCIES = $(ds_patch_LDADD)

ds_patch_SOURCES=\
CliParser.cpp \
ds-patch.cpp

ds_provides_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_provides_DEPENDENCIES = $(ds_provides_LDADD)

ds_provides_SOURCES=\
CliParser.cpp \
ds-references.cpp
