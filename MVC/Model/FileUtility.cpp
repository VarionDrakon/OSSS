#include "Header/FileUtility.h"
#include <ostream>

class FileHashAlgorithmProvider {

    public:
        virtual void triggerAlgorithm() {
            FileHashProvider fileHash;
            DirectoryLocal dirLoc("E:/Program FilesSS/LLVM/lib/clang/17/lib/windows");

            if(dirLoc.isFolderExist()){
                dirLoc.setContext();

                fileHash.fileCalculateHash(dirLoc.hybridVectorHashCur());

                fileHash.fileCalculateHash(dirLoc.hybridVectorHashNew());

                fileHash.equalVectors(dirLoc.hybridVectorHashCur(), dirLoc.hybridVectorHashNew());

                std::cout << "Complete!" << std::endl;
            }

        };
};