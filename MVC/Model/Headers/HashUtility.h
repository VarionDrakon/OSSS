#ifndef HASHUTILITY_H
#define HASHUTILITY_H
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms

#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>

#elif __linux__ //Linux platforms

#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>

#endif

class HashProviderObject {

    protected:

    public:
        virtual std::string hashCalculateFile(const std::filesystem::path& filePath) = 0;
        virtual std::string hashCalculateBlock(const char *data) = 0;

        virtual ~HashProviderObject() {};
};

class HashAlgorithm : public HashProviderObject {
    public: 
        virtual ~HashAlgorithm() {}
};

class SHA256Algorithm : public HashAlgorithm {
    private:
        std::string hashResult;
        CryptoPP::SHA256 sha256;

    public:

        std::string hashCalculateFile(const std::filesystem::path& filePath) override;
        std::string hashCalculateBlock(const char *data) override;

        virtual ~SHA256Algorithm() {}
};
#endif // HASHUTILITY_H