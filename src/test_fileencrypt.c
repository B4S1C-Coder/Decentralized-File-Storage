#include "dfs/fileencrypt.h"
#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {

    if (argc < 4) {
        printf("Incomplete arguments\n");
        return -1;
    }

    if (sodium_init() < 0) {
        printf("Couldn't initialize sodium.\n");
        return -1;
    }

    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[crypto_secretbox_NONCEBYTES];

    const char ownershipToken[] = "<OWNERSHIP-TOKEN>";

    crypto_secretbox_keygen(key);
    randombytes_buf(nonce, sizeof nonce);

    // Encrypt the file and split it into chunks
    int fileWasSplit = dfs_divideFileIntoChunks_encryptedSecretbox(
        argv[1], argv[2], 5, ownershipToken, strlen(ownershipToken), argv[3], key, nonce
    );

    if (fileWasSplit < 0) {
        printf("[ FAIL ] Test failed. Exit code = %d\n", fileWasSplit);
        return 1;
    }

    // If the operation went correctly only then it is feasible to store the key and nonce
    // /output/dir/path/key
    char* keyFilePath = (char*)malloc(strlen(argv[3]) + 4);
    strcpy(keyFilePath, argv[3]);
    strcat(keyFilePath, "key");

    FILE* keyFile = fopen(keyFilePath, "wb");
    fwrite(key, 1, crypto_secretbox_KEYBYTES, keyFile);
    fclose(keyFile);

    char* nonceFilePath = (char*)malloc(strlen(argv[3]) + 6);
    strcpy(nonceFilePath, argv[3]);
    strcat(nonceFilePath, "nonce");

    FILE* nonceFile = fopen(nonceFilePath, "wb");
    fwrite(nonce, 1, crypto_secretbox_NONCEBYTES, nonceFile);
    fclose(nonceFile);

    printf("[ PASS ] Test successful. Exit code = %d\n", fileWasSplit);
    return 0;
}