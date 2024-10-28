#include "Header/FileUtility.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class FileHashAlgorithmProvider {
    private:

    protected:

    public:

        virtual void triggerAlgorithm() {
            FileHashProvider fhp;
            DirectoryLocal dl("/home/sysadm/Downloads/");
            
            if(dl.isFolderExist()){
                dl.setContext();

                if((fhp.fileCalculateHash(fhp.getVectorHashCur(), dl.getFileList()) == true) && (fhp.fileCalculateHash(fhp.getVectorHashNew(), dl.getFileList()) == true)){

                    if(fhp.equalVectors(fhp.getVectorHashCur(), fhp.getVectorHashNew())){
                        std::cout << "Comparison: YES" << std::endl;
                    }
                    else {
                        std::cout << "Comparison: NO" << std::endl;
                    }
                }
                else{
                    std::cout << "Comparison ERROR!" << std::endl;
                    return;
                }

                std::cout << "Complete!" << std::endl;
            }
            else {
                std::cout << "Folder not found!" << std::endl;
            }
        };
};