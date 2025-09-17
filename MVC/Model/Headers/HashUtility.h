#ifndef HASHUTILITY_H
#define HASHUTILITY_H
#include <string>
#include <fstream>
#include <iostream> 

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
        virtual ~HashProviderObject() = default; // Virtual destructor.
        virtual std::string calcHash(const std::string& filePath) = 0;
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

        std::string calcHash(const std::string& filePath) override final {           
            try {
                // Check if this not file (Example: directory, not exist file) or file busy with another process or thread.
                if(!std::filesystem::exists(filePath) || std::filesystem::is_directory(filePath)) { 
                    std::cerr << "This not file or not could be read file." << std::endl;
                    return hashResult;
                }

                sha256.Restart();

                CryptoPP::FileSource(
                    filePath.c_str(), 
                    true, 
                    new CryptoPP::HashFilter(
                        sha256,                                   // https://cryptopp.com/wiki/HashFilter (Types of algorithms).
                        new CryptoPP::HexEncoder(                 // Split into HEX (binary).
                            new CryptoPP::StringSink(hashResult)  // Created `composite object`. This `object` receive binary data, encode to hexadecimal format and save it to string.
                        )
                    )
                );

                // std::cout << "SHA256 hash for file: " << filePath << " : " << hashResult << std::endl;
                return hashResult;
            }
            catch (const std::exception& e) {
                std::cerr << "Exception occurred while trying to calculate hash for file: " << filePath << ": " << e.what() << std::endl;
                return "Undefined";
            }    
        }

        virtual ~SHA256Algorithm() {}
};
#endif // HASHUTILITY_H