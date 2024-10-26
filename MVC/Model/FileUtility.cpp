#include "Header/FileUtility.h"
#include <memory>
#include <string>
#include <vector>

class FileHashAlgorithmProvider {
    private:

    protected:

    public:

        virtual void triggerAlgorithm() {
            FileHashProvider fhp;
            DirectoryLocal dl("E:/testing");
            
            if(dl.isFolderExist()){
                dl.setContext();
                dl.getFileList();

                if(fhp.fileCalculateHash(fhp.getVectorHashCur(), dl.getFileList()) & fhp.fileCalculateHash(fhp.getVectorHashNew(), dl.getFileList())){

                    if(fhp.equalVectors(fhp.getVectorHashCur(), fhp.getVectorHashNew())){
                        std::cout << "Comparison: YES" << std::endl;

                        // return;
                    }
                    else {
                        std::cout << "Comparison: NO" << std::endl;
                    }
                }

                std::cout << "Complete!" << std::endl;
            }
        };
};