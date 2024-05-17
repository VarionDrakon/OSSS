#include "Model/FileUtility.h"

int main(){
    //fs::path dirPath = "C:/Users/vario/AppData/Local/Opera Software";


    LocalDirectory directoryPath("C:/Users/vario/");

    if(directoryPath.exist()){
        directoryPath.getContents();
    }
    else{
        std::cout << "Path not found" << std::endl;
    }

    return 0;
}