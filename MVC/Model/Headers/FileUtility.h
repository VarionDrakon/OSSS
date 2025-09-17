#ifndef FILEUTILITY_H
#define FILEUTILITY_H
#include <filesystem>
#include <string>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <list>
#include <unordered_map>
#include "HashUtility.h"

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms

#include <Windows.h> // I didn't want to use third-party libraries, so I'll use the real evil - Windows API...
#include <sstream>
#include <cstddef>
#include <timezoneapi.h>
#include <AclAPI.h>

#define slp(x) Sleep((x) * 1000)
#define szt size_t
#define unitSize 1000

#elif __linux__ //Linux platforms

#define slp(x) usleep((x) * 1000000)
#define szt ssize_t
#define unitSize 1024

#endif

enum class filePropertiesTimeTypeEnum {
    TimeCreation, // Birth
    TimeModified, // Modify
    TimeAccess    // Access
};

struct FileMetadata {
    std::string filePath;
    std::string fileName;
    std::string fileSize;
    std::string fileTypeData; // * https://www.iana.org/assignments/media-types/media-types.xhtml, write format - "text/plain" 
    std::string fileOwner;
    std::string fileDateTime; // * https://www.w3.org/TR/NOTE-datetime, write format - YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+03:00) 16.07.1997 time 19:20:30.45 according to Moscow time
    std::string fileHash;
};

class FileUtility {
    private:

    protected:

    public:
        FileUtility() {};

        virtual ~FileUtility();
};

class FileUtilityProvider : public FileUtility {
    private:

    protected:
        
    public:
        FileUtilityProvider() {};

        virtual void fileMetadataCollectRecursively(std::string directoryRoot) = 0;

        virtual ~FileUtilityProvider();
};

class FileUtilityProviderLocal : public FileUtilityProvider {
    private:
        FileMetadata currentFileMetadata;

    public:
        FileUtilityProviderLocal() {};

        virtual void fileMetadataCollectRecursively(std::string directoryRoot) override final;
        virtual std::string getFilePropertiesTime(std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum);
        virtual std::string getFilePropertiesSize(std::filesystem::path fileSystemObjectPath);
        virtual std::string getFilePropertiesOwner(std::filesystem::path fileSystemObjectPath);

        virtual FileMetadata getFileMetadata();
        virtual void clearFileMetadata();

        virtual ~FileUtilityProviderLocal();
};


class FileUtilityAlgorithmProvider : public FileUtility {
    protected:

    public:
        FileUtilityAlgorithmProvider() {};

        virtual ~FileUtilityAlgorithmProvider();
};

class FileUtilityHashProvider : public FileUtilityAlgorithmProvider {
    public:
        FileUtilityHashProvider() {};

        std::string fileCalculateHash(const std::string& filePath);
        bool equalVectors(const std::vector<std::string> vectorFirst, const std::vector<std::string> vectorSecond);
        // virtual void fileMoving();
        
        virtual ~FileUtilityHashProvider();
};

/*
class CloudDirectory : public Directory{
    public:
        virtual ~CloudDirectory() {}
};

class NetworkDirectory : public Directory{
    public:
        virtual ~NetworkDirectory() {}
};

class DualOSDirectory : public Directory{
    public:
        virtual ~DualOSDirectory() {}
};

class MultiOSDirectory : public Directory{
    public:
        virtual ~MultiOSDirectory() {}
};*/
// LRU

class FileCache {
    private:
        std::unordered_map<std::string, FileMetadata> cache;
    
    public:

        void cacheUpdate(const FileMetadata& metadata) {
            cache[metadata.filePath] = metadata;
        }

        bool cacheContains(const std::string& path) const {
            return cache.find(path) != cache.end();
        }

        FileMetadata cacheGet(const std::string& path) const {
            return cache.at(path);
        }

        void cacheSaveToFile();

        void cacheLoadFromFile();
};

#endif // FILEUTILITY_H