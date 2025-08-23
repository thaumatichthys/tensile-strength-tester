#include "FileStuff.h"

extern "C" {
    #include "ff.h"
    #include "f_util.h"
    #include "hw_config.h"
}


void WriteTestFile() {
    FATFS fs;
    FRESULT fr = f_mount(&fs, "", 1);
    if (FR_OK != fr) {
        while(1) {
            printf("disk mount failed!, %d\n", fr);
        }
    }

    FIL fil;
    const char* filename = "test-file.txt";
    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    f_printf(&fil, "test file!!!!\n");

    fr = f_close(&fil);
    f_unmount("");
}