#include "../../mongoose.h"
#include <cstdarg>
#include <cstddef>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

class WebUtility {

  static size_t printRecursiveData(mg_pfn_t out, void *ptr, va_list *valst){
    
    std::vector<std::string> ded {"C:/Users/vario/Documents/Paradox Interactive/Hearts of Iron IV/mod/Millennium_Dawn__A_Modern_Day_Mod.mod", "awergagwaergaewrgaergergre", "awergagwaergaewrgaergergre", "awergagwaergaewrgaergergre", "awergagwaergaewrgaergergre", "awergagwaergaewrgaergergre",};

    unsigned start = va_arg(*valst, unsigned);
    unsigned max = start + 100;
    if (max > 10000) max = 10000;
    size_t n = 0;
    const char *comma = "";
    while(start < max){

      for(auto d : ded){
        n += mg_xprintf(out, ptr, d.c_str(), comma);
        comma = ",";
        start++;
      }
    } 
    return n;
  }

  static long getparam(struct mg_http_message *hm, const char *json_path) {
    double dv = 0;
    mg_json_get_num(hm->body, json_path, &dv);
    return dv;
  }

  // Connection event handler function
  static void fn(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_HTTP_MSG) {  // New HTTP request received
      struct mg_http_message *hm = (struct mg_http_message *) ev_data;  // Parsed HTTP request
      long start = getparam(hm, "$.start");
      if (mg_match(hm->uri, mg_str("/api/hello"), nullptr)) { // REST API call? 
            // Yes. Respond JSON
            mg_http_reply(c, 200, "", "{%m:%d}\n", printRecursiveData, start);
      } else {
        struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};  // For all other URLs,
        //mg_http_serve_file();
        mg_http_serve_dir(c, hm, &opts); // Serve static files
      }
    }
  }
};