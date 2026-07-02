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

        // virtual void fileMetadataCollectRecursively(std::string directoryRoot) = 0;

        virtual ~FileUtilityProvider();
};

class FileUtilityProviderLocal : public FileUtilityProvider {
    private:

    public:
        struct fileMetadata {
            std::string filePath;
            std::string fileName;
            std::string fileSize;
            std::string fileTypeData; // * https://www.iana.org/assignments/media-types/media-types.xhtml, write format - "text/plain" 
            std::string fileOwner;
            std::string fileDateTime; // * https://www.w3.org/TR/NOTE-datetime, write format - YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+03:00) 16.07.1997 time 19:20:30.45 according to Moscow time (RFC 3339)
            std::string fileHash;
        };
        fileMetadata fileMetadataDefault();

        FileUtilityProviderLocal() {};

        void fileMetadataCollect(std::vector<std::string> &fileList, std::ofstream &fileImage);
        std::string filePropertiesPathGet(const std::filesystem::path fileSystemObjectPath);
        std::string filePropertiesNameGet(const std::filesystem::path fileSystemObjectPath);
        std::string filePropertiesTimeGet(const std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum);
        std::string filePropertiesSizeGet(const std::filesystem::path filePath, const filePropertiesSizeEnum sizeUnit);
        std::string filePropertiesOwnerGet(const std::filesystem::path fileSystemObjectPath);
        std::string filePropertiesCalcHash(const std::filesystem::path fileSystemObjectPath);

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

        // std::string fileCalculateHash(const std::string& filePath);
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

// Snapshot-cache
class FileMetadataSnapshot {
    private:

        // void metadataSnapshotWriteFile(std::ofstream &file, const std::string &str);
        
        // std::string metadataSnapshotReadFile(std::ifstream &file);

    public:

    // void metadataSnapshotUpdate(const FileMetadata &metadata);

//         bool metadataSnapshotContains(const std::string &path) const;

//         const FileMetadata* metadataSnapshotGet(const std::string &path) const;

//         const std::unordered_map<std::string, FileMetadata> &metadataSnapshotGetAll();

//         bool metadataSnapshotSaveToFile();

//         bool metadataSnapshotLoadLatestFile();

//         bool metadataSnapshotLoadFromFile(const std::string &fileName);

//         void metadataSnapshotClear();
};

// struct FileMetadataStatus {
//     std::vector<FileMeta> addedFiles;
//     std::vector<FileMeta> modifiedFiles;
//     std::vector<std::string> deletedFiles;
// };

class FileMetadataUtility {
    private:
        // std::unordered_map<std::string, FileMetadata> metadataSnapshotDifferences;

    public:
        // void fileMetadataUtilityCompare(const FileMetadataSnapshot &currentSnapshot, const std::unordered_map<std::string, FileMetadata> &collectedMetadata);

};

class FileImage {
    private:

    #pragma pack(push, 1)
    struct imageHeadMetadata
    {
        char magic[8];                     // Magic number - File signature.
        uint32_t formatVersion;            // Required to determine the selected file format version.
        uint32_t engineVersion;            // Required to determine the version of the engine used to create the file.
        char uuid[36];                     // Universally unique identifier for operation within a server infrastructure.
        // File status flags in the archive:
        uint8_t encrypted;                 // Prepared for the future...
        uint8_t compressed;                // Prepared for the future...
        uint8_t deduplicated;              // Prepared for the future...
        char imageTimeStamp[30];           // Exact time of archive creation.
        char hostnameSource[65];           // Saved for future...
        uint8_t reserved[362];             // Reserved and aligned under 512 bytes.
    };
    #pragma pack(pop)
    imageHeadMetadata imageHeadMetadataDefault();

    #pragma pack(push, 1)
    struct imageConfigurationMetadata
    {
        uint8_t typeImage;                 // Prepared for the future...
        char parentUuid[36];               // Prepared for the future...
        uint8_t encryptAlgorithm;          // Prepared for the future...
        uint8_t encryptKDF;                // Prepared for the future...
        uint32_t encryptKDFConfiguration;  // Prepared for the future...
        char encryptSalt[32];              // Prepared for the future...
        uint8_t compressAlgorithm;         // Prepared for the future...
        uint8_t compressLevel;             // Prepared for the future...
        uint8_t compressHashAlgorithm;     // Prepared for the future...
        uint8_t reserved[434];             // Reserved and aligned under 512 bytes.
    };
    #pragma pack(pop)
    imageConfigurationMetadata imageConfigurationMetadataDefault();

    #pragma pack(push, 1)
    struct imageDataMetadata
    {
        char chunkHash[128];               // Just a hash sum...
        uint32_t chunkSize;                // The chunk size is needed to know in advance how many bytes to read.
        uint8_t chunkEncrypted;            // Prepared for the future...
        uint8_t chinkCompressed;           // Prepared for the future...
        char chunkData[chunkSize];         // A chunk with a pre-defined size.
    };
    #pragma pack(pop)
    imageDataMetadata imageDataMetadataDefault(); 

    #pragma pack(push, 1)
    struct imageFileMetadata
    {
        uint16_t pathLength;               // The size of the path, to know how many bytes to read.
        char pathAbsolute[pathLength];     // Absolute file path.
        char fileType[32];                 // Prepared for the future...
        uint32_t fileOwnerUId;             // Unique user identifier. Required to restore the file with its original parameters.
        uint32_t fileGroupGId;             // Unique groud identifier. Required to restore the file with its original parameters.
        uint32_t filePermisions;           // Prepared for the future...
        uint64_t fileSize;                 // Original file size. Required to verify the recovered file.
        char fileHash[128];                // The hash sum. Required to verify the recovered file.
        char fileCreatedTimeStamp[30];     // A timestamp for the file. This information is obtained from the file system and is not available on all file systems. By default, the value is zero or corresponds to the year 1970.
        char fileModifiedTimeStamp[30];    // A timestamp for the file. This information is obtained from the file system and is not available on all file systems. By default, the value is zero or corresponds to the year 1970.
        char fileAccessedTimeStamp[30];    // A timestamp for the file. This information is obtained from the file system and is not available on all file systems. By default, the value is zero or corresponds to the year 1970.
        uint32_t fileChunkCount;           // The number of chunks the file was split into. Required for deduplication.
        uint64_t *fileChunkOffsets;        // A byte array containing the offset value of a specific chunk from the beginning of the image. Required for deduplication.
    };
    #pragma pack(pop)
    imageFileMetadata imageFileMetadataDefault();

    #pragma pack(push, 1)
    struct imageHashMetadata
    {
        char chunkHash[128];               // Prepared for the future...
        uint64_t chunkOffset;              // Prepared for the future...
        uint32_t fileReferenceCount;       // Prepared for the future...
    };
    #pragma pack(pop)
    imageHashMetadata imageHashMetadataDefault();
    
    #pragma pack(push, 1)
    struct imageOffsetMetadata
    {
        uint64_t imageHeadOffsetOffset;    // Head offset from the start of the image. This must always be 0 bytes. Aligned to 512 bytes by default.
        uint64_t imageConfigurationOffset; // The parameters used to create the image. It always begins at the 512-byte mark of the image. Aligned to 512 bytes by default.
        uint64_t imageDataOffset;          // A block containing data. It is an unaligned block that occupies the majority of the archive. It almost always begins at the 1024 byte.
        uint64_t imageFileOffset;          // A block of metadata for each file stored in the archive. It always begins after the data block, yet is located demon knows where.
        uint64_t imageHashOffset;          // A duplicated chunk hash table. It also starts demon knows where. It is needed to quickly check data or chunks for changes, as well as to create other types of images.
        uint64_t imageOffsetOffset;        // The current table. Essentially, this is unnecessary information, but it is after this block that the basement footer block, which is already needed.
        uint64_t imageFooterOffset;        // Quick and important image metainformation. It is included in and stripped from the image at the end of global operations.
    };
    #pragma pack(pop)
    imageOffsetMetadata imageOffsetMetadataDefault();

    #pragma pack(push, 1)
    struct imageFooterMetadata
    {
        char magicFooter[9];               // Magic word - start of footer. It serves as the footer start marker.
        uint64_t imageFooterOffset;        // The offset (in bytes) of the footer from the beginning of the archive. This is required for quickly locating and reading the meta-information.
        uint64_t fileCount;                // The number of files saved in the image. It is used for quickly reading metadata from the image.
        uint64_t fileDataSize;             // The number of bytes the files will occupy after the image is extracted.
        uint64_t fileDataSizeArchive;      // The number of bytes occupied by the files in the image.
        char imageHash[128];               // The hash sum of the image without the footer. With the footer, the sum is different, and that is normal.
        uint8_t reserved[343];             // Reserved and aligned under 512 bytes.
    };
    #pragma pack(pop)
    imageFooterMetadata imageFooterMetadataDefault();

    public:
        std::vector<std::string> fileCollectRecursively(const std::string& pathSource);
        void encodeBlocksWithHash(std::vector<std::string> &fileList, std::ofstream &fileImage, std::vector<imageIndexMetadata> &viim);
        void stringSerialize(std::ofstream &fileImage, const std::string &str);

        void imageCollect(const std::string& pathSource, const std::string& fileOutput);
        void imageDisperse(const std::string& pathSource, const std::string& fileOutput);
        // void stringDeserialize(std::ofstream &fileImage, const std::string &str);
};

// class FileUtilityJSON {
//     private:

//     public:
//         void JSONSerialization();
//         void JSONDeserialization();

// };

#endif // FILEUTILITY_H