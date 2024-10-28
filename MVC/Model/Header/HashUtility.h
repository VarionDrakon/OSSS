#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms
#include <cryptopp890/hex.h>
#include <cryptopp890/sha.h>
#include <cryptopp890/files.h>

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
    public:

        std::string calcHash(const std::string& filePath) override final {
            std::string hash;
            CryptoPP::SHA256 sha256;

            std::ifstream file(filePath, std::ios::binary); // Open stream.

            if(!file){ // Check if this not file (Example: directory, not exist file) or file busy with another process or thread.
                std::cerr << "This not file or not could be read file." << std::endl;
                return hash;
            } // Try cath is required here.

            CryptoPP::FileSource calculateHashSum(file, true, 
                new CryptoPP::HashFilter(sha256,             // https://cryptopp.com/wiki/HashFilter (Types of algorithms).
                    new CryptoPP::HexEncoder(                    // Split into HEX (binary).
                        new CryptoPP::StringSink(hash)  // Created `composite object`. This `object` receive binary data, encode to hexadecimal format and save it to string.
                        )
                    )
            );

            file.close();
            return hash;
        }

        virtual ~SHA256Algorithm() {}
};