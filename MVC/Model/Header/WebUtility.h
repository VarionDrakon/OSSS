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

class WebProviderObject {
    protected:

    private: 

    public:

        static size_t returnAttr(void (*out)(char, void *), void *ptr, va_list *ap) {

                struct attr *val = va_arg(*ap, struct attr *);
                struct attr *key = attrKey;
                size_t len = 0;
                char *constructor = "\"%s\": \"%s\",\n";
                char *endConstructor = "\"%s\": \"%s\"\n";

                len += mg_xprintf(out, ptr, "{\n");
                len += mg_xprintf(out, ptr, "\"Files\": [ \n");

                const char *keys[] = { key->fileName, key->fileSize, key->typeData, key->owner, key->dateTime, key->hash };
                

                for (int i = 0; val[i].fileName != NULL; i++){
                    const char *varFields[] = { val[i].fileName, val[i].fileSize, val[i].typeData, val[i].owner, val[i].dateTime, val[i].hash };

                    len += mg_xprintf(out, ptr, "{\n");

                    if(strcmp(val[i].fileName, "NULL") != 0){
                        char buffer[1024];
                        int offset = 0;

                        offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->fileName, val[i].fileName);
                        offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->fileSize, val[i].fileSize);
                        offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->typeData, val[i].typeData);
                        offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->owner, val[i].owner);
                        offset += snprintf(buffer + offset, sizeof(buffer) - offset, constructor, key->dateTime, val[i].dateTime);
                        offset += snprintf(buffer + offset, sizeof(buffer) - offset, endConstructor, key->hash, val[i].hash);

                        len += mg_xprintf(out, ptr, "%s", buffer);
                    }

                    if (val[i + 1].fileName != NULL) {
                        len += mg_xprintf(out, ptr, "},\n");
                    } else {
                        len += mg_xprintf(out, ptr, "}\n");
                    }
                }
                return len += mg_xprintf(out, ptr, "]}");
            }

    virtual ~WebProviderObject() {}
};