
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)

bin_PROGRAMS = ds-install ds-remove ds-conf ds-update ds-repo ds-patch ds-provides ds-require

ds_install_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_install_DEPENDENCIES = $(ds_install_LDADD)
ds_install_SOURCES=\
CliParser.cpp \
Messages.cpp \
TransactionProgress.cpp \
ds-install.cpp

ds_remove_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_remove_DEPENDENCIES = $(ds_remove_LDADD)
ds_remove_SOURCES=\
CliParser.cpp \
Messages.cpp \
TransactionProgress.cpp \
ds-remove.cpp

ds_update_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_update_DEPENDENCIES = $(ds_update_LDADD)
ds_update_SOURCES=\
CliParser.cpp \
Messages.cpp \
IndexFetchProgress.cpp \
ds-update.cpp

ds_conf_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_conf_DEPENDENCIES = $(ds_conf_LDADD)
ds_conf_SOURCES=\
CliParser.cpp \
Messages.cpp \
ds-conf.cpp

ds_require_LDADD = \
$(top_srcdir)/lib/libdeepsolver.la
ds_require_DEPENDENCIES = $(ds_require_LDADD)
ds_require_SOURCES=\
CliParser.cpp \
Messages.cpp \
ds-require.cpp

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
