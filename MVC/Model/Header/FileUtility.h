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
        
        virtual const std::vector<std::string>& getFileList() const = 0;

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
        const std::vector<std::string>& getFileList() const override = 0;
        std::string getPath() const { return path; }

        virtual bool isFolderExist() const {
            return std::filesystem::exists(getPath());
        }

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
        virtual const std::vector<std::string>& getFileList() const override final {
            return directoryFileList;
        }

        virtual ~FileUtilityProviderLocal() = default;
};




class FileHashProvider {
    private:
        SHA256Algorithm sha256;
        std::vector<std::string> vectorHashCur;
        std::vector<std::string> vectorHashNew;

        virtual void setVectorData(const std::string& fileHash, std::vector<std::string>& vectorData) {
            vectorData.push_back(fileHash);
        }

    public:
        virtual std::vector<std::string>& getVectorHashCur() {
            return vectorHashCur;
        }
        virtual std::vector<std::string>& getVectorHashNew() {
            return vectorHashNew;
        }

        bool fileCalculateHash(std::vector<std::string>& vectorData, const std::vector<std::string>& vectorFileList) {
            for (const auto& filePath : vectorFileList){
                std::string calcHash = sha256.calcHash(filePath);
                                   
                if(!calcHash.empty()){
                    std::cout << "SHA256 hash for file: " << filePath << " : " << calcHash << std::endl;
                    setVectorData(calcHash, vectorData);
                }
                else{
                    std::cerr << "Error calculate hash for file: " << filePath << std::endl;
                    return false;
                }
            }
            return true;
        }

        bool equalVectors(const std::vector<std::string> vectorFirst, const std::vector<std::string> vectorSecond){

            if(!vectorFirst.empty() && !vectorSecond.empty()){

                if(vectorFirst.size() != vectorSecond.size()) {
                    std::cout << "Size not equals: " << vectorFirst.size() << " & " << vectorSecond.size() << std::endl;
                    return false; // Size vectors must be equal.
                }

                for(szt i = 0; i < vectorFirst.size(); ++i){
                    std::cout << "Current equals hash: " << vectorFirst[i] << " & " << vectorSecond[i] << std::endl;
                    if(vectorFirst[i] != vectorSecond[i]) {
                        std::cout << "Vector NOT equal`s!" << std::endl;
                        return false; // Elements with the same index are not equal.
                    }
                }
            }
            else{
                std::cout << "vector(!?).empty()" << std::endl;
            }
            return true;
        }

        void fileMoving(){
            // Sync data: checking the free disk space, calculating the required disk space, making a file map for moving files
        }
            virtual ~FileHashProvider() = default;
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