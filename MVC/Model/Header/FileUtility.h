#include <filesystem>
#include <string>
#include <vector>
// FOR TEST FUNCTIONALITY!
#include "HashUtility.h"
//using namespace std;

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms

#include <Windows.h> // I didn't want to use third-party libraries, so I'll use the real evil - Windows API...
#include <sstream>
#include <cstddef>
#include <filesystem>
#include <timezoneapi.h>
#include <AclAPI.h>

#define slp(x) Sleep((x) * 1000)
#define szt size_t
#define unitSize 1000

#elif __linux__ //Linux platforms

#include <unistd.h>
#define slp(x) usleep((x) * 1000000)
#define szt ssize_t
#define unitSize 1024

#endif

enum class filePropertiesTimeTypeEnum {
    TimeCreation,
    TimeModified,
    TimeAccess
};

struct FilePropertiesInfo { 
    const std::string *fileName, *fileSize, *typeData, *owner, *dateTime, *hash; /*
                *fileSize: It's necessary :)
                *typeData: https://www.iana.org/assignments/media-types/media-types.xhtml, write format - "text/plain" 
                *dateTime: https://www.w3.org/TR/NOTE-datetime, write format - YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+03:00) 16.07.1997 time 19:20:30.45 according to Moscow time
                */
};

std::vector<FilePropertiesInfo> vectorFilePropertiesInfo;

class FileUtility {
    private:
        std::string path;

    protected:

    public:
        FileUtility(const std::string& path) : path(path) {}
        FileUtility();

        virtual ~FileUtility()  {
            std::cout << "FileUtility destroyed." << std::endl;
        };
};

class FileUtilityProvider : public FileUtility {
    private:
        std::string path;

    protected:
        
    public:
        FileUtilityProvider(const std::string& path) : FileUtility(path), path(path){}
        FileUtilityProvider();

        virtual void setContext(std::vector<std::string>& vectorPropertiesFileName, std::vector<std::string>& vectorPropertiesFileSize, std::vector<std::string>& vectorPropertiesFileType, std::vector<std::string>& vectorPropertiesOwner, std::vector<std::string>& vectorPropertiesDateTime, std::vector<std::string>& vectorPropertiesHash) = 0;
        virtual std::vector<std::string>& getFileList() = 0;
        std::string getPath() const { return path; }

        bool isFolderExist() const;

        virtual ~FileUtilityProvider() {
            std::cout << "FileUtilityProvider destroyed." << std::endl;
        };
};

class FileUtilityProviderLocal : public FileUtilityProvider {
    private:
        std::vector<std::string> directoryFileList;
        std::string path;
        std::vector<FilePropertiesInfo> filePropertiesInfo;

    public:
        FileUtilityProviderLocal(const std::string& path) : FileUtilityProvider(path), path(path) {}
        FileUtilityProviderLocal();

        virtual void setContext(std::vector<std::string>& vectorPropertiesFileName, std::vector<std::string>& vectorPropertiesFileSize, std::vector<std::string>& vectorPropertiesFileType, std::vector<std::string>& vectorPropertiesOwner, std::vector<std::string>& vectorPropertiesDateTime, std::vector<std::string>& vectorPropertiesHash) override final;
        virtual std::vector<std::string>& getFileList() override final;
        virtual std::string getFilePropertiesTime(std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum);
        virtual std::string getFilePropertiesSize(std::filesystem::path fileSystemObjectPath);
        virtual std::string getFilePropertiesOwner(std::filesystem::path fileSystemObjectPath);

        virtual ~FileUtilityProviderLocal() {
            std::cout << "FileUtilityProviderLocal destroyed." << std::endl;
        };
};


class FileUtilityAlgorithmProvider : public FileUtility {
    protected:
        SHA256Algorithm sha256;

    public:
        FileUtilityAlgorithmProvider();
        
        void triggerAlgorithm(std::string contextPath, std::vector<std::string>& vectorPropertiesFileName, std::vector<std::string>& vectorPropertiesFileSize, std::vector<std::string>& vectorPropertiesFileType, std::vector<std::string>& vectorPropertiesOwner, std::vector<std::string>& vectorPropertiesDateTime, std::vector<std::string>& vectorPropertiesHash);

        virtual ~FileUtilityAlgorithmProvider() {
            std::cout << "FileUtilityAlgorithmProvider destroyed." << std::endl;
        };
};

class FileUtilityHashProvider : public FileUtilityAlgorithmProvider {
    public:
        FileUtilityHashProvider();

        std::string fileCalculateHash(const std::string& filePath);
        bool equalVectors(const std::vector<std::string> vectorFirst, const std::vector<std::string> vectorSecond);
        // virtual void fileMoving();
        
        virtual ~FileUtilityHashProvider() {
            std::cout << "FileUtilityHashProvider destroyed." << std::endl;
        };
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