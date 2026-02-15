#ifndef WEBUTILITY_H
#define WEBUTILITY_H

#include <mongoose/mongoose.h>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <cstdarg>

#include "../../Core/Headers/FileCore.h"

class WebUtility {
    protected:

    private:
        static std::vector<FileMetadata> fileMetadata; 

    public:
        void fileMetadataSet(const std::vector<FileMetadata> &fm);

        WebUtility();
        
        static std::vector<std::string>& getHash(std::vector<std::string>);
        static void httpHandler(struct mg_connection *connection, int event, void *event_data);
        static void HTTPServer();
        static void ev_handler(struct mg_connection *c, int ev, void *ev_data);
        static void returnApiListFiles(void (*connection)(char, void*), char* ptr);
        // static size_t returnAttr(void (*out)(char, void *), void *ptr, va_list *ap);

        virtual ~WebUtility() {}
};

#endif // WEBUTILITY_H