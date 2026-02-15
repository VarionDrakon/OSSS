#ifndef FILECORE_H
#define FILECORE_H


#include <string>

struct FileMetadata {
    std::string filePath;
    std::string fileName;
    std::string fileSize;
    std::string fileTypeData; // * https://www.iana.org/assignments/media-types/media-types.xhtml, write format - "text/plain" 
    std::string fileOwner;
    std::string fileDateTime; // * https://www.w3.org/TR/NOTE-datetime, write format - YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+03:00) 16.07.1997 time 19:20:30.45 according to Moscow time
    std::string fileHash;
};

#endif // FILECORE_H