#include "Model/Headers/WebUtility.h"
#include "Model/Headers/FileUtility.h"

bool isEnabledWebInterface = true;
bool isTriggerCheckDirectory = false;

#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms
#elif __linux__ //Linux platforms
#endif

// Object + Action + Modifier

int main(){

    FileUtilityProviderLocal fupl;
    // FileImage fi;
    WebUtility wu;

    fupl.fileMetadataCollectRecursively("/mnt/sda");
    // fi.imageCollect("/mnt/sda/utils/", "backFile.dat");
    // fi.imageDisperse("/mnt/sda/test-restore/", "backFile.dat");
    wu.fileMetadataSet(fupl.fileMetadataGet());

    wu.HTTPServer();

    return 0;
}