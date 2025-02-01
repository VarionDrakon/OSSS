#include <filesystem>
// FOR TEST FUNCTIONALITY!
#include "HashUtility.h"

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

        virtual void getFileProperties(std::vector<std::string>& VectorFilesProperties, const std::vector<std::string>& vectorFileList) = 0;
        virtual void setContext() = 0;
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

    public:
        FileUtilityProviderLocal(const std::string& path) : FileUtilityProvider(path), path(path) {}
        FileUtilityProviderLocal();

        virtual void setContext() override final;
        virtual std::vector<std::string>& getFileList() override final;
        virtual void getFileProperties(std::vector<std::string>& VectorFilesProperties, const std::vector<std::string>& vectorFileList) override final;
        virtual std::string getFilePropertiesTime(std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum);
        virtual size_t getFilePropertiesSize(std::filesystem::path fileSystemObjectPath);
        virtual std::string getFilePropertiesOwner(std::filesystem::path fileSystemObjectPath);

        virtual ~FileUtilityProviderLocal() {
            std::cout << "FileUtilityProviderLocal destroyed." << std::endl;
        };
};


class FileUtilityAlgorithmProvider : public FileUtility {

    protected:

        SHA256Algorithm sha256;
        std::vector<std::string> vectorHashCur;
        std::vector<std::string> vectorHashNew;
                
        void setVectorData(const std::string& fileHash, std::vector<std::string>& vectorData);

    public:
        FileUtilityAlgorithmProvider();
        
        std::vector<std::string>& getVectorHashCur();
        std::vector<std::string>& getVectorHashNew();
        void triggerAlgorithm(std::string contextPath, std::vector<std::string>& vectorProperties);

        virtual ~FileUtilityAlgorithmProvider() {
            std::cout << "FileUtilityAlgorithmProvider destroyed." << std::endl;
        };
};

class FileUtilityHashProvider : public FileUtilityAlgorithmProvider {

    public:
        FileUtilityHashProvider();

        bool fileCalculateHash(std::vector<std::string>& vectorData, const std::vector<std::string>& vectorFileList);
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