#include <filesystem>
#include "../lib/cryptopp890/files.h"
#include "../lib/cryptopp890/filters.h"
#include "../lib/cryptopp890/hex.h"
#include "../lib/cryptopp890/sha.h"
#include "Model/FileUtility.h"

namespace fs = std::filesystem;
using namespace CryptoPP;

HexEncoder encoder(new FileSink(std::cout));

CryptoPP::SHA256 hash;

std::string msg = "Yoda said, Do or do not. There is no try.";
std::string digest;

void readerDirectory(const fs::path& dirPath){
    for (const auto& entry : fs::directory_iterator(dirPath)){
        const auto& path = entry.path();

        if(fs::is_regular_file(path)){
            std::ifstream file(path, std::ios::binary);

            if(file){
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                std::cout << content << std::endl;
                file.close();
            }
            else{
                std::cerr << "Failed to open file/folder!" << std::endl;
            }
        }
        else if(fs::is_directory(path)){
            readerDirectory(path);
        }
    }


    //fs::path workerPath = "C:/Users/vario/AppData/Local/Opera Software";
    //readerDirectory(workerPath);
    hash.Update((const byte*)msg.data(), msg.size());
    digest.resize(hash.DigestSize());
    hash.Final((byte*)&digest[0]);
    std::cout << "SHA256: ";
    StringSource give_me_a_name(digest, true, new Redirector(encoder));
    std::cout << std::endl;
    //system("pause");
}

int main(){
    //fs::path dirPath = "C:/Users/vario/AppData/Local/Opera Software";

    LocalDirectory directoryPath("C:/Users/vario/AppData/Local/Opera Software");

    if(directoryPath.exist()){
        directoryPath.getContents();
    }

    return 0;
}