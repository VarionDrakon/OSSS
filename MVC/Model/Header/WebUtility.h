#include "../../../lib/mongoose/mongoose.h"
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
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
    { NULL, 0 }
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


struct attr { 
    int i, s, f; 
    const char *c; 
};

struct attr attr_data[] = {
    { 123, 234, 423, "name"},
    { 456, 234, 456, "surname"},
    { 0, 0, 0, NULL}
};

static std::vector<std::string> dataArray;
class WebUtility {
    protected:

        static size_t returnData(void (*out)(char, void *), void *ptr, va_list *ap){

            struct attribute *attr = filePropertiesAttributes;
            struct fileProperties *fP = (struct fileProperties *) ptr;
            
            size_t i, len = 0;

            len += mg_xprintf(out, ptr, "{");

            for (i = 0; filePropertiesAttributes[i].name != NULL; i++){
                
                char *attrptr = (char *) fP + attr[i].offset;

                len += mg_xprintf(out, ptr, "\"%s\"", attr[i].name);
                len += mg_xprintf(out, ptr, ": ");

                if (strcmp(attr[i].type, "string") == 0){
                    std::cout << "Log 1: " << *(const char *)attrptr << std::endl;
                    len += mg_xprintf(out, ptr, "\"%s\"", *(const char *)attrptr); 
                }  
                else if (strcmp(attr[i].type, "int") == 0){
                    std::cout << "Log 2: " <<  *(size_t *)attrptr << std::endl;
                    len += mg_xprintf(out, ptr, "\"%d\"", *(size_t *)attrptr);
                }
                else {
                    len += mg_xprintf(out, ptr, "null");
                }

                if(attr[i + 1].name != NULL){
                    len += mg_xprintf(out, ptr, ", ");
                }
            }

            return mg_xprintf(out, attr, "{%m:%d}", MG_ESC("a"), attr->name);
            // struct a { int a, b; };
            // size_t print_a(void (*out)(char, void *), void *ptr, va_list *ap) {
            //  struct a *ptr = va_arg(*ap, struct a *);
            //  return mg_xprintf(out, ptr, "{%m:%d}", MG_ESC("a"), ptr->a); // MG_ESC invokes mg_print_esc
            // }

            // struct a temp = { 42, 43 };
            // mg_snprintf(buf, sizeof(buf), "%M", print_a, &temp);    // {"a":42}
        }

            // size_t mg_print_esc(void (*out)(char, void *), void *arg, va_list *ap) {
            // size_t len = (size_t) va_arg(*ap, int);
            // char *p = va_arg(*ap, char *);
            // if (len == 0) len = p == NULL ? 0 : strlen(p);
            // return qcpy(out, arg, p, len);
            // }


            static size_t returnAttr(void (*out)(char, void *), void *ptr, va_list *ap) {
                struct attr *atr = va_arg(*ap, struct attr *);

                size_t len = 0;

                for (int i = 0; atr[i].c != NULL; i++){
                    len += mg_xprintf(out, ptr, {"%d:%d:%d:%s \n"}, atr[i].i, atr[i].s, atr[i].f, atr[i].c);
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

                    
                    // for (size_t i = 0; filePropertiesList[i].fileName != NULL; i++){
                    //     size_t len = returnData(outputFunction, &filePropertiesList[i], NULL);

                    //     if(filePropertiesList[i].fileName != NULL){
                    //         mg_http_reply(connection, 200, headers, ", ");
                    //     }    
                    // }

                    //char *msg = mg_mprintf(var);
                    struct attr temp[] = {
                        { 42, 203, 345, "Val" },
                        { 234, 987, 456, "Value data" },
                        {0, 0, 0, NULL}
                    };
                    mg_http_reply(connection, 200, headers, "%M", returnAttr, &temp);
                    //free(msg);
                }
                else {
                    struct mg_http_serve_opts opts = {.root_dir = "Web.UI/"};
                    mg_http_serve_dir(connection, hm, &opts);
                }
            }
        }

    virtual ~WebUtility() {}
};