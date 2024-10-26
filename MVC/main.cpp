#include "Model/WebUtility.cpp"
#include "Model/FileUtility.cpp"

bool isEnabledWebInterface = true;
bool isTriggerCheckDirectory = false;

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms

int main(){
    //fs::path dirPath = "C:/Users/vario/AppData/Local/Opera Software";
    FileHashAlgorithmProvider fhap;
    WebUtility wu;
    /*while (true){
        fhap.triggerAlgorithm();
    }*/
    // for (int i = 0; i <= 1; i++){
        
    //     //wu.hybridVectorDataArray() = fhap.hybridVectorDataArray();
    // }  
    fhap.triggerAlgorithm();

    while(true) { // General Operation Mode - GOM

        if(isEnabledWebInterface){
            struct mg_mgr mgr;  // Mongoose event manager. Holds all connections
            mg_log_set(MG_LL_DEBUG);
            mg_mgr_init(&mgr);  // Initialise event manager
            mg_http_listen(&mgr, "http://localhost:8000", wu.fn, nullptr);  // Setup listener
            for (;;) {
                mg_mgr_poll(&mgr, 1000);  // Infinite event loop
            }
        }
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
            DirectoryLocal directoryLocal("/home/vdrakonov/Downloads");

            if(directoryLocal.isFolderExist()){
                directoryLocal.setContext();
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