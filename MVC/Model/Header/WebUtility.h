#include "../../mongoose.h"
#include <cstdarg>
#include <cstddef>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

class WebUtility {

    static long getparam(struct mg_http_message *hm, const char *json_path) {
        double dv = 0;
        mg_json_get_num(hm->body, json_path, &dv);
        return dv;
    }

    static size_t printdata(mg_pfn_t out, void *ptr, va_list *ap) {

        std::vector<std::string> data_array {"C:/Mod.mod", "C:/ddd.mod"};

        unsigned start = 0;
        unsigned chunk_size = 1;  // Max number returned in one API call
        unsigned max = start + chunk_size;
        const char *comma = "";
        size_t n = 0;
        if (max > data_array.size()) max = static_cast<unsigned>(data_array.size()); // Total number of elements
        std::string dd = "";

        for(auto d : data_array){
            dd += d;
        }

        while (start < max){
            n += mg_xprintf(out, ptr, dd.c_str());
            dd.clear();
            start++;           
        }
        return n;
    }
  
    public:
        static void fn(struct mg_connection *c, int ev, void *ev_data) {
        if (ev == MG_EV_HTTP_MSG) {
            struct mg_http_message *hm =(struct mg_http_message*) ev_data;
            if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {
                const char *headers = "content-type: text/json\r\n";
                long start = getparam(hm, "$.start");
                MG_DEBUG(("%.*s", (int) hm->body.len, hm->body.buf));
                // Return data, up to CHUNK_SIZE elements
                mg_http_reply(c, 200, headers, "{%m:%m}",
                                MG_ESC("data"), printdata);
            }
            else {
                struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
                mg_http_serve_dir(c, hm, &opts);
            }
        }
    }

    virtual ~WebUtility() {}
};