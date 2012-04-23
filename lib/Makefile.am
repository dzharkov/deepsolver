
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)
LIBS += -lrpm 
LIBdir=${libdir}

LIB_LTLIBRARIES = libdeepsolver.la

libdeepsolver_ladir = $(includedir)

libdeepsolver_la_SOURCES = \
ConfigCenter.cpp \
ConfigFile.cpp \
IndexCore.cpp \
OperationCore.cpp \
os/Directory.cpp \
os/File.cpp \
PackageScopeContentBuilder.cpp \
PackageScopeContent.cpp \
PackageScopeContentLoader.cpp \
Pkg.cpp \
ProvidePriorityList.cpp \
repo/RepoIndexInfoFile.cpp \
repo/RepoIndexTextFormatReader.cpp \
repo/RepoIndexTextFormatWriter.cpp \
RequireFilter.cpp \
rpm/RpmFileHeaderReader.cpp \
rpm/vercmp.cpp \
transact/PackageScope.cpp \
transact/TaskPreprocessor.cpp \
utils/GZipInterface.cpp \
utils/logging.cpp \
utils/MD5.cpp \
utils/string-utils.cpp \
utils/TextFiles.cpp \
utils/ZLibInterface.cpp \
version.cpp 

libdeepsolver_la_HEADERS = \
AbstractConsoleMessages.h \
AbstractWarningHandler.h \
ConfigData.h \
ConfigCenter.h \
ConfigFile.h \
DeepsolverException.h \
deepsolver.h \
DeepsolverTypes.h \
DefaultValues.h \
IndexCoreException.h \
IndexCore.h \
OperationCore.h \
os/Directory.h \
os/File.h \
os/SystemException.h \
os/system.h \
PackageScopeContentBuilder.h \
PackageScopeContent.h \
PackageScopeContentLoader.h \
Pkg.h \
ProvidePriorityList.h \
repo/RepoIndexInfoFile.h \
repo/RepoIndexTextFormatReader.h \
repo/RepoIndexTextFormatWriter.h \
RequireFilter.h \
rpm/RpmException.h \
rpm/RpmFile.h \
rpm/RpmFileHeaderReader.h \
rpm/vercmp.h \
TaskException.h \
transact/PackageScope.h \
transact/TaskPreprocessor.h \
UserTask.h \
utils/DateTime.h \
utils/GZipInterface.h \
utils/logging.h \
utils/MD5.h \
utils/string-utils.h \
utils/TextFiles.h \
utils/ZLibInterface.h \
version.h 
