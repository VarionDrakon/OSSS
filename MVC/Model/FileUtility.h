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
#include <windows.h>

class FileSystemObject {
    protected:
        std::string path;
        std::vector<std::string> directoryFileList;
        std::vector<std::string> vectorHashOld;
        std::vector<std::string> vectorHashNew;
        std::string getPath() const { return path; }
        
    public:
        virtual ~FileSystemObject() = default; // Does abstract class.
        FileSystemObject(const std::string& memberPath) : path(memberPath) {} // Before `:` The body of constructor, after `:` objects for initialization.
        
        // Dynamic dispatch.
        virtual bool exist() const = 0;
        virtual std::vector<FileSystemObject*> getContents() = 0;

        virtual void addFile(const std::string& fileName) = 0;
        virtual const std::vector<std::string>& getFileList() const = 0;
        virtual void addHashOld(const std::string& fileName) = 0;
        virtual const std::vector<std::string>& getHashOld() const = 0;
        virtual void addHashNew(const std::string& fileName) = 0;
        virtual const std::vector<std::string>& getHashNew() const = 0;
};

class Directory : public FileSystemObject {
    public:
        Directory(const std::string& path) : FileSystemObject(path) {}
        virtual ~Directory() {}
};

class LocalDirectory : public Directory {
    public:
        LocalDirectory(const std::string& path) : Directory(path) {}

        bool exist() const override final {
            return std::filesystem::exists(getPath());
        }
        ///
        const std::vector<std::string>& getFileList() const override final {
            return directoryFileList;
        }
        void addFile(const std::string& fileName) override final {
            directoryFileList.push_back(fileName);
        }
        ///
        const std::vector<std::string>& getHashOld() const override final {
            return vectorHashOld;
        }
        void addHashOld(const std::string& fileName) override final {
            vectorHashOld.push_back(fileName);
        }
        ///
        const std::vector<std::string>& getHashNew() const override final {
            return vectorHashNew;
        }
        void addHashNew(const std::string& fileName) override final {
            vectorHashNew.push_back(fileName);
        }
        ///
        bool equalVectors(const std::vector<std::string> vectorFirst, const std::vector<std::string> vectorSecond){
            if(!vectorFirst.empty() && !vectorSecond.empty()){

                std::cout << "Four X2! " << std::endl;

                if(vectorFirst.size() != vectorSecond.size()) {
                    std::cout << "Size not equals: " << vectorFirst.size() << " & " << vectorSecond.size() << std::endl;
                    return false; // Size vectors must be equal.
                }
                
                std::cout << "Five! " << std::endl;

                for(size_t i = 0; i < vectorFirst.size(); ++i){
                    std::cout << "Current equals hash: " << vectorFirst[i] << " & " << vectorSecond[i] << std::endl;
                    if(vectorFirst[i] != vectorSecond[i]) {
                        std::cout << "Vector NOT equal`s!" << std::endl;
                        std::cout << "Five X3! " << std::endl;
                        return false; // Elements with the same index are not equal.
                    }
                }
                std::cout << "Six! " << std::endl;
            }
            std::cout << "Vector equal`s!" << std::endl;
            return true;
        }

        std::vector<FileSystemObject*> getContents() override final {
            std::vector<FileSystemObject*> contents;
            //std::vector<std::string> vectorPathFiles;

            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)){
                const auto& path = entry.path();

                //std::vector<std::string> pathFiles;

                if (!std::filesystem::is_directory(path)){
                    //vectorPathFiles.push_back(path.string());
                    addFile(path.string());
                }
                else {
                    //std::cout << "Folder path: " << path << std::endl;
                }
            }
            SHA256Algorithm sha256;
                
            for (const auto& filePath : getFileList()){
                    
                std::string calcHash = sha256.calcHash(filePath);
                    
                if(!calcHash.empty()){
                    std::cout << "SHA256 hash for file: " << filePath << " : " << calcHash << std::endl;
                    addHashOld(calcHash);
                }
                else{
                    std::cerr << "Error calculate hash for file: " << filePath << std::endl;
                }
            }
            
            std::cout << "One! " << std::endl;
            Sleep(5000);
            std::cout << "Two! " << std::endl;

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

            std::cout << "Three! " << std::endl;
            Sleep(5000);
            std::cout << "Four! " << std::endl;

            equalVectors(getHashOld(), getHashNew());

            return contents;
        }

        virtual ~LocalDirectory() {}
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