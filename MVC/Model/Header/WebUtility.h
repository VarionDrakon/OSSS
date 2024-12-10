#include "../../../lib/mongoose/mongoose.h"
#include <string>
#include <vector>

struct attr { 
    const char *fileName, *fileSize, *typeData, *owner, *dateTime, *hash; /*
                *fileSize: It's necessary :)
                *typeData: https://www.iana.org/assignments/media-types/media-types.xhtml, write format - "text/plain" 
                *dateTime: https://www.w3.org/TR/NOTE-datetime, write format - YYYY-MM-DDThh:mm:ss.sTZD (eg 1997-07-16T19:20:30.45+03:00) 16.07.1997 time 19:20:30.45 according to Moscow time
                */
};

struct attr attrKey[] = {
    { "File name", "File size", "File type", "Owner", "Date creation", "Hash" },
    { NULL, NULL,NULL, NULL, NULL, NULL }
};

struct attr attrValue[] = {
    { "fileName1.txt", "91011", "text/plain", "User",  "1997-07-16T19:20:30.45+03:00", "f5788b96310c9174411ea51777f675b8e0735fd51a4ae732c3d078987a953160" },
    { "fileName2.txt", "67898", "text/plain", "User",  "1997-07-16T19:20:30.45+03:00", "f5788b96310c9174411ea51777f675b8e0735fd51a4ae732c3d078987a953160" },
    { "fileName3.txt", "23452", "text/plain", "User",  "1997-07-16T19:20:30.45+03:00", "f5788b96310c9174411ea51777f675b8e0735fd51a4ae732c3d078987a953160" },
    { "fileName4.txt", "23452", "text/plain", "User",  "1997-07-16T19:20:30.45+03:00", "f5788b96310c9174411ea51777f675b8e0735fd51a4ae732c3d078987a953160" },
    { NULL, 0,NULL, NULL, NULL, NULL }
};

class WebUtility {
    protected:

    private: 

    public:

        WebUtility();

        static void httpHandler(struct mg_connection *connection, int event, void *event_data);

        static size_t returnAttr(void (*out)(char, void *), void *ptr, va_list *ap);

        virtual ~WebUtility() {}
};