#ifndef FILE_UTILS_H
#define FILE_UTILS_H

/* Merge different files into one single file with their size as prefix */
void merge_files(int num_files, char* file_names[], char* out_file);

/* Split a single file into different files based on size */
void split_file(char* in_file, int num_files, char* out_files[]);

#endif // !FILE_UTILS_H