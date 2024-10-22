#include "Header/FileUtility.h"

class FileHashAlgorithmProvider {
    protected:
        FileSystemProvider fsp;

    public:
        virtual void triggerAlgorithm() {
            FileHashProvider fileHash;
            DirectoryLocal dirLoc("E:/testing");
            auto files = fsp.getFileList();
            
            if(dirLoc.isFolderExist()){
                dirLoc.setContext();

                // if(fileHash.fileCalculateHash(dirLoc.getVectorHashCur()) & fileHash.fileCalculateHash(dirLoc.getVectorHashNew())){

                //     if(fileHash.equalVectors(dirLoc.getVectorHashCur(), dirLoc.getVectorHashNew())){
                //         std::cout << "Comparison: YES" << std::endl;

                //         // return;
                //     }
                //     else {
                //         std::cout << "Comparison: NO" << std::endl;
                //     }
                // }
                std::cout << "Complete!" << std::endl;
                // data = 
                for (const auto& d : files){
                    std::cout << " DATA: " << d << std::endl;
                }
            }
        };
};