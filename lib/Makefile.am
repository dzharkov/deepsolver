

AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)
LIBS += -lrpm -lcurl
LIBdir=${libdir}

LIB_LTLIBRARIES = libdeepsolver.la

libdeepsolver_ladir = $(includedir)

libdeepsolver_la_SOURCES = \
ConfigCenter.cpp \
ConfigFile.cpp \
IndexCore.cpp \
IndexFetch.cpp \
InfoCore.cpp \
logging.cpp \
OperationCore.cpp \
os/Directory.cpp \
os/File.cpp \
io/PackageScopeContentBuilder.cpp \
transact/PackageScopeContent.cpp \
io/PackageScopeContentLoader.cpp \
Pkg.cpp \
PkgUtils.cpp \
ProvidePriorityList.cpp \
repo/InfoFileReader.cpp \
repo/RepoIndexInfoFile.cpp \
repo/TextFormatReader.cpp \
repo/RepoIndexTextFormatWriter.cpp \
Repository.cpp \
RequireFilter.cpp \
RpmBackEnd.cpp \
rpm/Rpmdb.cpp \
rpm/RpmFileHeaderReader.cpp \
rpm/RpmHeaderReading.cpp \
rpm/vercmp.cpp \
transact/InstalledReferences.cpp \
transact/PackageScope.cpp \
transact/ProvideMap.cpp \
transact/StrictSolver.cpp \
transact/TaskPreprocessor.cpp \
utils/CurlInterface.cpp \
utils/GZipInterface.cpp \
utils/MD5.cpp \
utils/string-utils.cpp \
utils/TextFiles.cpp \
utils/TinyFileDownload.cpp \
version.cpp 

libdeepsolver_la_HEADERS = \
AbstractConsoleMessages.h \
AbstractPackageBackEnd.h \
AbstractTaskSolver.h \
AbstractWarningHandler.h \
ConfigCenter.h \
ConfigData.h \
ConfigFile.h \
DeepsolverException.h \
deepsolver.h \
DeepsolverTypes.h \
DefaultValues.h \
IndexCoreException.h \
IndexCore.h \
IndexFetch.h \
InfoCore.h \
logging.h \
OperationCore.h \
os/Directory.h \
os/File.h \
os/SystemException.h \
os/system.h \
io/PackageScopeContentBuilder.h \
transact/PackageScopeContent.h \
io/PackageScopeContentLoader.h \
Pkg.h \
PkgUtils.h \
ProvidePriorityList.h \
repo/InfoFileReader.h \
repo/RepoIndexInfoFile.h \
repo/TextFormatReader.h \
repo/RepoIndexTextFormatWriter.h \
Repository.h \
RequireFilter.h \
rpm/RpmException.h \
rpm/Rpmdb.h \
rpm/RpmFile.h \
rpm/RpmFileHeaderReader.h \
rpm/RpmHeaderReading.h \
rpm/vercmp.h \
transact/TaskException.h \
transact/InstalledReferences.h \
transact/PackageScope.h \
transact/ProvideMap.h \
transact/TaskPreprocessor.h \
UserTask.h \
utils/CurlInterface.h \
utils/DateTime.h \
utils/GZipInterface.h \
utils/MD5.h \
utils/string-utils.h \
utils/TextFiles.h \
utils/TinyFileDownload.h \
version.h 
