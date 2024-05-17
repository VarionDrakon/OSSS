#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
/*Temp data*/
#include "../../lib/cryptopp890/sha.h"
#include "../../lib/cryptopp890/files.h"
#include "../../lib/cryptopp890/hex.h"
/*Temp data*/

class FileSystemObject {
    protected:
        std::string path;
        std::string getPath() const { return path; }
        std::vector<std::string> vectorPathFiles;
        
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
        
        std::string calcHashSHA256(const std::string& filePath){
            std::string hash;
            CryptoPP::SHA256 sha256;

            std::ifstream file(filePath, std::ios::binary); 

            if(!file){
                std::cerr << "This not file or not could be read file." << std::endl;
                return hash;
            }

            CryptoPP::FileSource(file, true, 
                new CryptoPP::HashFilter(sha256, 
                    new CryptoPP::HexEncoder(
                        new CryptoPP::StringSink(hash)
                    )
                )
            );

            file.close();
            return hash;
        }

        std::vector<FileSystemObject*> getContents() override {
            std::vector<FileSystemObject*> contents;
            
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)){
                const auto& path = entry.path();

                //std::vector<std::string> pathFiles;

                if (!std::filesystem::is_directory(path)){
                    vectorPathFiles.push_back(path.string());
                }
                else {
                    //std::cout << "Folder path: " << path << std::endl;
                }

                for (const auto& filePath : vectorPathFiles){
                    std::string calcHash = calcHashSHA256(filePath);

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



        
};