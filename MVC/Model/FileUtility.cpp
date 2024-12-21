#include "Header/FileUtility.h"
#include <vector>

FileUtility::FileUtility() {}

FileUtilityProvider::FileUtilityProvider() {}

FileUtilityProviderLocal::FileUtilityProviderLocal() {}

FileUtilityAlgorithmProvider::FileUtilityAlgorithmProvider() {}

FileUtilityHashProvider::FileUtilityHashProvider() {}

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

bool FileUtilityProvider::isFolderExist() const {
    return std::filesystem::exists(getPath());
}

const std::vector<std::string>& FileUtilityProviderLocal::getFileList()  {
    return FileUtilityProviderLocal::directoryFileList;
}

void FileUtilityAlgorithmProvider::setVectorData(const std::string& fileHash, std::vector<std::string>& vectorData) {
    vectorData.push_back(fileHash);
}

std::vector<std::string>& FileUtilityAlgorithmProvider::getVectorHashCur() {
    return vectorHashCur;
}
std::vector<std::string>& FileUtilityAlgorithmProvider::getVectorHashNew() {
    return vectorHashNew;
}

void FileUtilityAlgorithmProvider::triggerAlgorithm() {
    FileUtilityHashProvider fhp;
    FileUtilityProviderLocal dl("E:/TempFilesProgramm/OSSS/TempFolder/rename_factions");

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
}

bool FileUtilityHashProvider::fileCalculateHash(std::vector<std::string>& vectorData, const std::vector<std::string>& vectorFileList) {

    for (const auto& filePath : vectorFileList){
        std::string calcHash = sha256.calcHash(filePath);
                            
        if(!calcHash.empty()){
            std::cout << "SHA256 hash for file: " << filePath << " : " << calcHash << std::endl;
            setVectorData(calcHash, vectorData);
        }
        else{
            std::cerr << "Error calculate hash for file: " << filePath << std::endl;
            return false;
        }
    }

    return true;

}

bool FileUtilityHashProvider::equalVectors(const std::vector<std::string> vectorFirst, const std::vector<std::string> vectorSecond){

    if(!vectorFirst.empty() && !vectorSecond.empty()){

        if(vectorFirst.size() != vectorSecond.size()) {
            std::cout << "Size not equals: " << vectorFirst.size() << " & " << vectorSecond.size() << std::endl;
            return false; // Size vectors must be equal.
        }

        for(szt i = 0; i < vectorFirst.size(); ++i){
            std::cout << "Current equals hash: " << vectorFirst[i] << " & " << vectorSecond[i] << std::endl;
            if(vectorFirst[i] != vectorSecond[i]) {
                std::cout << "Vector NOT equal`s!" << std::endl;
                return false; // Elements with the same index are not equal.
            }
        }
    }
    else{
        std::cout << "vector(!?).empty()" << std::endl;
    }

    return true;
}