#include "Model/FileUtility.h"

int main(){
    //fs::path dirPath = "C:/Users/vario/AppData/Local/Opera Software";

    LocalDirectory directoryPath("C:/Users/vario/AppData/Local/Opera Software");

    if(directoryPath.exist()){
        directoryPath.getContents();
    }

    return 0;
}