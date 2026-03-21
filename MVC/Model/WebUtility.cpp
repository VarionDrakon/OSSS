#include "Headers/WebUtility.h"
#include <string>

WebUtility::WebUtility() {}

std::vector<FileMetadata> WebUtility::fileMetadata;

void WebUtility::fileMetadataSet(const std::vector<FileMetadata> &fm) {
    fileMetadata = fm;
}

void WebUtility::serializeFileListMetadata(void (*connection)(char, void*), char* ptr){
    try {
        const char* keys[] = { "File path", "File name", "File size", "File type", "Owner", "Date creation", "Hash" };

        char *constructor = "{\"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\", \"%s\": \"%s\"}";

        mg_xprintf(connection, ptr, "{\"%s\":[", keys[0]);
        for (int f = 0; f < fileMetadata.size(); f++) {
            const FileMetadata &meta = (fileMetadata)[f];
            mg_xprintf(connection, ptr, constructor,
                keys[0], meta.filePath.c_str(),
                keys[1], meta.fileName.c_str(),
                keys[2], meta.fileSize.c_str(),
                keys[3], meta.fileTypeData.c_str(),
                keys[4], meta.fileOwner.c_str(),
                keys[5], meta.fileDateTime.c_str(),
                keys[6], meta.fileHash.c_str()
            );

            mg_xprintf(connection, ptr, (f < fileMetadata.size() - 1) ? "," : "");
        }
        mg_xprintf(connection, ptr, "]}");
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}

void WebUtility::ev_handler(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if (mg_match(hm->uri, mg_str("/api/get"), NULL)) {
            size_t offset = 0;
            const size_t offset_size = 1024;

            while (offset < hm->body.len) {
                size_t offset_endpoint = offset_size;
                if ((offset + offset_endpoint) > hm->body.len) {
                    offset_endpoint = hm->body.len - offset;
                }
                printf("%.*s\n", (int)offset_endpoint, hm->body.buf + offset);
                offset += offset_endpoint;
            }
            mg_http_reply(c, 200, "", "OK!");
        }
        else if (mg_match(hm->uri, mg_str("/api/data"), NULL)) {
            mg_http_reply(c, 200, "", "%M", serializeFileListMetadata);
        }
        else {
            struct mg_http_serve_opts opts = {.root_dir = ".", .fs = &mg_fs_posix};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

void WebUtility::deserializationFileListMetadata(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_CONNECT) {
        mg_printf(c, "GET /api/data HTTP/1.1 \r\n" 
                      "Host: localhost       \r\n"
                      "Connection: close     \r\n"
                                            "\r\n"); // https://datatracker.ietf.org/doc/html/rfc9112
    }
    else if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;

        if (hm->body.len > 0) {
            std::cout << std::string(hm->body.buf, hm->body.len) << std::endl;
        } else {
            std::cout << "(empty body)" << std::endl;
        }
    }
}

void WebUtility::runLocalHTTPServer() {

    const int portLimitPoolLower = 1834;
    const int portLimitPoolUpper = 1846;
    int portLimitPoolCurrent = portLimitPoolLower;
    std::string listenUrl = "http://0.0.0.0:";   

    struct mg_mgr mgr;
    mg_mgr_init(&mgr);
    struct mg_connection *c = nullptr;

    for(portLimitPoolCurrent; portLimitPoolCurrent <= portLimitPoolUpper; portLimitPoolCurrent++) {

        std::cout << "[]> Trying use port: " << portLimitPoolCurrent << std::endl;

        std::string listenUrlTemp = listenUrl + std::to_string(portLimitPoolCurrent);
        c = mg_http_listen(&mgr, listenUrlTemp.c_str(), ev_handler, NULL);

        if (c != nullptr) {
            listenUrl += std::to_string(portLimitPoolCurrent);
            break;
        }
    }

    if (c == nullptr) {
        std::cout << "[]> Failed bind port. Exit from program..." << std::endl;
        exit(1);
    }

    std::cout << "[]> Succesfull bind port. Current listener url: " << listenUrl << std::endl;

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
        // DEBUG:
        // mg_http_connect(&mgr, "http://localhost:8000/api/data", deserializationFileListMetadata, NULL);
    }

    mg_mgr_free(&mgr);
}