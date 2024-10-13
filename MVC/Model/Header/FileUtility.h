#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
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

class FileSystemProvider {
    protected:
        std::vector<std::string> directoryFileList;
        std::vector<std::string> vectorHashCur;
        std::vector<std::string> vectorHashNew;

        virtual void setFileList(const std::string& fileName) {
            directoryFileList.push_back(fileName);
        }
        
    public:
        virtual ~FileSystemProvider() = default; // Does abstract class.
        
        virtual const std::vector<std::string>& getFileList() {
            return directoryFileList;
        }
        
        virtual std::vector<std::string>& hybridVectorHashCur() {
            return vectorHashCur;
        }
        
        virtual std::vector<std::string>& hybridVectorHashNew() {
            return vectorHashNew;
        }

        virtual void setVectorData(const std::string& fileHash, std::vector<std::string>& vectorData) {
            vectorData.push_back(fileHash);
        }
        virtual const std::vector<std::string>& getVectorData(const std::vector<std::string>& vectorData) {
            return vectorData;
        }
};

class DirectoryProvider : public FileSystemProvider {
    protected:
        std::string path;
        std::string getPath() const { return path; }

        DirectoryProvider(const std::string& memberPath) : path(memberPath) {} // Before `:` The body of constructor, after `:` objects for initialization.

    public:
        virtual bool isFolderExist() const {
            return std::filesystem::exists(getPath());
        }

        virtual void setContext() = 0;
        
        virtual ~DirectoryProvider() {}
};

class DirectoryLocal : public DirectoryProvider {
    public:
        DirectoryLocal(const std::string& path) : DirectoryProvider(path) {}
    
        void setContext() override final {
            
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)){
                const auto& path = entry.path().u8string();

                if (!std::filesystem::is_directory(path)){
                    setFileList(path);
                    std::cout << "Folder path: " << path << std::endl;
                }
                else {
                    std::cout << "This is folder:" << path << " ?" << std::endl;
                }
            }

        }

        virtual ~DirectoryLocal() {}
};

class FileHashProvider : FileSystemProvider {
    private:
        SHA256Algorithm sha256;

    public:
        bool fileCalculateHash(std::vector<std::string> vectorData) {
            bool isProcessFinished = true;

            for (const auto& filePath : getFileList()){
                    
                std::string calcHash = sha256.calcHash(filePath);
                    
                if(!calcHash.empty()){
                    std::cout << "SHA256 hash for file: " << filePath << " : " << calcHash << std::endl;
                    setVectorData(calcHash, vectorData);
                }
                else{
                    std::cerr << "Error calculate hash for file: " << filePath << std::endl;
                    isProcessFinished = false;
                    break;
                }
            }
            return isProcessFinished;
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
            return true;
        }

        void fileMoving(){
            // Sync data: checking the free disk space, calculating the required disk space, making a file map for moving files
        }
    /*


            for (const auto& filePath : getFileList()){
                    
                std::string calcHash = sha256.calcHash(filePath);
                    
                if(!calcHash.empty()){
                    std::cout << "SHA256 hash for file: " << filePath << " : " << calcHash << std::endl;
                    addHashNew(calcHash);
                }   
                else{
                    std::cerr << "Error calculate hash for file: " << filePath << std::endl;
                }
            }

            equalVectors(getHashOld(), getHashNew());*/
            virtual ~FileHashProvider() {}
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