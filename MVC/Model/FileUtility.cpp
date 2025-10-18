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

std::string FileUtilityProviderLocal::filePropertiesTimeGet(std::filesystem::path fileSystemObjectPath, filePropertiesTimeTypeEnum filePropertiesTimeTypeEnum = filePropertiesTimeTypeEnum::TimeAccess) {
 
    // Get last write file time.
    auto timeLastWrite = std::filesystem::last_write_time(fileSystemObjectPath);
    
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

std::string FileUtilityProviderLocal::filePropertiesOwnerGet(std::filesystem::path fileSystemObjectPath) {

    struct stat fileStat;

    if (stat(fileSystemObjectPath.c_str(), &fileStat) == 0) {

        uid_t uid = fileStat.st_uid;
        gid_t gid = fileStat.st_gid;

        // Get username.
        struct passwd* pw = getpwuid(uid);
        std::string ownerName = pw ? pw->pw_name : std::to_string(uid);
        
        // Get username group.
        struct group* gr = getgrgid(gid);
        std::string groupName = gr ? gr->gr_name : std::to_string(gid);

        //
        return ownerName + ":" + groupName;
    }
    else {
        throw std::system_error(errno, std::system_category(), "Error stat for file.");
    }
}

#endif

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

/** @see std::string FileUtilityHashProvider::fileCalculateHash(const std::string& filePath)
    Calculates hashes for files, skipping directories, returning true or false.

    @param filePath - path to the file.
    @param calcHash - calls a function to calculate the hash.

    @return true, if hash calculate successful, else false with error message.
*/
std::string FileUtilityHashProvider::fileCalculateHash(const std::string &filePath) {

        SHA256Algorithm calculateHashSHA256;

        std::string calcHash = calculateHashSHA256.calcHash(filePath);

        if (calcHash.empty()) {
            std::cerr << "Error calculate hash for file: " << filePath << std::endl;
            return "Undefined";
        }

    return calcHash;
}

/** @see FileUtilityProviderLocal::fileMetadataCollectRecursively()
    Sets the execution context within which the algorithm will be executed, namely the path to the directory that will be the root of the job. 

    @param entry.path() - check directory existence.
    @param fsStr - Returns the internal pathname in native pathname format, converted to specific string type.

    @return directoryFileList - vector storing file path.
*/
void FileUtilityProviderLocal::fileMetadataCollectRecursively(std::string directoryRoot) {

    FileUtilityHashProvider fuhp;
    FileMetadataSnapshot fms;
    FileMetadataUtility fmu;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryRoot)) {
        const std::filesystem::path& fsObj = entry.path(); 
        std::string fsStr = fsObj.u8string();
        try {

            if (!std::filesystem::exists(fsStr)){
                std::cout << "File not found: " << fsStr << std::endl;
                continue;
            }

            if (!std::filesystem::is_directory(fsStr)) {
                std::replace(fsStr.begin(), fsStr.end(), '\\', '/');

                currentFileMetadata.filePath = fsStr;
                currentFileMetadata.fileName = fsObj.filename().u8string();
                currentFileMetadata.fileSize = filePropertiesSizeGet(fsObj);
                currentFileMetadata.fileTypeData = "none";
                currentFileMetadata.fileOwner = filePropertiesOwnerGet(fsStr);
                currentFileMetadata.fileDateTime = filePropertiesTimeGet(fsStr);
                // currentFileMetadata.fileHash = fuhp.fileCalculateHash(fsStr);
                currentFileMetadata.fileHash = "none";

                fms.metadataSnapshotUpdate(currentFileMetadata);
                
            } else {
                std::cout << "This is folder: " << fsStr << " ?" << std::endl;
            }

        } catch (const std::exception& e) {
            std::cerr << "Error getting information for: " << fsStr << " - " << e.what() << std::endl;
        }
    }
    
    fmu.fileMetadataUtilityCompare(fms, fms.metadataSnapshotGetAll());

    fms.metadataSnapshotSaveToFile();
    std::cout << "Snapshot the metadata has been created!" << std::endl;    
}

bool FileUtilityHashProvider::fileMetadataCompare() {
    return true;
}

void FileUtilityProviderLocal::fileMetadataClear() {
    currentFileMetadata = FileMetadata{};
}


void FileMetadataSnapshot::metadataSnapshotUpdate(const FileMetadata &metadata) {
    metadataSnapshot[metadata.filePath] = std::move(metadata);
}

bool FileMetadataSnapshot::metadataSnapshotContains(const std::string &path) const {
    return metadataSnapshot.find(path) != metadataSnapshot.end();
}

const FileMetadata* FileMetadataSnapshot::metadataSnapshotGet(const std::string &path) const {
    auto iteration = metadataSnapshot.find(path);
    if (iteration == metadataSnapshot.end()) {
        return nullptr;
    }
    return &iteration->second;
}

const std::unordered_map<std::string, FileMetadata>& FileMetadataSnapshot::metadataSnapshotGetAll() {
    return metadataSnapshot;
}

bool FileMetadataSnapshot::metadataSnapshotSaveToFile() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm time = *std::localtime(&time_t);
    
    std::stringstream stringStream;
    stringStream << "snapshot_"
            << std::put_time(&time, "%Y-%m-%d_%H-%M-%S")
            << ".dat";

    std::string fileName = stringStream.str();

    std::ofstream tempFile(fileName, std::ios::binary);

    if (!tempFile) return false;

    size_t countBytes = metadataSnapshot.size();
    tempFile.write(reinterpret_cast<const char*>(&countBytes), sizeof(countBytes));
    
    for (const auto& metadata : metadataSnapshot) {
        const auto& meta = metadata.second;
        metadataSnapshotWriteFile(tempFile, meta.filePath);
        metadataSnapshotWriteFile(tempFile, meta.fileName);
        metadataSnapshotWriteFile(tempFile, meta.fileSize);
        metadataSnapshotWriteFile(tempFile, meta.fileTypeData);
        metadataSnapshotWriteFile(tempFile, meta.fileOwner);
        metadataSnapshotWriteFile(tempFile, meta.fileDateTime);
        metadataSnapshotWriteFile(tempFile, meta.fileHash);
    }
    return true;
}

bool FileMetadataSnapshot::metadataSnapshotLoadLatestFile() {
    std::string latest;

    try {
        for (const auto &entry : std::filesystem::directory_iterator(".")) {
            if (entry.path().extension() == ".dat" && entry.path().string().find("snapshot_") != std::string::npos) {
                if (latest.empty() || entry.path().string() > latest) {
                    latest = entry.path().string();
                }
            }
        }

        metadataSnapshotLoadFromFile(latest);
        return true;
    }
    catch (const std::exception &e) {
        std::cerr << "Error load latest snapshot: " << e.what() << std::endl;
        return false;
    }
}

bool FileMetadataSnapshot::metadataSnapshotLoadFromFile(const std::string &fileName) {
    std::ifstream tempFile(fileName, std::ios::binary);
    if (!tempFile) return false;
    
    metadataSnapshot.clear();
    
    size_t countBytes;
    tempFile.read(reinterpret_cast<char*>(&countBytes), sizeof(countBytes));
    
    for (size_t i = 0; i < countBytes; ++i) {
        FileMetadata meta;
        meta.filePath = metadataSnapshotReadFile(tempFile);
        meta.fileName = metadataSnapshotReadFile(tempFile);
        meta.fileSize = metadataSnapshotReadFile(tempFile);
        meta.fileTypeData = metadataSnapshotReadFile(tempFile);
        meta.fileOwner = metadataSnapshotReadFile(tempFile);
        meta.fileDateTime = metadataSnapshotReadFile(tempFile);
        meta.fileHash = metadataSnapshotReadFile(tempFile);

        metadataSnapshot[meta.filePath] = meta;
    }
    return true;
}

void FileMetadataSnapshot::metadataSnapshotClear() {
    metadataSnapshot.clear();
}

void FileMetadataSnapshot::metadataSnapshotWriteFile(std::ofstream &file, const std::string &str) {
    size_t size = str.size();
    file.write(reinterpret_cast<const char*>(&size), sizeof(size));
    file.write(str.c_str(), size);
}

std::string FileMetadataSnapshot::metadataSnapshotReadFile(std::ifstream &file) {
    size_t size;
    file.read(reinterpret_cast<char*>(&size), sizeof(size));
    std::string str(size, ' ');
    file.read(&str[0], size);
    return str;
}

void FileMetadataUtility::fileMetadataUtilityCompare(const FileMetadataSnapshot &currentSnapshot, const std::unordered_map<std::string, FileMetadata> &collectMetadata) {
    FileMetadataSnapshot fms;

    if (!fms.metadataSnapshotLoadLatestFile()) {
        std::cout << "No previous snapshots were found. The next snapshot will be the first." << std::endl;
        return;
    }

    for (const auto &[path, metaCur] : collectMetadata) {
        const FileMetadata* metaPast = fms.metadataSnapshotGet(path);

        if (!metaPast) {
            std::cout << "NEW: " << metaCur.filePath << std::endl;
            continue;
        }

        if (metaPast->fileSize != metaCur.fileSize ||
            metaPast->fileHash != metaCur.fileHash ||
            metaPast->fileOwner != metaCur.fileOwner ||
            metaPast->fileDateTime != metaCur.fileDateTime ||
            metaPast->filePath != metaCur.filePath) {
            std::cout << "MOD: " << metaCur.filePath << std::endl;
            continue;
        }
    }

    for (const auto &[path, metaCur] : fms.metadataSnapshotGetAll()) {
        if (collectMetadata.find(path) == collectMetadata.end()) {
            std::cout << "DEL: " << metaCur.filePath << std::endl;
            continue;
        }
    }
}

void FileImage::imageCollect() {
    std::ofstream imageFile("backFile.dat", std::ios::binary);

    std::string pathCollectFiles = "/mnt/sda/utils/";

    const char imageSignature[] = "Test data";
    imageFile.write(imageSignature, sizeof(imageSignature));

    std::vector<std::string> filesPathRelativeList;

    for (const auto &entry : std::filesystem::recursive_directory_iterator(pathCollectFiles)) {
        if (entry.is_regular_file()) {
            std::string fileRelativePath = entry.path().lexically_relative(pathCollectFiles).string();
            filesPathRelativeList.push_back(fileRelativePath);
        }
    }

    size_t filesSize = filesPathRelativeList.size();
    imageFile.write(reinterpret_cast<const char*>(&filesSize), sizeof(filesSize));
    std::cout << "size_t filesSize: " << filesSize << std::endl;

    for (const auto &file : filesPathRelativeList) {
        std::string filePathAbsolute = pathCollectFiles + file;
        std::cout << "\nconst auto &file: " << filePathAbsolute << std::endl;

        size_t pathSize = filePathAbsolute.size();
        imageFile.write(reinterpret_cast<const char*>(&pathSize), sizeof(pathSize));
        imageFile.write(file.c_str(), pathSize);
        std::cout << "size_t pathSize: " << pathSize << std::endl;

        std::ifstream fileSource(filePathAbsolute, std::ios::binary);

        fileSource.seekg(0, std::ios::end);
        uintmax_t fileSourceSize = fileSource.tellg();
        fileSource.seekg(0, std::ios::beg);
        std::cout << "uintmax_t fileSourceSize: " << fileSourceSize << std::endl;

        imageFile.write(reinterpret_cast<const char*>(&fileSourceSize), sizeof(fileSourceSize));
        
        char buffer[4096];
        while (fileSource.read(buffer, sizeof(buffer)) || fileSource.gcount()) {
            imageFile.write(buffer, fileSource.gcount());
        }
        std::cout << "\nBacked up: " << filePathAbsolute << std::endl;
    }
    std::cout << "Backup created! Files: " << filesSize << std::endl;
}

void FileImage::imageDisperse() {
    std::ifstream imageFile("backFile.dat", std::ios::binary);

    std::string pathDisperseFiles = "/mnt/sda/test-restore/";

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
        std::cout << "std::string filePath(pathSize, ' '): " << filePath << std::endl;

        uintmax_t fileSize;
        imageFile.read(reinterpret_cast<char*>(&fileSize), sizeof(fileSize));
        std::cout << "uintmax_t fileSize: " << fileSize << std::endl;

        std::filesystem::path imageFilePathOriginal(filePath);

        std::filesystem::path pathDisperseFile = pathDisperseFiles / imageFilePathOriginal.parent_path();

        if (!std::filesystem::exists(pathDisperseFile)) {
            std::filesystem::create_directory(pathDisperseFile);
            std::cout << "std::filesystem::create_directory(pathDisperseFile) == true" << std::endl;
        } else {
            std::cout << "std::filesystem::create_directory(pathDisperseFile) == false" << std::endl;
        }

        std::filesystem::path imageFilePathDisperseRestore = pathDisperseFile / imageFilePathOriginal.filename();

        std::ofstream fileSourceOutput(imageFilePathDisperseRestore, std::ios::binary);

        char buffer[4096];
        uintmax_t fileReadTotal = 0;
        uintmax_t remainder = fileSize;
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
// Block of destructors

FileUtilityProviderLocal::~FileUtilityProviderLocal() {
    // std::cout << "FileUtilityProviderLocal destroyed." << std::endl;
}

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