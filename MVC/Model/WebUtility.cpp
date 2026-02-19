#include "Headers/WebUtility.h"
#include <string>

WebUtility::WebUtility() {}

std::vector<FileMetadata> WebUtility::fileMetadata;

void WebUtility::fileMetadataSet(const std::vector<FileMetadata> &fm) {
    fileMetadata = fm;
}

void WebUtility::returnApiListFiles(void (*connection)(char, void*), char* ptr){
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
            mg_http_reply(c, 200, "", "%M", returnApiListFiles);
        }
        else {
            struct mg_http_serve_opts opts = {.root_dir = ".", .fs = &mg_fs_posix};
            mg_http_serve_dir(c, hm, &opts);
        }
    }
}

void WebUtility::HTTPServer() {
    struct mg_mgr mgr;
    mg_mgr_init(&mgr);

    mg_http_listen(&mgr, "http://localhost:8000", ev_handler, NULL);

    for (;;) {
        mg_mgr_poll(&mgr, 1000);
    }
    
    mg_mgr_free(&mgr);
}