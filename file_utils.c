#include "common.h"
#include "file_utils.h"

/* Merge(copy) different files into one single file with their size as prefix */
void merge_files(int num_files, char* file_names[], char* out_file) {
    FILE* fp_write;
    FILE* fp_read;
    int n;
    uint32_t file_size;
    char buffer[BUF_SIZE];

    fp_write = fopen(out_file, "w");
    if (fp_write == NULL) fatal_error("[ERROR OPENING FILE]");
    
    for (int i = 0; i < num_files; ++i) {
        fp_read = fopen(file_names[i], "r");
        if (fp_read == NULL) fatal_error("[ERROR OPENING FILE]");

        // Write the file size into the out_file first
        fseek(fp_read, 0, SEEK_END);
        file_size = ftell(fp_read);
        fseek(fp_read, 0, SEEK_SET);
        fwrite(&file_size, 1, sizeof(file_size), fp_write);
        
        // Write actual contents of the file into the file
        while((n = fread(buffer, 1, BUF_SIZE, fp_read)) > 0) {
            fwrite(buffer, 1, BUF_SIZE, fp_write);
        }

        fclose(fp_read);
    }

    fclose(fp_write);
}

/* Split a single file into different files based on size */
void split_file(char* in_file, int num_files, char* out_files[]) {
    FILE* fp_read;
    FILE* fp_write;
    uint32_t file_size;         /* Number of bytes to be written in the file */
    uint32_t written_bytes;     /* Number of bytes written in the file */
    uint32_t to_write;          /* Number of bytes to write in the file at one time */
    int n;

    fp_read = fopen(in_file, "r");
    if (fp_read == NULL) fatal_error("[ERROR OPENING FILE]");
    
    for (int i = 0; i < num_files; ++i) {
        fp_write = fopen(out_files[i], "w");
        if (fp_write == NULL) fatal_error("[ERROR OPENING FILE]");

        // Read the file_size corresponding to the file contained in the section
        n = fread(&file_size, 1, sizeof(file_size), fp_read);
        if (n <= 0) fatal_error("[ERROR READING FILE LENGTH]");
        written_bytes = 0;

        // Write to the file
        while (written_bytes < file_size) {
            to_write = (file_size - written_bytes) > BUF_SIZE ?
                        BUF_SIZE : (file_size - written_bytes);
            n = fread(file_buf_in, 1, to_write, fp_write);
            if (n <= 0) fatal_error("[ERROR READING FILE CONTENT]");
            fwrite(file_buf_in, 1, n, fp_write);
        }

        fclose(fp_write);
    }
    
    fclose(fp_read);
}