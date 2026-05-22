#include "Headers/HashUtility.h"
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

std::string SHA256Algorithm::hashCalculateFile(const std::filesystem::path& filePath) {           
    try {
        if(!std::filesystem::exists(filePath) || std::filesystem::is_directory(filePath)) { 
            std::cerr << "This not file or not could be read file." << std::endl;
            return hashResult;
        }
        sha256.Restart();

        CryptoPP::FileSource(filePath.c_str(), true, new CryptoPP::HashFilter(sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hashResult)))); // Types of algorithms in docs: https://cryptopp.com/wiki/HashFilter

        hashResult.resize(sha256.DigestSize());

        return hashResult;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception occurred while trying to calculate hash for file: " << filePath << ": " << e.what() << std::endl;
        return "Undefined";
    }    
}

std::string SHA256Algorithm::hashCalculateBlock(const char *data) {           
    try {
        std::string hashResult;
        CryptoPP::SHA256 sha256;
        sha256.Restart();

        CryptoPP::StringSource(data, true, new CryptoPP::HashFilter(sha256, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hashResult))));

        hashResult.resize(sha256.DigestSize());

        return hashResult;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception occurred while trying calculate: " << e.what() << std::endl;
        return "Undefined";
    }    
}