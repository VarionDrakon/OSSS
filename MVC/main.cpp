#include "Model/WebUtility.h"
#include "Model/FileUtility.h"

bool isEnabledWebInterface = false;
bool isTriggerCheckDirectory = false;

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms

#include "windows.h"

int main(){
    //fs::path dirPath = "C:/Users/vario/AppData/Local/Opera Software";

    LocalDirectory dx1irectoryPath("./TempFolder/");

    if(directoryPath.exist()){
        directoryPath.getContents();
    }
    else {
        std::cout << "Path not found" << std::endl;
    }

    return 0;
}
#elif __linux__ //Linux platforms

int main(){

    isTriggerCheckDirectory = true;
    isEnabledWebInterface = false;

    while(true) { // General Operation Mode - GOM

        if(isEnabledWebInterface){
            struct mg_mgr mgr;  // Mongoose event manager. Holds all connections
            mg_mgr_init(&mgr);  // Initialise event manager
            mg_http_listen(&mgr, "http://0.0.0.0:8000", fn, nullptr);  // Setup listener
            for (;;) {
                mg_mgr_poll(&mgr, 1000);  // Infinite event loop
            }
        }

        if(isTriggerCheckDirectory){
            LocalDirectory directoryPath("/home/vdrakonov/Downloads");

            if(directoryPath.exist()){
                directoryPath.getContents();
            }  
            else {
                std::cout << "Path not found" << std::endl;
            }
            isTriggerCheckDirectory = false;
        }
    }

    return 0;
}

#endif