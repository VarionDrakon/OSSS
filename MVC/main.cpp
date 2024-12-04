#include "Model/WebUtility.cpp"
#include "Model/FileUtility.cpp"

bool isEnabledWebInterface = true;
bool isTriggerCheckDirectory = false;

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms
#elif __linux__ //Linux platforms
#endif

int main(){
    //fs::path dirPath = "C:/Users/vario/AppData/Local/Opera Software";
    FileHashAlgorithmProvider fhap;
    WebUtilityObject wuo;
    /*while (true){
        fhap.triggerAlgorithm();
    }*/
    // for (int i = 0; i <= 1; i++){
        
    //     //wu.hybridVectorDataArray() = fhap.hybridVectorDataArray();
    // }  
    // fhap.triggerAlgorithm();

    while(true) { // General Operation Mode - GOM

        if(isEnabledWebInterface){

            struct mg_mgr mgr;  // Mongoose event manager. Holds all connections
            mg_mgr_init(&mgr);  // Initialise event manager
            mg_http_listen(&mgr, "http://localhost:8000", wuo.httpHandler, nullptr);  // Setup listener

            for (;;) {
                mg_mgr_poll(&mgr, 1000);  // Infinite event loop
            }
        }
    }

    return 0;
}