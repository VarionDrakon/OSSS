#include "../../../lib/mongoose/mongoose.h"
#include <cstdarg>
#include <cstddef>
#include <cstdint>
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
    { NULL, NULL }
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

static std::vector<std::string> dataArray;
class WebUtility {
    protected:

        static size_t returnData(void (*out)(char, void *), void *ptr, va_list *ap){

            struct attribute *atr = filePropertiesAttributes;
            struct fileProperties *fP = (struct fileProperties *) ptr;
            size_t i, len = 0;

            len = mg_xprintf(out, ptr, "{");

            if(out){
                out('c', ptr);
            }
            else{
                return 0;
            }

            for (i = 0; filePropertiesAttributes[i].name != NULL; i++){
                
                char *attrptr = (char *) fP + atr[i].offset;
                
                std::cout << (char)attrptr << std::endl;

                len += mg_xprintf(out, ptr, "%s\"", atr[i].name); //name attribute
                len += mg_xprintf(out, ptr, ": ");

                if (strcmp(atr[i].type, "string") == 0){
                    len += mg_xprintf(out, ptr, "\"%s\"", *(const char **)attrptr);
                }  
                else if (strcmp(atr[i].type, "int") == 0){
                    len += mg_xprintf(out, ptr, "\"%d\"", *(size_t *)attrptr);
                }
                else {
                    len += mg_xprintf(out, ptr, "null");
                }

                if(atr[i + 1].name != NULL){
                    len += mg_xprintf(out, ptr, ", ");
                }
            }

            return len;
        }
        

    private: 

    public:
static void myOutputFunction(char c, void *data) {
    printf("Output: %c, Data: %s\n", c, (char *)data);
}
        static void fn(struct mg_connection *connection, int event, void *event_data) {
            if (event == MG_EV_HTTP_MSG) {
                struct mg_http_message *hm =(struct mg_http_message*) event_data;
                if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {

                    char json_buffer[512];
                    const char *headers = "content-type: application/json\r\n";

                    for (size_t i = 0; filePropertiesList[i].fileName != NULL; i++){
                        size_t len = returnData(myOutputFunction, &filePropertiesList[i], NULL);

                        mg_http_reply(connection, 200, headers, "%c", len, json_buffer);

                        if(filePropertiesList[i].fileName != NULL){
                            mg_http_reply(connection, 200, headers, ", ");
                        }

                        mg_http_reply(connection, 200, headers, "]");
                    }
                    
                }
                else {
                    struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
                    mg_http_serve_dir(connection, hm, &opts);
                }
            }
        }

    virtual ~WebUtility() {}
};