#include "Model/WebUtility.cpp"
#include "Model/FileUtility.cpp"

bool isEnabledWebInterface = true;
bool isTriggerCheckDirectory = false;

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms
#elif __linux__ //Linux platforms
#endif

int main(){
    
    //fs::path dirPath = "C:/Users/vario/AppData/Local/Opera Software";
    FileUtilityAlgorithmProvider fhap;
    WebUtility wu;
    FileUtilityProviderLocal fupl;
    
    // while (true){
        fhap.triggerAlgorithm("C:/Users/vario/AppData/Local/Opera Software", wu.getVectorFilePropertiesFileName(), wu.getVectorFilePropertiesFileSize(), wu.getVectorFilePropertiesFileType(), wu.getVectorFilePropertiesOwner(), wu.getVectorFilePropertiesDateTime(), wu.getVectorFilePropertiesHash());

    // } std::cout << "addr: " << &fupl.getFileList() << " size: " << fupl.getFileList().size() << std::endl;

    while(true) { // General Operation Mode - GOM

        if(isEnabledWebInterface){

            struct mg_mgr mgr;  // Mongoose event manager. Holds all connections
            mg_mgr_init(&mgr);  // Initialise event manager
            mg_http_listen(&mgr, "http://localhost:8000", wu.httpHandler, nullptr);  // Setup listener

            for (;;) {
                mg_mgr_poll(&mgr, 1000);  // Infinite event loop
            }
        }
    }

    return 0;
}