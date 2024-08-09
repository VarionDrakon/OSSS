#if defined(WIN32) || defined (_WIN32) || defined(__WIN32__) || defined(__NT__) //NT platforms

#include "Model/FileUtility.h"
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

#include "Model/FileUtility.h"

int main(){

    LocalDirectory directoryPath("/home/vdrakonov/Downloads");

    if(directoryPath.exist()){
        directoryPath.getContents();
    }
    else {
        std::cout << "Path not found" << std::endl;
    }

    return 0;
}

#endif