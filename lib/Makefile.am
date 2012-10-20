
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)
LIBS += -lrpm -lcurl
LIBdir=${libdir}
LDFLAGS=-version-info ${LT_CURRENT}:$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE)

LIB_LTLIBRARIES = libdeepsolver.la

libdeepsolver_ladir = $(includedir)

libdeepsolver_la_SOURCES = \
ConfigCenter.cpp \
ConfigFile.cpp \
IndexCore.cpp \
IndexFetch.cpp \
InfoCore.cpp \
io/PackageScopeContentBuilder.cpp \
io/PackageScopeContentLoader.cpp \
logging.cpp \
OperationCore.cpp \
os/Directory.cpp \
os/File.cpp \
Pkg.cpp \
PkgUtils.cpp \
ProvidePriorityList.cpp \
repo/InfoFileReader.cpp \
repo/PkgSection.cpp \
repo/RepoParams.cpp \
Repository.cpp \
repo/TextFormatSectionReader.cpp \
RequireFilter.cpp \
RpmBackEnd.cpp \
rpm/Rpmdb.cpp \
rpm/RpmFileHeaderReader.cpp \
rpm/RpmHeaderReading.cpp \
rpm/vercmp.cpp \
transact/InstalledReferences.cpp \
transact/PackageScopeContent.cpp \
transact/PackageScope.cpp \
transact/ProvideMap.cpp \
transact/sat.cpp \
transact/StrictSolver.cpp \
utils/CurlInterface.cpp \
utils/GzipInterface.cpp \
utils/Md5.cpp \
utils/Md5File.cpp \
utils/RegExp.cpp \
utils/string-utils.cpp \
utils/TinyFileDownload.cpp \
version.cpp

libdeepsolver_la_HEADERS = \
AbstractPackageBackEnd.h \
AbstractPackageRecipient.h \
AbstractTaskSolver.h \
ConfigCenter.h \
ConfigData.h \
ConfigFile.h \
DeepsolverException.h \
deepsolver.h \
DeepsolverTypes.h \
DefaultValues.h \
IndexCore.h \
IndexFetch.h \
InfoCore.h \
io/PackageScopeContentBuilder.h \
io/PackageScopeContentLoader.h \
logging.h \
OperationCore.h \
os/Directory.h \
os/File.h \
os/SystemException.h \
os/system.h \
PackageInfoProcessor.h \
Pkg.h \
PkgUtils.h \
ProvidePriorityList.h \
repo/InfoFileException.h \
repo/InfoFileReader.h \
repo/PkgSection.h \
repo/RepoParams.h \
Repository.h \
repo/TextFormatSectionReader.h \
RequireFilter.h \
rpm/Rpmdb.h \
rpm/RpmException.h \
rpm/RpmFile.h \
rpm/RpmFileHeaderReader.h \
rpm/RpmHeaderReading.h \
rpm/vercmp.h \
transact/InstalledReferences.h \
transact/PackageScopeContent.h \
transact/PackageScope.h \
transact/ProvideMap.h \
transact/sat.h \
transact/TaskException.h \
UserTask.h \
utils/CurlInterface.h \
utils/DateTime.h \
utils/GzipInterface.h \
utils/Md5.h \
utils/Md5File.h \
utils/string-utils.h \
utils/TinyFileDownload.h \
version.h
