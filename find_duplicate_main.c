#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "sha.h" // Include your SHA-256 header if it's separate

#define SHA256_BLOCK_SIZE 32
#define MAX_PATH_LENGTH 1024

typedef struct {
    unsigned char hash[SHA256_BLOCK_SIZE];
    char **file_paths;
    size_t file_count;
} HashEntry;

void add_to_hash_map(HashEntry *hash_map, size_t *hash_count, unsigned char *hash, const char *file_path) {
    for (size_t i = 0; i < *hash_count; i++) {
        if (memcmp(hash_map[i].hash, hash, SHA256_BLOCK_SIZE) == 0) {
            hash_map[i].file_paths = realloc(hash_map[i].file_paths, (hash_map[i].file_count + 1) * sizeof(char *));
            hash_map[i].file_paths[hash_map[i].file_count] = _strdup(file_path);
            hash_map[i].file_count++;
            return;
        }
    }

    hash_map[*hash_count].file_count = 1;
    hash_map[*hash_count].file_paths = malloc(sizeof(char *));
    hash_map[*hash_count].file_paths[0] = _strdup(file_path);
    memcpy(hash_map[*hash_count].hash, hash, SHA256_BLOCK_SIZE);
    (*hash_count)++;
}

char *get_duplicate_files_output(HashEntry *hash_map, size_t hash_count) {
    size_t buffer_size = 1024;
    char *output = malloc(buffer_size);
    if (!output) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    output[0] = '\0';

    for (size_t i = 0; i < hash_count; i++) {
        if (hash_map[i].file_count > 1) {
            char hash_string[SHA256_BLOCK_SIZE * 2 + 1];
            for (int j = 0; j < SHA256_BLOCK_SIZE; j++) {
                snprintf(hash_string + j * 2, 3, "%02x", hash_map[i].hash[j]);
            }

            size_t needed_size = strlen(output) + strlen(hash_string) + 128;
            for (size_t j = 0; j < hash_map[i].file_count; j++) {
                needed_size += strlen(hash_map[i].file_paths[j]) + 4;
            }

            if (needed_size > buffer_size) {
                buffer_size = needed_size * 2;
                char *new_output = realloc(output, buffer_size);
                if (!new_output) {
                    fprintf(stderr, "Memory allocation failed\n");
                    free(output);
                    return NULL;
                }
                output = new_output;
            }

            strcat(output, "Duplicate files for hash: ");
            strcat(output, hash_string);
            strcat(output, "\n");

            for (size_t j = 0; j < hash_map[i].file_count; j++) {
                strcat(output, "  ");
                strcat(output, hash_map[i].file_paths[j]);
                strcat(output, "\n");
            }
            strcat(output, "\n");
        }
    }

    return output;
}

char *print_duplicate_files(const char *dir_path, HashEntry *hash_map, size_t *hash_count) {
    WIN32_FIND_DATA findFileData;
    char searchPath[MAX_PATH_LENGTH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*", dir_path);

    HANDLE hFind = FindFirstFile(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "ERROR: Could not open directory %s (Error Code: %lu)\n", dir_path, GetLastError());
        return NULL;
    }

    do {
        if (findFileData.cFileName[0] == '.') {
            continue;
        }

        char fullPath[MAX_PATH_LENGTH];
        snprintf(fullPath, sizeof(fullPath), "%s\\%s", dir_path, findFileData.cFileName);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            print_duplicate_files(fullPath, hash_map, hash_count);
        } else {
            unsigned char hash[SHA256_BLOCK_SIZE];
            sha256(fullPath, hash); // Get the SHA-256 hash of the file
            add_to_hash_map(hash_map, hash_count, hash, fullPath);
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);

    char *duplicates_output = get_duplicate_files_output(hash_map, *hash_count);
    return duplicates_output;
}

void print_and_delete_duplicates(HashEntry *hash_map, size_t hash_count) {
    for (size_t i = 0; i < hash_count; i++) {
        if (hash_map[i].file_count > 1) {
            printf("Duplicate files for hash: ");
            for (int j = 0; j < SHA256_BLOCK_SIZE; j++) {
                printf("%02x", hash_map[i].hash[j]);
            }
            printf("\n");

            for (size_t j = 0; j < hash_map[i].file_count; j++) {
                printf("  %s\n", hash_map[i].file_paths[j]);
            }

            // Ask the user if they want to delete duplicates
            char choice[4];
            printf("Do you want to delete all duplicates except the first file? (yes/no): ");
            scanf("%3s", choice);

            if (strcmp(choice, "yes") == 0) {
                for (size_t j = 1; j < hash_map[i].file_count; j++) { // Start from the second file
                    if (DeleteFile(hash_map[i].file_paths[j])) {
                        printf("Deleted: %s\n", hash_map[i].file_paths[j]);
                    } else {
                        fprintf(stderr, "Failed to delete: %s (Error Code: %lu)\n", hash_map[i].file_paths[j], GetLastError());
                    }
                }
            } else {
                printf("No duplicates were deleted.\n");
            }
            printf("\n");
        }
    }
}

// int main(int argc, char **argv) {
//     HashEntry hash_map[1000];
//     size_t hash_count = 0;

//     const char *dir_path = ".";
//     char *output = print_duplicate_files(dir_path, hash_map, &hash_count);

//     if (output) {
//         printf("%s", output);
//         free(output);
//     }

//     for (size_t i = 0; i < hash_count; i++) {
//         for (size_t j = 0; j < hash_map[i].file_count; j++) {
//             free(hash_map[i].file_paths[j]);
//         }
//         free(hash_map[i].file_paths);
//     }

//     return 0;
// }
