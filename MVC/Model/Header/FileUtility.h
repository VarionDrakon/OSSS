#include <cstddef>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
// FOR TEST FUNCTIONALITY!
#include "HashUtility.h"

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms

#include <windows.h>
#define slp(x) Sleep((x) * 1000)
#define szt size_t

#elif __linux__ //Linux platforms

#include <unistd.h>
#define slp(x) usleep((x) * 1000000)
#define szt ssize_t

#endif

class FileUtility {
    private:
        std::string path;

    protected:

    public:
        FileUtility(const std::string& path) : path(path) {}
        FileUtility();

        virtual ~FileUtility() = default;
};

class FileUtilityProvider : public FileUtility {
    private:
        std::string path;

    protected:
        
    public:
        FileUtilityProvider(const std::string& path) : FileUtility(path), path(path){}
        FileUtilityProvider();

        virtual void setContext() = 0;
        virtual const std::vector<std::string>& getFileList() = 0;
        std::string getPath() const { return path; }

        bool isFolderExist() const;

        virtual ~FileUtilityProvider() = default;
};

class FileUtilityProviderLocal : public FileUtilityProvider {
    private:
        std::vector<std::string> directoryFileList;
        std::string path;

    public:
        FileUtilityProviderLocal(const std::string& path) : FileUtilityProvider(path), path(path) {}
        FileUtilityProviderLocal();

        virtual void setContext() override final;
        virtual const std::vector<std::string>& getFileList() override final;

        virtual ~FileUtilityProviderLocal() = default;
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
        void triggerAlgorithm();

        virtual ~FileUtilityAlgorithmProvider() = default;
};

class FileUtilityHashProvider : public FileUtilityAlgorithmProvider {

    public:
        FileUtilityHashProvider();

        bool fileCalculateHash(std::vector<std::string>& vectorData, const std::vector<std::string>& vectorFileList);
        bool equalVectors(const std::vector<std::string> vectorFirst, const std::vector<std::string> vectorSecond);
        // virtual void fileMoving();
        
        virtual ~FileUtilityHashProvider() = default;
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