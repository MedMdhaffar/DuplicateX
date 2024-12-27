#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>
#include <sys/stat.h>  // Required for stat()
#define PATH_SEP "/"
#define PATH_SEP_LEN (sizeof(PATH_SEP) - 1)
char *join_path(const char *base, const char *file) {
    size_t base_len = strlen(base);  // Size of the base directory path
    size_t file_len = strlen(file);  // Size of the file or directory name
    // Handle case where base directory path doesn't end with a separator
    size_t total_len = base_len + file_len + PATH_SEP_LEN + 1;
    if (base[base_len - 1] == PATH_SEP[0]) {
        total_len -= PATH_SEP_LEN;  // No need to add separator if base already ends with it
    }

    // Allocate memory for the full path (base + separator + file)
    char *begin = malloc(total_len);
    assert(begin != NULL);  // Check if memory allocation was successful

    char *end = begin;
    memcpy(end, base, base_len);  // Copy the base directory path
    end += base_len;

    // Add the separator only if the base path doesn't already end with it
    if (base[base_len - 1] != PATH_SEP[0]) {
        memcpy(end, PATH_SEP, PATH_SEP_LEN);  // Copy the directory separator
        end += PATH_SEP_LEN;
    }

    memcpy(end, file, file_len);  // Copy the file or directory name
    end += file_len;
    *end = '\0';  // Null-terminate the full path string

    return begin;  // Return the full path string
}
// typedef void (File_Action)(const char *fiel_path);
char* print_existing_files(const char *dir_path)
{
    
    DIR *dir = opendir(dir_path);  // Open the directory
    if (dir == NULL) {  // If the directory cannot be opened
        fprintf(stderr, "error: could not open directory %s: %s\n", dir_path, strerror(errno));
        return NULL;  // Exit the function gracefully
    }

    errno = 0;  // Reset errno
    struct dirent *ent;
    struct stat statbuf;  // Buffer for file status
    char *result = malloc(1);  
    result[0] = '\0';  

    while ((ent = readdir(dir)) != NULL) {  // Read each directory entry
        // Skip hidden files (those starting with '.')
        if (ent->d_name[0] != '.') {
            // Join the base directory path and the entry name to form the child path
                char *child_path = join_path(dir_path, ent->d_name);

            // Use stat() to get information about the file or directory
            if (stat(child_path, &statbuf) == 0) {
                char buffer[1024];
                if (S_ISDIR(statbuf.st_mode)) {  // If it's a directory
                    snprintf(buffer, sizeof(buffer), "dir : %s\n", child_path);
                    result = realloc(result, strlen(result) + strlen(buffer) + 1);
                    strcat(result, buffer);
                    result = realloc(result, strlen(result) + 1); // Ensure null-termination
                    print_existing_files(child_path); // Recursively print contents of subdirectory
                } else if (S_ISREG(statbuf.st_mode)) {  // If it's a regular file
                    snprintf(buffer, sizeof(buffer), "file : %s\n", child_path);
                    result = realloc(result, strlen(result) + strlen(buffer) + 1);
                    strcat(result, buffer);
                    // file_action(join)
                }
            } else {
                // Handle error with stat() (e.g., file doesn't exist)
                fprintf(stderr, "error: could not stat file %s: %s\n", child_path, strerror(errno));
            }
            free(child_path);  // Free the memory allocated for the child path
        }
    }

    if (errno != 0) {  // If readdir() encountered an error
        fprintf(stderr, "error: could not read directory %s: %s\n", dir_path, strerror(errno));
    }

    closedir(dir);  // Close the directory
    return result;
}