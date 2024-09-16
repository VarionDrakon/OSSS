#include "Header/FileUtility.h"
#include <ostream>

class FileHashAlgorithmProvider {

    public:
        virtual void triggerAlgorithm() {
            FileHashProvider fileHash;
            DirectoryLocal dirLoc("E:/testing");

            if(dirLoc.isFolderExist()){
                dirLoc.setContext();

                if(fileHash.fileCalculateHash(dirLoc.hybridVectorHashCur()) & fileHash.fileCalculateHash(dirLoc.hybridVectorHashNew())){

                    if(fileHash.equalVectors(dirLoc.hybridVectorHashCur(), dirLoc.hybridVectorHashNew())){
                        std::cout << "Comparison: YES" << std::endl;
                        return;
                    }
                    else {
                        std::cout << "Comparison: NO" << std::endl;
                    }

                }
                
                std::cout << "Complete!" << std::endl;
            }

        };
};