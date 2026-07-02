#include "Headers/FileUtility.h"
#include <cstddef>
#include <ostream>
#include <string>
#include <filesystem>
#include <vector>
#include <unistd.h>
#include <chrono>

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms

#include <AclAPI.h>
#include <accctrl.h>
#include <errhandlingapi.h>
#include <minwindef.h>
#include <securitybaseapi.h>
#include <winbase.h>
#include <winnt.h>

#define slp(x) Sleep((x) * 1000)
#define szt size_t
#define unitSize 1000



/** @see FileUtilityProviderLocal::filePropertiesTimeGet()
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
std::string FileUtilityProviderLocal::filePropertiesTimeGet(std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum) {
    
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
std::string FileUtilityProviderLocal::filePropertiesOwnerGet(std::filesystem::path fileSystemObjectPath){
    
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

#elif __linux__ //Linux platforms

#define slp(x) usleep((x) * 1000000)
#define szt ssize_t
#define unitSize 1024

#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

std::string FileUtilityProviderLocal::filePropertiesPathGet(const std::filesystem::path fileSystemObjectPath) {
    std::string fsStrObj = fileSystemObjectPath.u8string();
    return fsStrObj;
}

std::string FileUtilityProviderLocal::filePropertiesNameGet(const std::filesystem::path fileSystemObjectPath) {
    std::string fsStrObj = fileSystemObjectPath.filename().u8string();
    return fsStrObj;
}

std::string FileUtilityProviderLocal::filePropertiesTimeGet(const std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum = filePropertiesTimeTypeEnum::TimeAccess) {
    
    std::string fsStrObj = fileSystemObjectPath.u8string();
    std::replace(fsStrObj.begin(), fsStrObj.end(), '\\', '/');

    // Get last write file time.
    auto timeLastWrite = std::filesystem::last_write_time(fsStrObj);
    
    // Convert file_time to system_clock::time_point.
    auto systemTime = std::chrono::system_clock::now() + (timeLastWrite - std::filesystem::file_time_type::clock::now());

    std::time_t time = std::chrono::system_clock::to_time_t(systemTime); // Convert time_point to arithmetic ty pe capable of representing times.
    std::tm timeManagement = *std::localtime(&time); // Get local time.

    // Get the total length of time since the "epoch" (usually 00:00:00 January 1, 1970 UTC for system_clock).
    auto duration = systemTime.time_since_epoch();
    // Convert the total duration to whole seconds, discarding the fractional part. For example: 1234567890 milliseconds → 1234567 seconds (discard 890 ms).
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    // Calculate the fractional part of a second and convert it to hundredths (centiseconds). (duration - seconds) - get only the fractional part (what is left after whole seconds).
    auto centiseconds = std::chrono::duration_cast<std::chrono::duration<int, std::centi>>(duration - seconds);

    // Milliseconds (thousandths)
    // auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration - seconds);
    // Microseconds (millionths)
    // auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds);
    // Nanoseconds (billionths)
    // auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds);    

    std::ostringstream result;
    result << std::put_time(&timeManagement, "%Y-%m-%dT%H:%M:%S") << "." << std::setfill('0') << std::setw(2) << centiseconds.count();; // 1997-07-16T19:20:30.45+03:00 \0 | ISO 8601 format.

    // Get the time zone offset in hours.
    std::time_t nowTime = std::time(nullptr);
    std::tm local_tm = *std::localtime(&nowTime);
    std::tm utc_tm = *std::gmtime(&nowTime);

    // Calculate the offset in hours.
    int timezoneOffset = local_tm.tm_hour - utc_tm.tm_hour;
    if (local_tm.tm_mday != utc_tm.tm_mday) {
        if (local_tm.tm_mday > utc_tm.tm_mday) {
            timezoneOffset += 24;
        } else {
            timezoneOffset -= 24;
        }
    }
    
    // Format the offset.
    if (timezoneOffset >= 0) {
        result << "+" << std::setfill('0') << std::setw(2) << timezoneOffset << ":00";
    } else {
        result << "-" << std::setfill('0') << std::setw(2) << -timezoneOffset << ":00";
    }

    return result.str();
}

std::string FileUtilityProviderLocal::filePropertiesOwnerGet(const std::filesystem::path fileSystemObjectPath) {

    std::string fsStrObj = fileSystemObjectPath.u8string();
    std::replace(fsStrObj.begin(), fsStrObj.end(), '\\', '/');

    struct stat fileStat;

    if (stat(fsStrObj.c_str(), &fileStat) == 0) {

        // Get username.
        uid_t uid = fileStat.st_uid;
        struct passwd* pw = getpwuid(uid);
        std::string ownerName = pw ? pw->pw_name : std::to_string(uid);
        
        // Get username group.
        gid_t gid = fileStat.st_gid;
        struct group* gr = getgrgid(gid);
        std::string groupName = gr ? gr->gr_name : std::to_string(gid);

        std::string strObj = ownerName + ":" + groupName;

        return strObj;
    }
    else {
        throw std::system_error(errno, std::system_category(), "Error stat for file.");
        return nullptr;
    }
}

#endif

FileImage::imageHeadMetadata FileImage::imageHeadMetadataDefault() {
    imageHeadMetadata ihm = {};
    return ihm;
}

FileImage::imageConfigurationMetadata FileImage::imageConfigurationMetadataDefault() {
    imageConfigurationMetadata icm = {};
    return icm;
}

FileImage::imageDataMetadata FileImage::imageDataMetadataDefault() {
    imageDataMetadata idm = {};
    return idm;
}

FileImage::imageFileMetadata FileImage::imageFileMetadataDefault() {
    imageFileMetadata ifm = {};
    return ifm;
}

FileImage::imageOffsetMetadata FileImage::imageOffsetMetadataDefault() {
    imageOffsetMetadata iom = {};
    return iom;
}

FileImage::imageFooterMetadata FileImage::imageFooterMetadataDefault() {
    imageFooterMetadata ifm = {};
    return ifm;
}

std::string FileUtilityProviderLocal::filePropertiesSizeGet(const std::filesystem::path filePath, const filePropertiesSizeEnum sizeUnit = filePropertiesSizeEnum::Bytes) {

    uintmax_t sizeBytes = std::filesystem::file_size(filePath);

    static const std::unordered_map<filePropertiesSizeEnum, uintmax_t> measureUnits = {
        { filePropertiesSizeEnum::Bytes, 1 },
        { filePropertiesSizeEnum::Kibibytes, 1024 },
        { filePropertiesSizeEnum::Mebibyte, 1024 * 1024 },
        { filePropertiesSizeEnum::Gibibyte, 1024 * 1024 * 1024 },
        { filePropertiesSizeEnum::Tebibytes, 1024ULL * 1024 * 1024 * 1024 },
        { filePropertiesSizeEnum::Pebibyte, 1024ULL * 1024 * 1024 * 1024 * 1024 },
        { filePropertiesSizeEnum::Exbibyte, 1024ULL * 1024 * 1024 * 1024 * 1024 * 1024 },
        { filePropertiesSizeEnum::Zebibyte, 1024ULL * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 },
        { filePropertiesSizeEnum::Yobibyte, 1024ULL * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 }
    };

    auto iterator = measureUnits.find(sizeUnit);

    if (iterator != measureUnits.end()) {
        return std::to_string(sizeBytes / iterator->second);
    }
   
    return "Undefined";
}

std::string FileUtilityProviderLocal::filePropertiesCalcHash(const std::filesystem::path fileSystemObjectPath) {
    SHA256Algorithm sha256;
    std::string fsStrObj = fileSystemObjectPath.u8string();
    std::replace(fsStrObj.begin(), fsStrObj.end(), '\\', '/');

    std::string calcHash = sha256.hashCalculateFile(fsStrObj);

    if (calcHash.empty()) {
        std::cerr << "[]> Error calculate hash for file: " << fsStrObj << std::endl;
        return "Undefined";
    }

    return calcHash;
}

void FileUtilityProviderLocal::fileMetadataCollect(std::vector<std::string> &fileList, std::ofstream &fileImage) {
    SHA256Algorithm sha256;
    FileImage fi;

    fileMetadata fm = fileMetadataDefault();
    std::ofstream fileLog("fileLog.dat", std::ios::binary | std::ios::app);

    for (const auto &file : fileList) {
        try {
            if (!std::filesystem::exists(file)){
                std::cout << "[]> File not found: " << file << std::endl;
                return;
            }
            if (!std::filesystem::is_directory(file)) {
                fm.filePath = filePropertiesPathGet(file);
                fm.fileName = filePropertiesNameGet(file);
                fm.fileSize = filePropertiesSizeGet(file);
                fm.fileTypeData = "none";
                fm.fileOwner = filePropertiesOwnerGet(file);
                fm.fileDateTime = filePropertiesTimeGet(file);
                fm.fileHash = filePropertiesCalcHash(file);

                std::string log = "[]> fm.filePath: " + fm.filePath
                + " fm.fileName: " + fm.fileName
                + " fm.fileSize: " + fm.fileSize
                + " fm.fileTypeData: " + fm.fileTypeData
                + " fm.fileOwner " + fm.fileOwner
                + " fm.fileDateTime " + fm.fileDateTime
                + " fm.fileHash " + fm.fileHash + "\n \n";
                fileLog.write(log.data(), log.length());

                fi.stringSerialize(fileImage, fm.filePath);
                fi.stringSerialize(fileImage, fm.fileName);
                fi.stringSerialize(fileImage, fm.fileSize);
                fi.stringSerialize(fileImage, fm.fileTypeData);
                fi.stringSerialize(fileImage, fm.fileOwner);
                fi.stringSerialize(fileImage, fm.fileDateTime);
                fi.stringSerialize(fileImage, fm.fileHash);

            } else {
                std::cout << "This is folder: " << file << " ?" << std::endl;
            }
        } 
        catch (const std::exception& e) {
            std::cerr << "Error during processing a file: " << file << " - " << e.what() << std::endl;
        }
        std::cout << "[]> Collect metadata about files - Completed!" << std::endl;
    }
}

void FileImage::stringSerialize(std::ofstream &fileImage, const std::string &str) {
    uint64_t strSize = str.size();

    fileImage.write(reinterpret_cast<const char*>(&strSize), sizeof(strSize));
    fileImage.write(str.data(), strSize);
}

FileImage::imageIndexMetadata FileImage::imageIndexMetadataDefault() {
    imageIndexMetadata iim = {};
    
    iim.hash[32] = {0};
    iim.chunkSizeUncompressed = 0;
    iim.chunkSizeCompressed = 0;
    iim.chunkCompressionAlgorithm = 0;
    iim.chunkFlags = 0;
    iim.reserved[462] = {0};

    return iim;
}

void FileImage::encodeBlocksWithHash(std::vector<std::string> &fileList, std::ofstream &fileImage, std::vector<imageIndexMetadata> &viim) {
    SHA256Algorithm sha256;
    const std::size_t bufferDataSize = 4 * 1024;
    std::vector<char> bufferData(bufferDataSize);

    std::ofstream fileLog("fileLog.dat", std::ios::binary | std::ios::app);

    for (const auto &file : fileList) {
        std::ifstream fileSource(file, std::ios::binary);
        
        while (fileSource.read(bufferData.data(), bufferDataSize) || fileSource.gcount() > 0) {
            imageIndexMetadata iim = imageIndexMetadataDefault();

            std::string hash = sha256.hashCalculateBlock(bufferData.data());
            std::streamsize sz = fileSource.gcount();
            uint64_t sz64 = static_cast<uint64_t>(sz);
            
            std::vector<char> buffer(hash.size() + sizeof(sz64) + sz);

            std::memcpy(buffer.data(), hash.data(), hash.size());
            std::memcpy(iim.hash, hash.data(), hash.size());
            std::memcpy(buffer.data() + hash.size(), &sz64, sizeof(sz64));
            // iim.chunkSizeUncompressed = sz64;
            std::memcpy(buffer.data() + hash.size() + sizeof(sz64), bufferData.data(), sz);

            std::string log = "[]> HASH: " + hash + " HASH SIZE: " + std::to_string(hash.size()) + " SZ: " + std::to_string(sz) + " SZ64: " + std::to_string(sz64) + " Offset " + std::to_string(fileImage.tellp()) + "\n";
            fileLog.write(log.data(), log.length());

            // viim.push_back(iim);

            fileImage.write(buffer.data(), buffer.size());
        }
    }
}

std::vector<std::string> FileImage::fileCollectRecursively(const std::string& pathSource) {
    std::vector<std::string> result;
    for (const auto &entry : std::filesystem::recursive_directory_iterator(pathSource)) {
        if (entry.is_regular_file()) {
            result.push_back(std::filesystem::absolute(entry.path()).string());
            std::cout << "[]> File added in queue: " << entry.path().c_str() << std::endl;
        }
    }
    return result;
}

void FileImage::imageCollect(const std::string& pathSource, const std::string& fileOutput) {
    FileUtilityProviderLocal fupl;

    std::vector<std::string> filesPathRelativeList = fileCollectRecursively(pathSource);
    std::ofstream imageFile(fileOutput, std::ios::binary);

    imageHeadMetadata ihm = imageHeadMetadataDefault();

    imageFile.write(reinterpret_cast<const char*>(&ihm), sizeof(ihm));
    
    // Configuration backup file.
    #pragma pack(push, 1)
    struct imageConfiguration
    {
        uint16_t reserved[512] = {0};
    };
    #pragma pack(pop)
    imageConfiguration config = {};
    imageFile.write(reinterpret_cast<const char*>(&config), sizeof(config));

    // Table footer image.
    #pragma pack(push, 1)
    struct imageFooter
    {
        uint16_t reserved[512] = {0};
    };
    #pragma pack(pop)
    imageFooter footer = {};

    std::vector<imageIndexMetadata> viim;

    encodeBlocksWithHash(filesPathRelativeList, imageFile, viim);

    fupl.fileMetadataCollect(filesPathRelativeList, imageFile);

    imageFile.write(reinterpret_cast<const char*>(&footer), sizeof(footer));

    std::cout << "Backup created!" << std::endl;
}

void FileImage::imageDisperse(const std::string& pathSource, const std::string& fileOutput) {
    std::ifstream imageFile(fileOutput, std::ios::binary);

    std::string pathDisperseFiles = pathSource;

    char signature[10];
    imageFile.read(signature, sizeof(signature));
    std::cout << "\nchar signature[10]: " << signature << std::endl;

    size_t filesSize;
    imageFile.read(reinterpret_cast<char*>(&filesSize), sizeof(filesSize));
    std::cout << "size_t filesSize: " << filesSize << std::endl;

    for (size_t s = 0; s < filesSize; s++) {
        size_t pathSize;
        imageFile.read(reinterpret_cast<char*>(&pathSize), sizeof(pathSize));
        std::cout << "\nsize_t pathSize: " << pathSize << std::endl;

        std::string filePath(pathSize, ' ');
        imageFile.read(&filePath[0], pathSize);
        std::cout << "std::string filePath(pathSize, ' '): " << filePath.c_str() << std::endl;

        uint64_t fileSize;
        imageFile.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
        std::cout << "uint64_t fileSize: " << fileSize << std::endl;

        std::filesystem::path imageFilePathOriginal(filePath);

        std::filesystem::path pathDisperseFile = pathDisperseFiles / imageFilePathOriginal.parent_path();

        if (!std::filesystem::exists(pathDisperseFile)) {
            std::filesystem::create_directories(pathDisperseFile);
            std::cout << "std::filesystem::create_directory(pathDisperseFile) == true" << std::endl;
        } else {
            std::cout << "std::filesystem::create_directory(pathDisperseFile) == false" << std::endl;
        }

        std::filesystem::path imageFilePathDisperseRestore = pathDisperseFile / imageFilePathOriginal.filename();

        std::ofstream fileSourceOutput(imageFilePathDisperseRestore, std::ios::binary);

        char buffer[4096];
        uint64_t fileReadTotal = 0;
        uint64_t remainder = fileSize;
        while (remainder > 0) {
            size_t read = std::min(sizeof(buffer), static_cast<size_t>(remainder));
            imageFile.read(buffer, read);
            fileSourceOutput.write(buffer, imageFile.gcount());
            remainder -= imageFile.gcount();
        }
        std::cout << "Restored: " << imageFilePathDisperseRestore.string() << " ( " << fileSize << " bytes)" << std::endl;
    }
    std::cout << "Restore completed! Files: " << filesSize << std::endl;

}

#pragma region plugs

FileUtilityProviderLocal::~FileUtilityProviderLocal() {}

FileUtilityAlgorithmProvider::~FileUtilityAlgorithmProvider() {
    // std::cout << "FileUtilityAlgorithmProvider destroyed." << std::endl;
}

FileUtilityHashProvider::~FileUtilityHashProvider() {
    // std::cout << "FileUtilityHashProvider destroyed." << std::endl;
}

FileUtility::~FileUtility() {
    // std::cout << "FileUtility destroyed." << std::endl;
}

FileUtilityProvider::~FileUtilityProvider() {
    // std::cout << "FileUtilityProvider destroyed." << std::endl;
}

#pragma endregion