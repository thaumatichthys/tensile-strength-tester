#include "FileStuff.h"

extern "C" {
    #include "ff.h"
    #include "f_util.h"
    #include "hw_config.h"
}

FATFS fs;
FRESULT fr;
size_t file_num = 0;
size_t MAX_FILES = 99999;
const size_t filename_buf_size = 32;
const char* file_extension = ".csv";
const char* file_name_body = "force-test-result";

char filename_buf[filename_buf_size];

bool recording = false;
bool error_state = false;
uint32_t record_start_time = 0;
bool IsError() {
    return error_state;
}
bool IsRecording() {
    return recording;
}

uint32_t GetStartTime() {
    return record_start_time;
}

void WriteTestFile() {
    fs;
    fr = f_mount(&fs, "", 1);
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

void ErrorHandler(FRESULT error, const char* msg) {
    recording = false;
    error_state = true;
    f_unmount("");

    // must do this or else sd_init_driver wont do anything
    sd_card_t* sd_p = sd_get_by_num(0);
    sd_p->state.m_Status |= STA_NOINIT;

    sd_init_driver();
    printf("error occurred, %d. msg: %s. filename: %s\n", fr, msg, filename_buf);
    //while(1) {
    //    
    //}
}

bool StartRecording() {
    error_state = false;
    fr = f_mount(&fs, "", 1);
    if (fr != FR_OK || recording) {
        ErrorHandler(fr, "mounting");
        return false;
    }

    for (file_num = 0; file_num < MAX_FILES; file_num++) {
        snprintf(filename_buf, filename_buf_size, "%s-%d%s", file_name_body, file_num, file_extension);
        FILINFO fno;
        fr = f_stat(filename_buf, &fno);
        if (fr == FR_NO_FILE)
            break;
        else if ((fr != FR_OK) && (fr != FR_NO_FILE)) {
            // error
            ErrorHandler(fr, "cycling thru names error");
        }
    }
    FIL fil;
    fr = f_open(&fil, filename_buf, FA_OPEN_APPEND | FA_WRITE);
    if ((fr != FR_NO_FILE) && (fr != FR_OK)) ErrorHandler(fr, "startrecording fopen error");
    f_printf(&fil, "load_cell_force_N,delta_position_mm,speed_degrees_per_sec,elapsed_time_ms,delta_steps\n");
    fr = f_close(&fil);
    if (fr != FR_OK) ErrorHandler(fr, "startrecording fclose error");
    recording = true;
    
    record_start_time = to_ms_since_boot(get_absolute_time());
    return true;
}

void UpdateRecording(CSVData data) {
    if (recording) {
        FIL fil;
        fr = f_open(&fil, filename_buf, FA_WRITE | FA_OPEN_APPEND);
        if (fr != FR_OK) ErrorHandler(fr, "updaterecording fopen error");
        float load_cell_force_N = data.load_cell_force_N_;
        float delta_position_mm = data.delta_position_mm_;
        float speed_degrees_sec = data.speed_degrees_sec_;
        int elapsed_time_ms = data.elapsed_time_ms_;
        int delta_steps = data.delta_steps_;
        
        f_printf(&fil, "%f,%f,%f,%d,%d\n", load_cell_force_N, delta_position_mm, speed_degrees_sec, elapsed_time_ms, delta_steps);

        fr = f_close(&fil);
        if (fr != FR_OK) ErrorHandler(fr, "updaterecording fclose error");
    }
}

void StopRecording() {
    f_unmount("");
    recording = false;
}