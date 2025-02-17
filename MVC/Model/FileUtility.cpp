#include "Header/FileUtility.h"
#include <AclAPI.h>
#include <accctrl.h>
#include <cstddef>
#include <errhandlingapi.h>
#include <minwindef.h>
#include <ostream>
#include <securitybaseapi.h>
#include <string>
#include <vector>
#include <winbase.h>
#include <winnt.h>

FileUtility::FileUtility() {}

FileUtilityProvider::FileUtilityProvider() {}

FileUtilityProviderLocal::FileUtilityProviderLocal() {}

FileUtilityAlgorithmProvider::FileUtilityAlgorithmProvider() {}

FileUtilityHashProvider::FileUtilityHashProvider() {}

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

/**

*/
std::string FileUtilityProviderLocal::getFilePropertiesOwner(std::filesystem::path fileSystemObjectPath){
    
    PSID pointerSidOwner = NULL;
    PSECURITY_DESCRIPTOR pointerSecurityDescriptor = NULL;

    const size_t maxName = 256;
    DWORD dwordSize = maxName;
    char accountName[maxName];
    char domainName[maxName];
    BOOL isSidAccountFound = TRUE;
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
        std::cout << "Handle file error: "  << dwErrorCode << std::endl; // Debug info
        dwErrorCode = GetLastError();
    }

    // Get the owner SID of the file.
    DWORD dwordSecurityInfoReturn = GetSecurityInfo(
                    fileHandle,                                 // Descriptor object from extracted information.
                    SE_FILE_OBJECT,                         // Object type, indication the type oj object.
                    OWNER_SECURITY_INFORMATION,           // Type security information, defines value which set or queried.
                    &pointerSidOwner,                       // Receives identifier security of the owner in the security descriptor returned in ppSecurityDescriptor.
                    NULL,                                   // Receives pointer to the identifier security main group in returned descriptor security.
                    NULL,                                       // Receives a pointer to the DACL(?) in the returned security descriptor.
                    NULL,                                       // Receives a pointer to the saCL(?) in the returned security descriptor.
                    &pointerSecurityDescriptor);  // Receives a pointer to the security descriptor object.

    // Check GetLastError for GetSecurityInfo error condition.
    if (dwordSecurityInfoReturn != ERROR_SUCCESS) {
            DWORD dwordErrorCode = 0;
            std::cout << "GetSecurityInfo error: "  << dwordErrorCode << std::endl; // Debug info
            dwordErrorCode = GetLastError();
    }

    // Call to LookupAccountSid to get 
    isSidAccountFound = LookupAccountSid(
                    NULL,                                 // Uses address machine, if "NULL" - local computer.
                    pointerSidOwner,                               // Pointer to a security identification for search.
                    accountName,                                  // Receiving null-terminated string, containing account name corresponding to the para meter LpSID.
                    &dwordSize,                                // Sets the size LpName. The Function fails if size buffer cattle or equals zero, otherwise receives size of the buffer with null symbol.
                    domainName,                   // Same as in accountName.
                    &dwordSize,
                    &eUse);                                      // A pointer to a variable that receives a SID_NAME_USE value that indicates the type of the account.

    // Check isSidAccountFound to the final value and if equals FALSE, then resets dwordErrorCode=0. And then an error is returned, otherwise the account name will be returned with or without the domain name.
    if (isSidAccountFound == FALSE) {
        DWORD dwordErrorCode = 0;
        dwordErrorCode = GetLastError();
        if (dwordErrorCode == ERROR_NONE_MAPPED)
            std::cout << "Account owner not found for specified SID: "  << dwordErrorCode << std::endl; // Debug info
        else
            std::cout << "Error in LookupAccountSid: "  << dwordErrorCode << std::endl; // Debug info
    } else if (isSidAccountFound == TRUE){
        std::cout << "Account owner: "  << accountName << domainName << std::endl; // Debug info
    }

    return accountName;
}

std::string FileUtilityProviderLocal::getFilePropertiesSize(std::filesystem::path fileSystemObjectPath){

    std::string sizeReturn = std::to_string(std::filesystem::file_size(fileSystemObjectPath) / unitSize);
    
    return sizeReturn;
}

/** @see FileUtilityProviderLocal::setContext()
    Sets the execution context within which the algorithm will be executed, namely the path to the directory that will be the root of the job. 

    @param getPath() - get path to root folder.
    @param entry.path() - check directory existence.
    @param fsStr - Returns the internal pathname in native pathname format, converted to specific string type.

    @return directoryFileList - vector storing file path.
*/
void FileUtilityProviderLocal::setContext(std::vector<std::string>& vectorPropertiesFileName, std::vector<std::string>& vectorPropertiesFileSize, std::vector<std::string>& vectorPropertiesFileType, std::vector<std::string>& vectorPropertiesOwner, std::vector<std::string>& vectorPropertiesDateTime, std::vector<std::string>& vectorPropertiesHash) {

    FileUtilityHashProvider fuhp;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(getPath())) {
        const std::filesystem::path& fsObj = entry.path(); 
        std::string fsStr = fsObj.u8string();

        if (!std::filesystem::is_directory(fsStr)) {
            std::replace(fsStr.begin(), fsStr.end(), '\\', '/');
            std::cout <<  "Path size: " << getFilePropertiesSize(fsStr) << " Folder path: " << fsStr << " Last time: " << getFilePropertiesTime(fsStr, filePropertiesTimeTypeEnum::TimeAccess) << std::endl;

            directoryFileList.push_back(fsStr);

            vectorPropertiesFileName.push_back(fsStr);
            vectorPropertiesFileSize.push_back(getFilePropertiesSize(fsStr));
            vectorPropertiesFileType.push_back("none");
            vectorPropertiesOwner.push_back(getFilePropertiesOwner(fsStr));
            vectorPropertiesDateTime.push_back(getFilePropertiesTime(fsStr, filePropertiesTimeTypeEnum::TimeAccess));
            vectorPropertiesHash.push_back(fuhp.fileCalculateHash(fsStr));
            
        } else {
            std::cout << "This is folder:" << fsStr << " ?" << std::endl;
        }
    }
}

bool FileUtilityProvider::isFolderExist() const {
    return std::filesystem::exists(getPath());
}

std::vector<std::string>& FileUtilityProviderLocal::getFileList() {
    std::cout << "addr in method: " << &directoryFileList << " size: " << directoryFileList.size() << std::endl; // Debug info.
    return directoryFileList;
}

void FileUtilityAlgorithmProvider::triggerAlgorithm(std::string contextPath, std::vector<std::string>& vectorPropertiesFileName, std::vector<std::string>& vectorPropertiesFileSize, std::vector<std::string>& vectorPropertiesFileType, std::vector<std::string>& vectorPropertiesOwner, std::vector<std::string>& vectorPropertiesDateTime, std::vector<std::string>& vectorPropertiesHash) {
    FileUtilityHashProvider fuhp;
    FileUtilityProviderLocal fupl(contextPath);
    
    fupl.setContext(vectorPropertiesFileName, vectorPropertiesFileSize, vectorPropertiesFileType, vectorPropertiesOwner, vectorPropertiesDateTime, vectorPropertiesHash);

}

/** @see std::string FileUtilityHashProvider::fileCalculateHash(const std::string& filePath)
    Calculates hashes for files, skipping directories, returning true or false.

    @param filePath - path to the file.
    @param calcHash - calls a function to calculate the hash.

    @return true, if hash calculate successful, else false with error message.
*/
std::string FileUtilityHashProvider::fileCalculateHash(const std::string& filePath) {

        std::string calcHash = sha256.calcHash(filePath);

        if(!calcHash.empty()){
            std::cout << "SHA256 hash for file: " << filePath << " : " << calcHash << std::endl;
        }
        else{
            std::cerr << "Error calculate hash for file: " << filePath << std::endl;
            return false;
        }

    return calcHash;
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