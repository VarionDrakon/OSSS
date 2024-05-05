#include <filesystem>
#include <iostream>
#include <vector>

class FileSystemObject {
    protected:
        std::string path;
        std::string getPath() const { return path; }
        
    public:
        virtual ~FileSystemObject() {}
        FileSystemObject(const std::string& path) : path(path) {}
        virtual bool exist() const = 0;
};

class Directory : public FileSystemObject {
    public:
        Directory(const std::string& path) : FileSystemObject(path) {}
        
        bool exist() const override = 0;

        virtual std::vector<FileSystemObject*> getContents() = 0;
};

class LocalDirectory : public Directory {
    public:
        LocalDirectory(const std::string& path) : Directory(path) {}
        
        bool exist() const override final {
            return std::filesystem::exists(getPath());
        }
        
        std::vector<FileSystemObject*> getContents() override {
            std::vector<FileSystemObject*> contents;
            
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)){
                const auto& path = entry.path();

                if (std::filesystem::is_directory(path)){
                    std::cout << "Directory: " << path << std::endl;
                }
                else {
                    std::cout << "File: " << path << std::endl;
                }  
            }
            return contents;
        }
};