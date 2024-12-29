#include "../../../lib/mongoose/mongoose.h"
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

struct attr { 
    const char *fileName, *fileSize, *typeData, *owner, *dateTime, *hash; /*
                *fileSize: It's necessary :)
                *typeData: https://www.iana.org/assignments/media-types/media-types.xhtml, write format - "text/plain" 
                *dateTime: https://www.w3.org/TR/NOTE-datetime, write format - YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+03:00) 16.07.1997 time 19:20:30.45 according to Moscow time
                */
    void printInfo() const {
        std::cout << fileName << fileSize << typeData << owner << dateTime << hash << std::endl;
    }
};

struct attr attrKey[] = {
    { "File name", "File size", "File type", "Owner", "Date creation", "Hash" },
    { NULL, NULL,NULL, NULL, NULL, NULL }
};

class WebUtility {
    protected:

    private: 

    public:

        WebUtility();

        static std::vector<std::string>& getHash(std::vector<std::string>);
        static void httpHandler(struct mg_connection *connection, int event, void *event_data);
        static size_t returnApiListFiles(void (*out)(char, void *), void *ptr, va_list *ap);
        // static size_t returnAttr(void (*out)(char, void *), void *ptr, va_list *ap);

        virtual ~WebUtility() {}
};