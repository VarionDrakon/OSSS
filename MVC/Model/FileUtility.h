#include <iostream>
#include <vector>

class FileSystemObject {
    protected:
        std::string path;

    public:
        virtual ~FileSystemObject() {}
        FileSystemObject(const std::string& path) : path(path) {}
        virtual bool exist() const = 0;
        //std::string getPath() const { return path; }
};

class Directory : FileSystemObject {
    public:
        Directory(const std::string& path) : FileSystemObject(path) {}
        
        bool exist() const override = 0;

        virtual std::vector<FileSystemObject*> getContents() = 0;
};

class LocalDirectory : Directory {
    public:
        LocalDirectory(const std::string& path) : Directory(path) {}
        
        bool exist() const override {
            
        }
};