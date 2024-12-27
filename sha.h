#ifndef SHA_H
#define SHA_H

#include <stdio.h>
#include <stdint.h>

void sha256(const char *file_path, unsigned char *hash_output);

#endif // SHA_H
