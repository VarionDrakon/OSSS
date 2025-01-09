#include "Header/FileUtility.h"
#include <cstddef>
#include <filesystem>
#include <timezoneapi.h>

FileUtility::FileUtility() {}

FileUtilityProvider::FileUtilityProvider() {}

FileUtilityProviderLocal::FileUtilityProviderLocal() {}

FileUtilityAlgorithmProvider::FileUtilityAlgorithmProvider() {}

FileUtilityHashProvider::FileUtilityHashProvider() {}

void FileUtilityProviderLocal::getFileProperties(std::vector<std::string>& VectorFilesProperties, const std::vector<std::string>& vectorFileList) {

    if(VectorFilesProperties.empty()) {
        VectorFilesProperties.reserve(vectorFileList.size());
        VectorFilesProperties.insert(VectorFilesProperties.end(), vectorFileList.begin(), vectorFileList.end());
    }
    else {
        std::cout << "vectorListPathFiles is not empty! Vector size: " << VectorFilesProperties.size() << std::endl;
    }

}
/** @see FileUtilityProviderLocal::getFilePropertiesTime() EDITED
    

    @param getPath() - get path to root folder.
    @param entry.path() - check directory existence.
    @param fsStr - Returns the internal pathname in native pathname format, converted to specific string type.

    @return directoryFileList - vector storing file path.
*/
std::string FileUtilityProviderLocal::getFilePropertiesTime(std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum) {
    
        FILETIME fileSystemObjectPropertiesFT;
        SYSTEMTIME fileSystemObjectPropertiesST;
        TIME_ZONE_INFORMATION systemObjectTimeZoneTZI;
        size_t timeReturnBuffer = 0;
        
        char timeFormattedUTC[5];
        char timeReturnBufferData[36]; // String buffer length:  1997-07-16T19:20:30.45+03:00 (28) + \0 (2) + 20% (6) | ISO 8601 format
        char *timeReturnConstructor = "%d-%d-%dT%d:%d.%d%c%d:%d";

        HANDLE fileHandle = CreateFileW ( // Microsoft, i hate you! Doc link: https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilew
            fileSystemObjectPath.c_str(),       // Path to the file.
            GENERIC_READ,                  // Request to system for read file.
            FILE_SHARE_READ,                   // Allow joint/shared reading file.
            NULL,                     // Secure attribute (?).
            OPEN_EXISTING,           // Read file if file existing.
            FILE_ATTRIBUTE_NORMAL,    // Normal attribute file (?).
            NULL                             // No template file (?).
        );

        if (fileHandle != INVALID_HANDLE_VALUE) {

            switch (filePropertiesTimeTypeEnum) {
                case filePropertiesTimeTypeEnum::TimeCreation:
                    if (GetFileTime(fileHandle, &fileSystemObjectPropertiesFT, NULL, NULL)) {
                        FileTimeToLocalFileTime(&fileSystemObjectPropertiesFT, &fileSystemObjectPropertiesFT);
                        FileTimeToSystemTime(&fileSystemObjectPropertiesFT, &fileSystemObjectPropertiesST);
                    }
                    else {
                        std::cout << "Error get file time: " << GetLastError() << std::endl;
                    }
                    break;
                case filePropertiesTimeTypeEnum::TimeModified:
                    if (GetFileTime(fileHandle, NULL, &fileSystemObjectPropertiesFT, NULL)) {
                        FileTimeToLocalFileTime(&fileSystemObjectPropertiesFT, &fileSystemObjectPropertiesFT);
                        FileTimeToSystemTime(&fileSystemObjectPropertiesFT, &fileSystemObjectPropertiesST);
                    }
                    else {
                        std::cout << "Error get file time: " << GetLastError() << std::endl;
                    }
                    
                    break;
                case filePropertiesTimeTypeEnum::TimeAccess:
                    if (GetFileTime(fileHandle, NULL, NULL, &fileSystemObjectPropertiesFT)) {
                        FileTimeToLocalFileTime(&fileSystemObjectPropertiesFT, &fileSystemObjectPropertiesFT);
                        FileTimeToSystemTime(&fileSystemObjectPropertiesFT, &fileSystemObjectPropertiesST);
                    }
                    else {
                        std::cout << "Error get file time: " << GetLastError() << std::endl;
                    }
                    break;
            }

            CloseHandle(fileHandle);
            
            GetTimeZoneInformation(&systemObjectTimeZoneTZI);
            char offsetSignUTC = systemObjectTimeZoneTZI.Bias > 0 ? '-' : '+';
            int offsetHoursUTC = -systemObjectTimeZoneTZI.Bias / 60;
            int offsetMinutesUTC = std::abs(systemObjectTimeZoneTZI.Bias + (offsetHoursUTC * 60));

            timeReturnBuffer += snprintf(timeReturnBufferData + timeReturnBuffer, sizeof(timeReturnBufferData) - timeReturnBuffer,timeReturnConstructor,
                fileSystemObjectPropertiesST.wYear, 
                fileSystemObjectPropertiesST.wMonth, 
                fileSystemObjectPropertiesST.wDay, 
                fileSystemObjectPropertiesST.wHour, 
                fileSystemObjectPropertiesST.wMinute, 
                fileSystemObjectPropertiesST.wSecond,
                offsetSignUTC,
                offsetHoursUTC,
                offsetMinutesUTC
            ); // 1997-07-16T19:20:30.45+03:00 ISO 8601 format
        } 
        else {
            std::cout << "Error open file: " << GetLastError() << std::endl;
        }

    return timeReturnBufferData;
}

size_t FileUtilityProviderLocal::getFilePropertiesSize(std::filesystem::path fileSystemObjectPath){

    size_t sizeReturn = std::filesystem::file_size(fileSystemObjectPath) / unitSize;
    
    return sizeReturn;
}

/** @see FileUtilityProviderLocal::setContext()
    Sets the execution context within which the algorithm will be executed, namely the path to the directory that will be the root of the job. 

    @param getPath() - get path to root folder.
    @param entry.path() - check directory existence.
    @param fsStr - Returns the internal pathname in native pathname format, converted to specific string type.

    @return directoryFileList - vector storing file path.
*/
void FileUtilityProviderLocal::setContext() {

    for (const auto& entry : std::filesystem::recursive_directory_iterator(getPath())) {
        const std::filesystem::path& fsObj = entry.path(); 
        std::string fsStr = fsObj.u8string();

        if (!std::filesystem::is_directory(fsStr)) {
            std::replace(fsStr.begin(), fsStr.end(), '\\', '/');
            
            std::cout <<  "Path size: " << getFilePropertiesSize(fsStr) << " Folder path: " << fsStr << " Last time: " << getFilePropertiesTime(fsStr, filePropertiesTimeTypeEnum::TimeAccess) <<  std::endl;
            directoryFileList.push_back(fsStr);
        } else {
            std::cout << "This is folder:" << fsStr << " ?" << std::endl;
        }
    }

}

bool FileUtilityProvider::isFolderExist() const {
    return std::filesystem::exists(getPath());
}

std::vector<std::string>& FileUtilityProviderLocal::getFileList() {
    std::cout << "addr in method: " << &directoryFileList << " size: " << directoryFileList.size() << std::endl;
    return directoryFileList;
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

void FileUtilityAlgorithmProvider::triggerAlgorithm(std::string contextPath, std::vector<std::string>& vectorProperties) {
    FileUtilityHashProvider fuhp;
    FileUtilityProviderLocal fupl(contextPath);

    if(fupl.isFolderExist()){
        fupl.setContext();

        if((fuhp.fileCalculateHash(fuhp.getVectorHashCur(), fupl.getFileList()) == true) && (fuhp.fileCalculateHash(fuhp.getVectorHashNew(), fupl.getFileList()) == true)){

            if(fuhp.equalVectors(fuhp.getVectorHashCur(), fuhp.getVectorHashNew())) {
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

    fupl.getFileProperties(vectorProperties, fupl.getFileList());
}

/** @see FileUtilityHashProvider::fileCalculateHash(std::vector<std::string>& vectorData, const std::vector<std::string>& vectorFileList) 
    Calculates hashes for files, skipping directories, returning true or false.

    @param vectorFileList - accepts a vector with path.
    @param vectorData - temporary hash storage.
    @param filePath - path to the file.
    @param calcHash - calls a function to calculate the hash.
    @param setVectorData - @see FileUtilityAlgorithmProvider::setVectorData

    @return true, if hash calculate successful, else false with error message.
*/
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

bool FileUtilityHashProvider::equalVectors(const std::vector<std::string> vectorFirst, const std::vector<std::string> vectorSecond) {

    if(!vectorFirst.empty() && !vectorSecond.empty()){
        if(vectorFirst.size() != vectorSecond.size()){
            std::cout << "Size not equals: " << vectorFirst.size() << " & " << vectorSecond.size() << std::endl;
            return false; // Size vectors must be equal.
        }

        for(szt i = 0; i < vectorFirst.size(); ++i){
            std::cout << "Current hashes being compared: " << vectorFirst[i] << " & " << vectorSecond[i] << std::endl;
            if(vectorFirst[i] != vectorSecond[i]){
                std::cout << "Vectors are not equal!" << std::endl;
                return false; // Elements with the same index are not equal.
            }
        }
    }
    else{
        std::cout << "vector(!?).empty()" << std::endl;
    }

    return true;
}