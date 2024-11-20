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
    std::string fileName;
    int fileSize;
};


static std::vector<std::string> dataArray;
class WebUtility {
    protected:



    private: 
        static size_t printSctructure(void (*out) (char, void *), void *ptr, va_list *ap){
            std::vector<fileProperties> files = {
                {"fileName1.txt", 1234},
                {"fileName2.txt", 5678},
                {"fileName3.txt", 91011},
                {"fileName4.txt", 121314}
            };

            for (int i = 0; i < files.size(); i++){
                char file_entry[256];
                mg_snprintf(file_entry, sizeof(files), "{\"file name\" : \"%s\", \"file size\" : \"%d\"}", files[i].fileName, files[i].fileSize);
            }
        }


    public:

    static void fn(struct mg_connection *connection, int event, void *event_data) {
        if (event == MG_EV_HTTP_MSG) {
            struct mg_http_message *hm =(struct mg_http_message*) event_data;
            if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {
                
                const char *headers = "content-type: application/json\r\n";

                //char *msg = mg_mprintf("Build the message: %s %s", fP.filePath.c_str(), "ded");

                mg_http_reply(connection, 200, headers, "{%m:\"%s\"}\n", MG_ESC("data"), );
            }
            else {
                struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
                mg_http_serve_dir(connection, hm, &opts);
            }
        }
    }
    virtual ~WebUtility() {}
};