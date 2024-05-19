#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
// FOR TEST FUNCTIONALITY!
#include "HashUtility.h"

class FileSystemObject {
    protected:
        std::string path;
        std::string getPath() const { return path; }
        
    public:
        virtual ~FileSystemObject() = default; // Does not abstract class.
        FileSystemObject(const std::string& memberPath) : path(memberPath) {} // Before `:` The body of constructor, after `:` objects for initialization.
        
        // Dynamic dispatch.
        virtual bool exist() const = 0;
        virtual std::vector<FileSystemObject*> getContents() = 0;
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

        std::vector<FileSystemObject*> getContents() override final {
            std::vector<FileSystemObject*> contents;
            std::vector<std::string> vectorPathFiles;
            
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)){
                const auto& path = entry.path();

                //std::vector<std::string> pathFiles;

                if (!std::filesystem::is_directory(path)){
                    vectorPathFiles.push_back(path.string());
                }
                else {
                    //std::cout << "Folder path: " << path << std::endl;
                }

                SHA256Algorithm sha256;
                
                for (const auto& filePath : vectorPathFiles){
                    
                    std::string calcHash = sha256.calcHash(filePath);
                    
                    if(!calcHash.empty()){
                        std::cout << "SHA256 hash for file: " << filePath << " : " << calcHash << std::endl;
                    }
                    else{
                        std::cerr << "Error calculate hash for file: " << filePath << std::endl;
                    }
                }
            }
            return contents;
        }

        virtual ~LocalDirectory() {}
};