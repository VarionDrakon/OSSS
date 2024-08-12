#include "mongoose.h"

// Connection event handler function
static void fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_HTTP_MSG) {  // New HTTP request received
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;  // Parsed HTTP request
    if (mg_match(hm->uri, mg_str("/api/hello"), nullptr)) {              // REST API call?
          // Yes. Respond JSON
          mg_http_reply(c, 200, "", "{%m:%d}\n", MG_ESC("status"), 1);
    } else {
      struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};  // For all other URLs,
      //mg_http_serve_file();
      mg_http_serve_dir(c, hm, &opts); // Serve static files
    } 
  }
}