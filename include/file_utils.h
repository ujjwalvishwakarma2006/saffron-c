#ifndef FILE_UTILS_H
#define FILE_UTILS_H

/* Check whether a file can be accessed or not*/
bool can_access(char* filename);

/* A wrapper around fopen function to handle all file related errors at one place */
FILE* open_file(char* filename, char* mode);

/* Merge different files into one single file with their size as prefix */
bool merge_files(int num_files, char* file_names[], char* out_file);

/* Split a single file into different files based on size */
bool split_file(char* in_file, int num_files, char* out_files[]);

#endif // !FILE_UTILS_H