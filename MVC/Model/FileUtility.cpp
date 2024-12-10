#include "Header/FileUtility.h"

FileUtility::FileUtility() {}

FileUtilityProvider::FileUtilityProvider() {}

FileUtilityProviderLocal::FileUtilityProviderLocal() {}

void FileUtilityProviderLocal::setContext() {

    for (const auto& entry : std::filesystem::recursive_directory_iterator(getPath())){
        const auto& fsObj = entry.path(); 
        const std::string fsStr = fsObj.u8string();

        if (!std::filesystem::is_directory(fsStr)){
            std::cout <<  "Path size: " << std::size(fsStr) << " Folder path: " << fsStr  <<  std::endl;
            directoryFileList.push_back(fsStr);
        } else {
            std::cout << "This is folder:" << fsStr << " ?" << std::endl;
        }   
    }

};

class FileHashAlgorithmProvider {
    private:

    protected:

    public:

        virtual void triggerAlgorithm() {
            FileHashProvider fhp;
            // FileUtilityProviderLocal dl("E:/TempFilesProgramm/OSSS/TempFolder/rename_factions");
            
            // if(dl.isFolderExist()){
            //     dl.setContext();

            //     if((fhp.fileCalculateHash(fhp.getVectorHashCur(), dl.getFileList()) == true) && (fhp.fileCalculateHash(fhp.getVectorHashNew(), dl.getFileList()) == true)){

            //         if(fhp.equalVectors(fhp.getVectorHashCur(), fhp.getVectorHashNew())){
            //             std::cout << "Comparison: YES" << std::endl;
            //         }
            //         else {
            //             std::cout << "Comparison: NO" << std::endl;
            //         }
            //     }
            //     else{
            //         std::cout << "Comparison ERROR!" << std::endl;
            //         return;
            //     }

            //     std::cout << "Complete!" << std::endl;
            // }
            // else {
            //     std::cout << "Folder not found!" << std::endl;
            // }
        };
};