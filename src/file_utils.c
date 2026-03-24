#include "common.h"
#include "tui.h"
#include "file_utils.h"

/* Check whether a file can be accessed or not*/
bool can_access(char* filename) {
    if (access(filename, F_OK) != 0) {
        int err = errno;
        sem_wait(&printing);
        wattron(log_win, COLOR_PAIR(CP_RED));
        wprintw(log_win, "Error accessing `%s`: %s\n", filename, strerror(err));
        wattroff(log_win, COLOR_PAIR(CP_RED));
        sem_post(&printing);
        wrefresh(log_win);
        return false;
    }
    return true;
}

/* A wrapper around fopen function to handle all file related errors at one place */
FILE* open_file(char* filename, char* mode) {
    // return NULL if caller wants to read, but can't access the file
    if (mode[0] == 'r' && !can_access(filename)) return NULL;

    FILE* fp = fopen(filename, mode);
    if (fp == NULL) {
        int err = errno;
        sem_wait(&printing);
        wattron(log_win, COLOR_PAIR(CP_RED));
        wprintw(log_win, "Error opening `%s`: %s\n", filename, strerror(err));
        wattroff(log_win, COLOR_PAIR(CP_RED));
        sem_post(&printing);
        wrefresh(log_win);
        return NULL;
    }

    return fp;
}

/* Merge(copy) different files into one single file with their size as prefix */
bool merge_files(int num_files, char* file_names[], char* out_file) {
    FILE* fp_write;
    FILE* fp_read;
    int n;
    uint32_t file_size;
    char buffer[BUF_SIZE];

    fp_write = open_file(out_file, "wb");
    if (fp_write == NULL) return false;
    
    for (int i = 0; i < num_files; ++i) {
        fp_read = open_file(file_names[i], "rb");
        if (fp_read == NULL) return false;

        // Write the file size into the out_file first
        fseek(fp_read, 0, SEEK_END);
        long raw_size = ftell(fp_read);
        fseek(fp_read, 0, SEEK_SET);
        file_size = htonl((uint32_t)raw_size);
        fwrite(&file_size, sizeof(uint32_t), 1, fp_write);
        
        // Write actual contents of the file into the file
        while((n = fread(buffer, 1, BUF_SIZE, fp_read)) > 0) {
            fwrite(buffer, 1, n, fp_write);
        }

        fclose(fp_read);
    }

    fclose(fp_write);
    return true;
}

/* Split a single file into different files based on size */
bool split_file(char* in_file, int num_files, char* out_files[]) {
    FILE* fp_read;
    FILE* fp_write;
    uint32_t file_size;         /* Number of bytes to be written in the file */
    uint32_t written_bytes;     /* Number of bytes written in the file */
    uint32_t to_write;          /* Number of bytes to write in the file at one time */
    int n;

    fp_read = open_file(in_file, "rb");
    if (fp_read == NULL) return false;
    
    for (int i = 0; i < num_files; ++i) {
        fp_write = open_file(out_files[i], "wb");
        if (fp_write == NULL) return false;

        // Read the file_size corresponding to the file contained in the section
        // uint32_t ntohl_size;    /* This is necessary for conversion */
        n = fread(&file_size, sizeof(uint32_t), 1, fp_read);
        if (n <= 0) fatal_error("[ERROR READING FILE LENGTH]");
        file_size = ntohl((uint32_t)file_size);
        written_bytes = 0;

        // Write to the file
        while (written_bytes < file_size) {
            to_write = (file_size - written_bytes) > BUF_SIZE ?
                        BUF_SIZE : (file_size - written_bytes);
            n = fread(file_input_buffer, 1, to_write, fp_read);
            if (n <= 0) fatal_error("[ERROR READING FILE CONTENT]");
            fwrite(file_input_buffer, 1, n, fp_write);
            written_bytes += n;
        }

        fclose(fp_write);
    }
    
    fclose(fp_read);
    return true;
}