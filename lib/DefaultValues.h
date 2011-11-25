
#ifndef DEPSOLVER_DEFAULT_VALUES_H
#define DEPSOLVER_DEFAULT_VALUES_H

//Configuration parameters;
#define DEFAULT_CONFIG_DIR "/etc/depsolver"
#define DEFAULT_CONFIG_FILE_NAME "depsolver.conf"

//Repo structure;
#define REPO_RPMS_DIR_NAME "RPMS"
#define REPO_SRPMS_DIR_NAME "RPMS"

//Repo index parameters;
#define REPO_INDEX_DIR "base"
#define REPO_INDEX_INFO_FILE "info"
#define REPO_INDEX_RPMS_DATA_FILE "rpms.data"
#define REPO_INDEX_PROVIDES_DATA_FILE "provides.data"
#define REPO_INDEX_MD5SUM_FILE "md5sum.txt"
#define DEFAULT_REPO_INDEX_COMPRESSION_TYPE CompressionTypeGzip
#define DEFAULT_REPO_INDEX_FORMAT_TYPE FormatTypeText

#endif //DEPSOLVER_DEFAULT_VALUES_H;
