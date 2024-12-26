#include "Header/WebUtility.h"

WebUtility::WebUtility() {}

size_t WebUtility::returnAttr(void (*out)(char, void *), void *ptr, va_list *ap) {

    size_t len = 0;
    struct attr *val = va_arg(*ap, struct attr *);
    struct attr *key = attrKey;

    char *constructor = "\"%s\": \"%s\",";
    char *endConstructor = "\"%s\": \"%s\"";

    len += mg_xprintf(out, ptr, "{");
    len += mg_xprintf(out, ptr, "\"Files\": [");

    const char *keys[] = { key->fileName, key->fileSize, key->typeData, key->owner, key->dateTime, key->hash };

    for (int i = 0; val[i].fileName != NULL; i++) {

        const char *varFields[] = { val[i].fileName, val[i].fileSize, val[i].typeData, val[i].owner, val[i].dateTime, val[i].hash };

        len += mg_xprintf(out, ptr, "{");

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
            len += mg_xprintf(out, ptr, "},");
        } else {
            len += mg_xprintf(out, ptr, "}");
        }
    }
    return len += mg_xprintf(out, ptr, "]}");
}

void WebUtility::httpHandler(struct mg_connection *connection, int event, void *event_data) {

    const char *headers = "content-type: text/json\r\n";

    if (event == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm =(struct mg_http_message*) event_data;

        if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {
            mg_http_reply(connection, 200, headers, "%M", returnAttr, &attrValue);
        }
        else {
            struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
            mg_http_serve_dir(connection, hm, &opts);
        }
    }
}