#include "../../../lib/mongoose/mongoose.h"
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

struct fileProperties {
    const char *fileName;
    int fileSize;
    const struct attribute *filePropertiesList;
};

struct fileProperties fileLists[] = {
    {"fileName1.txt", 91011},
    {"fileName2.txt", 67898},
    {"fileName3.txt", 23452},
    {"fileName4.txt", 23452},
    {NULL, NULL}
};



static std::vector<std::string> dataArray;
class WebUtility {
    protected:

        static size_t returnData(void (*out)(char, void *), void *ptr, va_list *ap){

            struct fileProperties *fP = va_arg(*ap, struct fileProperties *);
            size_t i, len = 0;

            for(i = 0; fP->fileName != NULL; i++){
                len += mg_xprintf(out, ptr, "%s%m:", i == 0 ? "" : ",", MG_ESC(fP->fileName[i]));
            }
            return len;
        }

    private: 

    public:

        static void fn(struct mg_connection *connection, int event, void *event_data) {
            if (event == MG_EV_HTTP_MSG) {
                struct mg_http_message *hm =(struct mg_http_message*) event_data;
                if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {
                    
                    const char *headers = "content-type: application/json\r\n";

                    //char *msg = mg_mprintf("Build the message: %s %s", fP.filePath.c_str(), "ded");

                    mg_http_reply(connection, 200, headers, "{\"%M\"}\n", returnData);
                }
                else {
                    struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
                    mg_http_serve_dir(connection, hm, &opts);
                }
            }
        }

    virtual ~WebUtility() {}
};