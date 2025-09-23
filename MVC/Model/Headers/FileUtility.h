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

enum class filePropertiesSizeEnum {
    Bytes,      // 1024^0 = 1 byte
    Kibibytes,  // 1024^1 = 1024 byte
    Mebibyte,   // 1024^2 = 1,048,576 byte
    Gibibyte,   // 1024^3 = 1,073,741,824 byte
    Tebibytes,  // 1024^4 = 1,099,511,627,776 byte
    Pebibyte,   // 1024^5 = 1,125,899,906,842,624 byte
    Exbibyte,   // 1024^6 = 1,152,921,504,606,846,976 byte
    Zebibyte,   // 1024^7 = 1,180,591,620,717,411,303,424 byte
    Yobibyte    // 1024^8 = 1,208,925,819,614,629,174,706,176 byte
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

inline std::ostream& operator<<(std::ostream& os, const FileMetadata& fm) {
    os << "File path: " << fm.filePath 
        << "\nFile Name: " << fm.fileName
        << "\nFile Size: " << fm.fileSize
        << "\nFile TypeData: " << fm.fileTypeData
        << "\nFile Owner: " << fm.fileOwner
        << "\nFile DateTime: " << fm.fileDateTime
        << "\nFile Hash: " << fm.fileHash 
        << std::endl;
    return os;
}

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
        virtual std::string filePropertiesTimeGet(std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum);
        virtual std::string filePropertiesSizeGet(const std::filesystem::path filePath, const filePropertiesSizeEnum sizeUnit);
        virtual std::string filePropertiesOwnerGet(std::filesystem::path fileSystemObjectPath);

        virtual FileMetadata fileMetadataGet();
        virtual void fileMetadataClear();

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
        bool fileMetadataCompare();
        
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

class FileMetadataStorage {
    private:
        std::unordered_map<std::string, FileMetadata> cacheStorage;
    
    public:

        void cacheUpdate(const FileMetadata& metadata) {
            cacheStorage[metadata.filePath] = std::move(metadata);
        }

        bool cacheContains(const std::string& path) const {
            return cacheStorage.find(path) != cacheStorage.end();
        }

        const FileMetadata* cacheGet(const std::string& path) const {
            auto iteration = cacheStorage.find(path);
            if (iteration == cacheStorage.end()) {
                return nullptr;
            }
            return &iteration->second;
        }

        const std::unordered_map<std::string, FileMetadata>& cacheGetAll () {
            return cacheStorage;
        }

        bool cacheSaveToFile(const std::string& fileName) {
            std::ofstream tempFile(fileName, std::ios::binary);

            if (!tempFile) return false;

            size_t countBytes = cacheStorage.size();
            tempFile.write(reinterpret_cast<const char*>(&countBytes), sizeof(countBytes));
            
            for (const auto& pair : cacheStorage) {
                const auto& meta = pair.second;
                writeString(tempFile, meta.filePath);
                writeString(tempFile, meta.fileName);
                writeString(tempFile, meta.fileSize);
                writeString(tempFile, meta.fileTypeData);
                writeString(tempFile, meta.fileOwner);
                writeString(tempFile, meta.fileDateTime);
                writeString(tempFile, meta.fileHash);
            }
            return true;
        }

        bool cacheLoadFromFile(const std::string& fileName) {
            std::ifstream tempFile(fileName, std::ios::binary);
            if (!tempFile) return false;
            
            cacheStorage.clear();
            
            size_t countBytes;
            tempFile.read(reinterpret_cast<char*>(&countBytes), sizeof(countBytes));
            
            for (size_t i = 0; i < countBytes; ++i) {
                FileMetadata meta;
                meta.filePath = readString(tempFile);
                meta.fileName = readString(tempFile);
                meta.fileSize = readString(tempFile);
                meta.fileTypeData = readString(tempFile);
                meta.fileOwner = readString(tempFile);
                meta.fileDateTime = readString(tempFile);
                meta.fileHash = readString(tempFile);
                
                cacheStorage[meta.filePath] = meta;
            }
            return true;
        }

        void cacheClear() {
            cacheStorage.clear();
        }
        
    private:

        void writeString(std::ofstream& file, const std::string& str) {
            size_t size = str.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size));
            file.write(str.c_str(), size);
        }
        
        std::string readString(std::ifstream& file) {
            size_t size;
            file.read(reinterpret_cast<char*>(&size), sizeof(size));
            std::string str(size, ' ');
            file.read(&str[0], size);
            return str;
        }
};

#endif // FILEUTILITY_H