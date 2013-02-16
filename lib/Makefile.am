
AM_CXXFLAGS = $(DEEPSOLVER_CXXFLAGS) $(DEEPSOLVER_INCLUDES)
LIBS += -lcurl -lminisat -lsqlite3
LIBdir=${libdir}
LDFLAGS=-version-info ${LT_CURRENT}:$(LT_REVISION):$(LT_AGE) -release $(LT_RELEASE)

LIB_LTLIBRARIES = libdeepsolver.la

libdeepsolver_ladir = $(includedir)

libdeepsolver_la_SOURCES = \
ConfigCenter.cpp \
ConfigFile.cpp \
DummyPackageBackEnd.cpp \
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
repo/InfoFileReader.cpp \
repo/PkgSection.cpp \
repo/RepoParams.cpp \
Repository.cpp \
repo/TextFormatSectionReader.cpp \
SqlitePkgCacheCreator.cpp \
transact/GeneralSolver.cpp \
transact/InstalledReferences.cpp \
transact/LibMinisatSolver.cpp \
transact/PackageScopeContent.cpp \
transact/PackageScope.cpp \
transact/ProvideMap.cpp \
transact/ProvidePriorityList.cpp \
transact/RelaxedSolver.cpp \
utils/CurlInterface.cpp \
utils/GzipInterface.cpp \
utils/Md5.cpp \
utils/Md5File.cpp \
utils/RegExp.cpp \
utils/SqliteInterface.cpp \
utils/StringUtils.cpp \
utils/TinyFileDownload.cpp 

libdeepsolver_la_HEADERS = \
AbstractPackageBackEnd.h \
AbstractPackageRecipient.h \
ConfigCenter.h \
ConfigData.h \
ConfigFile.h \
DeepsolverExceptions.h \
deepsolver.h \
DeepsolverTypes.h \
DefaultValues.h \
DummyPackageBackEnd.h \
IndexCore.h \
IndexFetch.h \
InfoCore.h \
io/PackageScopeContentBuilder.h \
io/PackageScopeContentLoader.h \
logging.h \
OperationCore.h \
os/Directory.h \
os/File.h \
os/system.h \
PackageInfoProcessor.h \
Pkg.h \
PkgUtils.h \
repo/InfoFileReader.h \
repo/PkgSection.h \
repo/RepoParams.h \
Repository.h \
repo/TextFormatSectionReader.h \
SqlitePkgCacheCreator.h \
transact/AbstractPackageScope.h \
transact/AbstractSatSolver.h \
transact/AbstractTaskSolver.h \
transact/GeneralSolver.h \
transact/InstalledReferences.h \
transact/PackageNameSorting.h \
transact/PackageScopeContent.h \
transact/PackageScope.h \
transact/ProvideMap.h \
transact/ProvidePriorityList.h \
transact/RelaxedSolver.h \
utils/CurlInterface.h \
utils/DateTime.h \
utils/GzipInterface.h \
utils/Md5File.h \
utils/Md5.h \
utils/RegExp.h \
utils/SqliteInterface.h \
utils/StringUtils.h \
utils/TinyFileDownload.h

