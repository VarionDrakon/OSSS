#include "Header/WebUtility.h"

class WebUtilityObject {
    private:
        static WebProviderObject wpo;

    public:
        static void httpHandler(struct mg_connection *connection, int event, void *event_data) {
            const char *headers = "content-type: text/json\r\n";

            if (event == MG_EV_HTTP_MSG) {
                struct mg_http_message *hm =(struct mg_http_message*) event_data;

                if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {
                    mg_http_reply(connection, 200, headers, "%M", wpo.returnAttr, &attrValue);
                }
                else {
                    struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
                    mg_http_serve_dir(connection, hm, &opts);
                }
            }
        }
};