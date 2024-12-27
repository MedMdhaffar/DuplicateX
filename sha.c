#include "sha.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SHA256_BLOCK_SIZE 32  // SHA256 outputs a 256 bit digest

// Constants for SHA-256
static const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0x0857371d, 0x2c1a3e20, 0x5a01d04a, 0x6e8d7e28, 0x7f2b3f14, 0x8e23a00b, 0x93f249b2,
    0xa4c31c2f, 0xaff0a3f9, 0xc8a4fbc7, 0xd6d6d09f, 0x2179b5f6, 0x382d131a, 0x4e84f26f, 0x62a303c6,
    0x7661b150, 0x97e3f0c2, 0xa317d01f, 0xd82d0b9e, 0x3f98c0b9, 0x52b115f4, 0x83a2671f, 0x94fd9e45,
    0x184f9134, 0x3e378bbf, 0x5d5a5f3f, 0x6c7c1631, 0x9f6162e7, 0xae9f4874, 0x8ba56c76, 0x84a1102d
};

typedef struct {
    uint32_t state[8];
    uint64_t bit_count;
    uint8_t buffer[64];
} SHA256_CTX;

// SHA-256 initialization
void sha256_init(SHA256_CTX *ctx) {
    ctx->state[0] = 0x6a09e667;
    ctx->state[1] = 0xbb67ae85;
    ctx->state[2] = 0x3c6ef372;
    ctx->state[3] = 0xa54ff53a;
    ctx->state[4] = 0x510e527f;
    ctx->state[5] = 0x9b05688c;
    ctx->state[6] = 0x1f83d9ab;
    ctx->state[7] = 0x5be0cd19;
    ctx->bit_count = 0;
}

// SHA-256 transform function
void sha256_transform(SHA256_CTX *ctx, const uint8_t *data) {
    uint32_t a, b, c, d, e, f, g, h, temp1, temp2;
    uint32_t w[64];
    int i, t;

    // Prepare the message schedule
    for (i = 0; i < 16; i++) {
        w[i] = (data[i * 4] << 24) | (data[i * 4 + 1] << 16) |
               (data[i * 4 + 2] << 8) | (data[i * 4 + 3]);
    }
    for (i = 16; i < 64; i++) {
        w[i] = w[i - 16] + (w[i - 15] >> 7 | w[i - 15] << 25) + w[i - 7] + (w[i - 2] >> 17 | w[i - 2] << 15);
    }

    // Initialize working variables
    a = ctx->state[0];
    b = ctx->state[1];
    c = ctx->state[2];
    d = ctx->state[3];
    e = ctx->state[4];
    f = ctx->state[5];
    g = ctx->state[6];
    h = ctx->state[7];

    // Main loop
    for (i = 0; i < 64; i++) {
        temp1 = h + (e >> 6 | e << 26) + (e & f) + (k[i] + w[i]);
        temp2 = (a >> 2 | a << 30) + (a & b) + (c & d);
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;
    }

    // Update the hash value
    ctx->state[0] += a;
    ctx->state[1] += b;
    ctx->state[2] += c;
    ctx->state[3] += d;
    ctx->state[4] += e;
    ctx->state[5] += f;
    ctx->state[6] += g;
    ctx->state[7] += h;
}

// Finalize the SHA-256 hash
void sha256_final(SHA256_CTX *ctx, unsigned char *hash_output) {
    uint8_t data[64] = {0};
    uint64_t bit_count = ctx->bit_count;

    // Padding
    data[0] = 0x80;
    if (ctx->bit_count % 512 > 448) {
        sha256_transform(ctx, data);
        memset(data, 0, 64);
    }
    data[56] = (bit_count >> 56) & 0xff;
    data[57] = (bit_count >> 48) & 0xff;
    data[58] = (bit_count >> 40) & 0xff;
    data[59] = (bit_count >> 32) & 0xff;
    data[60] = (bit_count >> 24) & 0xff;
    data[61] = (bit_count >> 16) & 0xff;
    data[62] = (bit_count >> 8) & 0xff;
    data[63] = (bit_count) & 0xff;

    sha256_transform(ctx, data);

    // Output the hash
    for (int i = 0; i < 8; i++) {
        hash_output[i * 4] = (ctx->state[i] >> 24) & 0xff;
        hash_output[i * 4 + 1] = (ctx->state[i] >> 16) & 0xff;
        hash_output[i * 4 + 2] = (ctx->state[i] >> 8) & 0xff;
        hash_output[i * 4 + 3] = (ctx->state[i]) & 0xff;
    }
}

// SHA-256 function that reads a file and calculates its hash
void sha256(const char *file_path, unsigned char *hash_output) {
    SHA256_CTX ctx;
    sha256_init(&ctx);

    FILE *f = fopen(file_path, "rb");
    if (f == NULL) {
        perror("Could not open file");
        exit(1);
    }

    unsigned char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        ctx.bit_count += bytesRead * 8;
        sha256_transform(&ctx, buffer);
    }

    sha256_final(&ctx, hash_output);
    fclose(f);
}
