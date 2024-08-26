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
        std::string path;
        std::vector<std::string> directoryFileList;
        std::vector<std::string> vectorHashCur;
        std::vector<std::string> vectorHashNew;
        std::string getPath() const { return path; }
        
    public:
        virtual ~FileSystemProvider() = default; // Does abstract class.
        FileSystemProvider(const std::string& memberPath) : path(memberPath) {} // Before `:` The body of constructor, after `:` objects for initialization.
        
        virtual bool isFolderExist() const {
            return std::filesystem::exists(getPath());
        }
        virtual std::vector<FileSystemProvider*> getContents() = 0;

        virtual void setFileList(const std::string& fileName) {
            directoryFileList.push_back(fileName);
        }
        virtual const std::vector<std::string>& getFileList() const {
            return directoryFileList;
        }

        virtual void addHashCur(const std::string& fileName) {
            vectorHashCur.push_back(fileName);
        }
        virtual const std::vector<std::string>& getHashCur() const {
            return vectorHashCur;
        }
        virtual void addHashNew(const std::string& fileName) {
            vectorHashNew.push_back(fileName);
        }
        virtual const std::vector<std::string>& getHashNew() const {
            return vectorHashNew;
        }
};

class Directory : public FileSystemProvider {
    public:
        Directory(const std::string& path) : FileSystemProvider(path) {}
        virtual ~Directory() {}
};

class DirectoryLocal : public Directory {
    public:
        DirectoryLocal(const std::string& path) : Directory(path) {}

        std::vector<FileSystemProvider*> getContents() override final {
            std::vector<FileSystemProvider*> contents;
            //std::vector<std::string> vectorPathFiles;

            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)){
                const auto& path = entry.path();

                //std::vector<std::string> pathFiles;

                if (!std::filesystem::is_directory(path)){
                    setFileList(path.string());
                }
                else {
                    //std::cout << "Folder path: " << path << std::endl;
                }
            }

            return contents;
        }

        virtual ~DirectoryLocal() {}
};

class FileHasher : FileSystemProvider {
    private:
        SHA256Algorithm sha256;

    public:
        virtual void fileCalcHash(std::vector<std::string*>& fileName) {
            for (const auto& filePath : getFileList()){
                    
                std::string calcHash = sha256.calcHash(filePath);
                    
                if(!calcHash.empty()){
                    std::cout << "SHA256 hash for file: " << filePath << " : " << calcHash << std::endl;
                    
                }
                else{
                    std::cerr << "Error calculate hash for file: " << filePath << std::endl;
                }
            }
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
            std::cout << "Vector equal`s!" << std::endl;
            return true;
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