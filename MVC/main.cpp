#include "Model/FileUtility.h"

int main(){
    //fs::path dirPath = "C:/Users/vario/AppData/Local/Opera Software";


    LocalDirectory directoryPath("E:/TempFilesProgramm/OSSS/TempFolder/");

    if(directoryPath.exist()){
        directoryPath.getContents();
    }
    else{
        std::cout << "Path not found" << std::endl;
    }

    return 0;
}