
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)

bin_PROGRAMS = ds-install ds-update ds-repo ds-patch ds-provides

ds_install_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_install_DEPENDENCIES = $(ds_install_LDADD)

ds_install_SOURCES=\
CliParser.cpp \
Messages.cpp \
ds-install.cpp

ds_update_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_update_DEPENDENCIES = $(ds_update_LDADD)

ds_update_SOURCES=\
CliParser.cpp \
Messages.cpp \
IndexFetchProgress.cpp \
ds-update.cpp

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
ds-provides.cpp
