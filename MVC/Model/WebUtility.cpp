#include "Header/WebUtility.h"
#include <string>

struct attr attrKey[] = {
    { "File name", "File size", "File type", "Owner", "Date creation", "Hash" },
    { NULL, NULL,NULL, NULL, NULL, NULL }
};

WebUtility::WebUtility() {}

std::vector<std::string> WebUtility::vectorFilesPropertiesFileName;
std::vector<std::string> WebUtility::vectorFilesPropertiesFileSize;
std::vector<std::string> WebUtility::vectorFilesPropertiesFileType;
std::vector<std::string> WebUtility::vectorFilesPropertiesOwner;
std::vector<std::string> WebUtility::vectorFilesPropertiesDateTime;
std::vector<std::string> WebUtility::vectorFilesPropertiesHash;

std::vector<std::string>& WebUtility::getVectorFilePropertiesFileName(){
    std::cout << "vectorFilesPropertiesFileName size: " << vectorFilesPropertiesFileName.size() << std::endl;
    return vectorFilesPropertiesFileName;
}

std::vector<std::string>& WebUtility::getVectorFilePropertiesFileSize(){
    std::cout << "vectorFilesPropertiesFileSize size: " << vectorFilesPropertiesFileSize.size() << std::endl;
    return vectorFilesPropertiesFileSize;
}

std::vector<std::string>& WebUtility::getVectorFilePropertiesFileType(){
    std::cout << "vectorFilesPropertiesFileType size: " << vectorFilesPropertiesFileType.size() << std::endl;
    return vectorFilesPropertiesFileType;
}

std::vector<std::string>& WebUtility::getVectorFilePropertiesOwner(){
    std::cout << "vectorFilesPropertiesOwner size: " << vectorFilesPropertiesOwner.size() << std::endl;
    return vectorFilesPropertiesOwner;
}

std::vector<std::string>& WebUtility::getVectorFilePropertiesDateTime(){
    std::cout << "vectorFilesPropertiesDateTime size: " << vectorFilesPropertiesDateTime.size() << std::endl;
    return vectorFilesPropertiesDateTime;
}

std::vector<std::string>& WebUtility::getVectorFilePropertiesHash(){
    std::cout << "vectorFilesPropertiesHash size: " << vectorFilesPropertiesHash.size() << std::endl;
    return vectorFilesPropertiesHash;
}
// size_t WebUtility::returnAttr(void (*out)(char, void *), void *ptr, va_list *ap) {

//     size_t len = 0;
//     struct attr *val = va_arg(*ap, struct attr *);
//     struct attr *key = attrKey;

//     char *constructor = "\"%s\": \"%s\",";
//     char *endConstructor = "\"%s\": \"%s\"";

//     len += mg_xprintf(out, ptr, "{");
//     len += mg_xprintf(out, ptr, "\"Files\": [");

//     const char *keys[] = { key->fileName, key->fileSize, key->typeData, key->owner, key->dateTime, key->hash };

//     for (int i = 0; val[i].fileName != NULL; i++) {

//         const char *varFields[] = { val[i].fileName, val[i].fileSize, val[i].typeData, val[i].owner, val[i].dateTime, val[i].hash };

//         len += mg_xprintf(out, ptr, "{");

//         if(strcmp(val[i].fileName, "NULL") != 0){
//             char buffer[1024];
//             int offset = 0;

//             offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->fileName, val[i].fileName);
//             offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->fileSize, val[i].fileSize);
//             offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->typeData, val[i].typeData);
//             offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->owner, val[i].owner);
//             offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->dateTime, val[i].dateTime);
//             offset += snprintf(buffer + offset, sizeof(buffer) - offset, endConstructor, key->hash, val[i].hash);

//             len += mg_xprintf(out, ptr, "%s", buffer);
//         }

//         if (val[i + 1].fileName != NULL) {
//             len += mg_xprintf(out, ptr, "},");
//         } else {
//             len += mg_xprintf(out, ptr, "}");
//         }
//     }
//     return len += mg_xprintf(out, ptr, "]}");
// }

size_t WebUtility::returnApiListFiles(void (*out)(char, void *), void *ptr, va_list *ap){

    size_t len = 0;
    // struct attr *val = va_arg(*ap, struct attr *);
    struct attr *key = attrKey;
    std::vector<attr> atr;

    atr.push_back({ " E:/TempFilesProgramm/OSSS/TempFolder/rename_factions/rename_factions/localisation/rename_faction_l_english.yml ", " 91011 ", " text/plain ", " User ",  " 1997-07-16T19:20:30.45+03:00 ", " f5788b96310c9174411ea51777f675b8e0735fd51a4ae732c3d078987a953160 " });

    char *constructor = "{\"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\"}";

    char* keyHead = "Head";

    len += mg_xprintf(out, ptr, "{\"%s\": [", keyHead );

    for(int n = 0; n < getVectorFilePropertiesFileName().size(); n++) {
        char buffer[1024];
        size_t dataBuffer = 0;

        dataBuffer += snprintf(buffer + dataBuffer, sizeof(buffer) - dataBuffer, constructor, attrKey->fileName, getVectorFilePropertiesFileName()[n].data(), attrKey->fileSize, getVectorFilePropertiesFileSize()[n].data(), attrKey->typeData, getVectorFilePropertiesFileType()[n].data(), attrKey->owner, getVectorFilePropertiesOwner()[n].data(), attrKey->dateTime, getVectorFilePropertiesDateTime()[n].data(), attrKey->hash, getVectorFilePropertiesHash()[n].data());

        if(n >= (getVectorFilePropertiesFileName().size() - 1)) {
            dataBuffer += snprintf(buffer + dataBuffer, sizeof(buffer) - dataBuffer, "");
        }
        else {
            dataBuffer += snprintf(buffer + dataBuffer, sizeof(buffer) - dataBuffer, ",");
        }
        len += mg_xprintf(out, ptr, "%s", buffer);
    }

    len += mg_xprintf(out, ptr, "]}");

    return len;
}

/** @see WebUtility::httpHandler
    The function processing http events by number event. 

    @param headers - HTTP request or response that passes additional context and metadata about the request or response.
    @param MG_EV_HTTP_MSG - Full HTTP request/response. 
    @param event - event processing by event number. @see https://mongoose.ws/documentation/#events
    @param mg_http_message - handler message. Parsed HTTP request. @see https://mongoose.ws/documentation/#struct-mg_http_message
    @param uri - handler based on url response/request.

    @return directoryFileList - vector storing file path
*/
void WebUtility::httpHandler(struct mg_connection *connection, int event, void *event_data) {

    const char *headers = "content-type: text/json\r\n";

    if (event == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm =(struct mg_http_message*) event_data;

        if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {
            mg_http_reply(connection, 200, headers, "%M", returnApiListFiles);
        }
        else {
            struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
            mg_http_serve_dir(connection, hm, &opts);
        }
    }
}