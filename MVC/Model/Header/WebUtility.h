#include "../../../lib/mongoose/mongoose.h"
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

struct fileProperties {
    const char *fileName;
    int fileSize;
};

struct fileProperties filePropertiesList[] = {
    { "fileName1.txt", 91011 },
    { "fileName2.txt", 67898 },
    { "fileName3.txt", 23452 },
    { "fileName4.txt", 23452 },
    { NULL, 0 }
};
struct attribute {
    const char *name;
    const char *type;
    const char *format;
    size_t offset;
    size_t size;
};

struct attribute filePropertiesAttributes[] = {
    { "fileName", "string", NULL, offsetof(struct fileProperties, fileName), 0 },
    { "size", "int", NULL, offsetof(struct fileProperties, fileSize), 0 },
    { NULL, NULL, NULL, 0, 0 }
};


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

static std::vector<std::string> dataArray;
class WebUtility {
    protected:

        static size_t returnData(void (*out)(char, void *), void *ptr, va_list *ap){

            struct attribute *attr = filePropertiesAttributes;
            struct fileProperties *fP = (struct fileProperties *) ptr;
            
            size_t i, len = 0;

            len += mg_xprintf(out, ptr, "{");

            for (i = 0; filePropertiesAttributes[i].name != NULL; i++){
                
                char *attrptr = (char *) fP + attr[i].offset;

                len += mg_xprintf(out, ptr, "\"%s\"", attr[i].name);
                len += mg_xprintf(out, ptr, ": ");

                if (strcmp(attr[i].type, "string") == 0){

                    len += mg_xprintf(out, ptr, "\"%s\"", *(const char *)attrptr); 
                }  
                else if (strcmp(attr[i].type, "int") == 0){

                    len += mg_xprintf(out, ptr, "\"%d\"", *(size_t *)attrptr);
                }
                else {
                    len += mg_xprintf(out, ptr, "null");
                }

                if(attr[i + 1].name != NULL){
                    len += mg_xprintf(out, ptr, ", ");
                }
            }

            return mg_xprintf(out, attr, "{%m:%d}", MG_ESC("a"), attr->name);
            // struct a { int a, b; };
            // size_t print_a(void (*out)(char, void *), void *ptr, va_list *ap) {
            //  struct a *ptr = va_arg(*ap, struct a *);
            //  return mg_xprintf(out, ptr, "{%m:%d}", MG_ESC("a"), ptr->a); // MG_ESC invokes mg_print_esc
            // }

            // struct a temp = { 42, 43 };
            // mg_snprintf(buf, sizeof(buf), "%M", print_a, &temp);    // {"a":42}
        }

            // size_t mg_print_esc(void (*out)(char, void *), void *arg, va_list *ap) {
            // size_t len = (size_t) va_arg(*ap, int);
            // char *p = va_arg(*ap, char *);
            // if (len == 0) len = p == NULL ? 0 : strlen(p);
            // return qcpy(out, arg, p, len);
            // }


        static size_t returnAttr(void (*out)(char, void *), void *ptr, va_list *ap) {

            struct attr *val = va_arg(*ap, struct attr *);
            struct attr *key = attrKey;
            size_t len = 0;
            char *constructor = "\"%s\": \"%s\",\n";
            char *endConstructor = "\"%s\": \"%s\"\n";

            len += mg_xprintf(out, ptr, "{\n");
            len += mg_xprintf(out, ptr, "\"Files\": [ \n");

            for (int i = 0; val[i].fileName != NULL; i++){
                len += mg_xprintf(out, ptr, "{\n");
                    // if (strcmp(key->typeData, "string") == 0){
                    //     len += mg_xprintf(out, ptr, "\"string\"");
                    // }
                    // else if (strcmp(key->typeData, "int") == 0){
                    //     len += mg_xprintf(out, ptr, "\"int\"");
                    // }
                    // else {
                    //     std::cout << i << " null" << std::endl;
                    // }
                    // std::cout << i << " cycl" << std::endl;
                if(strcmp(val[i].fileName, "NULL") != 0){
                    len += mg_xprintf(out, ptr, constructor, key->fileName, val[i].fileName);
                    len += mg_xprintf(out, ptr, constructor, key->fileSize, val[i].fileSize);
                    len += mg_xprintf(out, ptr, constructor, key->typeData, val[i].typeData);
                    len += mg_xprintf(out, ptr, constructor, key->owner, val[i].owner);
                    len += mg_xprintf(out, ptr, constructor, key->dateTime, val[i].dateTime);
                    len += mg_xprintf(out, ptr, endConstructor, key->hash, val[i].hash);
                }

                if (val[i + 1].fileName != NULL) {
                    len += mg_xprintf(out, ptr, "},\n");
                } else {
                    len += mg_xprintf(out, ptr, "}\n");
                }
                    //len += mg_xprintf(out, ptr, { ": %s: \"%s\": \"%s\": \"%s\": \"%s\"\n"}, val[i].fileSize, val[i].typeData, val[i].owner, val[i].dateTime, val[i].hash); // { "attr_data": "temp", ( key: value )}
            }
            
            return len += mg_xprintf(out, ptr, "]}");
        }


    private: 

    public:

        static void fn(struct mg_connection *connection, int event, void *event_data) {
            if (event == MG_EV_HTTP_MSG) {
                struct mg_http_message *hm =(struct mg_http_message*) event_data;
                if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {

                    const char *headers = "content-type: application/json\r\n";

                    
                    // for (size_t i = 0; filePropertiesList[i].fileName != NULL; i++){
                    //     size_t len = returnData(outputFunction, &filePropertiesList[i], NULL);

                    //     if(filePropertiesList[i].fileName != NULL){
                    //         mg_http_reply(connection, 200, headers, ", ");
                    //     }    
                    // }

                    //char *msg = mg_mprintf(var);
                    mg_http_reply(connection, 200, headers, "%M", returnAttr, &attrValue);
                    //free(msg);
                }
                else {
                    struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
                    mg_http_serve_dir(connection, hm, &opts);
                }
            }
        }

    virtual ~WebUtility() {}
};