#include "Header/FileUtility.h"
#include <AclAPI.h>
#include <accctrl.h>
#include <cstddef>
#include <errhandlingapi.h>
#include <minwindef.h>
#include <securitybaseapi.h>
#include <winbase.h>
#include <winnt.h>

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
/** @see FileUtilityProviderLocal::getFilePropertiesTime()
    This function creates and returns an object representing the size of the object in memory and the number of elements in it. The function takes two arguments, the path to the file and the desired return file time, and then forms a string from the constructor. Since some file systems have two or three file time options, some can be combined into one value. Also, inside each constructor, the time is converted to local time at the same address where the original time is stored.

    @param fileSystemObjectPropertiesFT - this structure is used for representations of time in hundred of nanoseconds, starting from 1 january 1601 years (UTC).
    @param fileSystemObjectPropertiesST - this structure is used for representations of time in a format convenient for humans. Example wYear - years, wMonth is month and so on.
    @param systemObjectTimeZoneTZI - this structure keep info about the current timezone.  
    @param fileHandle - this abstraction is used to manage system object. It is also a pointer to a system object.
    @param timeReturnBufferData - this buffer that limits the capacity of characters and also stores data written to it.
    @param timeReturnConstructor - this constructor string is used to format the return value.
    @param timeReturnBuffer - returns the data passed to it from the function.
    @param offsetSignUTC - returns a character depending on the time zone offset.
    @param offsetHoursUTC - returns a hours depending on the time zone offset
    @param offsetMinutesUTC - returns a minutes depending on the time zone offset

*/
std::string FileUtilityProviderLocal::getFilePropertiesTime(std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum) {
    
        FILETIME fileSystemObjectPropertiesFT;
        SYSTEMTIME fileSystemObjectPropertiesST;
        TIME_ZONE_INFORMATION systemObjectTimeZoneTZI;
        size_t timeReturnBuffer = 0;
        
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

void FileUtilityProviderLocal::getFilePropertiesOwner(std::filesystem::path fileSystemObjectPath){
    
    PSID pointerSIDOwner = NULL;
    PSECURITY_DESCRIPTOR pointerSecurityDescriptor = NULL;

    const size_t maxName = 256;
    DWORD dwordSize = maxName;
    char accountName[maxName];
    char domainName[maxName];
    BOOL isSIDAccountFound = TRUE;
    SID_NAME_USE eUse = SidTypeUnknown;

    // Get the handle of the file object.
    HANDLE fileHandle = CreateFileW(
                    fileSystemObjectPath.c_str(),
                    GENERIC_READ,
                    FILE_SHARE_READ,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    NULL);

    // Check GetLastError for Handle file error code.
    if (fileHandle == INVALID_HANDLE_VALUE) {
        DWORD dwErrorCode = 0;
        std::cout << "Handle file error: "  << dwErrorCode << std::endl;
        dwErrorCode = GetLastError();
    }

    // Get the owner SID of the file.
    DWORD dwordSecurityInfoReturn = GetSecurityInfo(
                    fileHandle,                                 // Descriptor object from extracted information.
                    SE_FILE_OBJECT,                         // Object type, indication the type oj object.
                    OWNER_SECURITY_INFORMATION,           // Type security information, defines value which set or queried.
                    &pointerSIDOwner,                       // Receives identifier security of the owner in the security descriptor returned in ppSecurityDescriptor.
                    NULL,                                   // Receives pointer to the identifier security main group in returned descriptor security.
                    NULL,                                       // Receives a pointer to the DACL(?) in the returned security descriptor.
                    NULL,                                       // Receives a pointer to the saCL(?) in the returned security descriptor.
                    &pointerSecurityDescriptor);  // Receives a pointer to the security descriptor object.

    // Check GetLastError for GetSecurityInfo error condition.
    if (dwordSecurityInfoReturn != ERROR_SUCCESS) {
            DWORD dwordErrorCode = 0;
            std::cout << "GetSecurityInfo error: "  << dwordErrorCode << std::endl;
            dwordErrorCode = GetLastError();
    }


    // Call to LookupAccountSid to get 
    isSIDAccountFound = LookupAccountSid(
                    NULL,                                 // Uses address machine, if "NULL" - local computer.
                    pointerSIDOwner,                               // Pointer to a security identification for search.
                    accountName,                                  // Receiving null-terminated string, containing account name corresponding to the para meter LpSID.
                    &dwordSize,                                // Sets the size LpName. The Function fails if size buffer cattle or equals zero, otherwise receives size of the buffer with null symbol.
                    domainName,                   // Same as in accountName.
                    &dwordSize,
                    &eUse);                                      // A pointer to a variable that receives a SID_NAME_USE value that indicates the type of the account.

    // Check isSIDAccountFound to the final value and if equals FALSE, then resets dwordErrorCode=0. And then an error is returned, otherwise the account name will be returned with or without the domain name.
    if (isSIDAccountFound == FALSE) {
        DWORD dwordErrorCode = 0;
        dwordErrorCode = GetLastError();
        if (dwordErrorCode == ERROR_NONE_MAPPED)
            std::cout << "Account owner not found for specified SID: "  << dwordErrorCode << std::endl; 
        else
            std::cout << "Error in LookupAccountSid: "  << dwordErrorCode << std::endl; 
    } else if (isSIDAccountFound == TRUE){
        std::cout << "Account owner: "  << accountName << domainName << std::endl; 
    }
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
            getFilePropertiesOwner(fsStr);
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