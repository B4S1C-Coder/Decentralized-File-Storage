#include "dfs/filedecrypt.h"
#include <sodium.h>
#include <stdio.h>

int main() {
    if (sodium_init() < 0) {
        printf("Couldn't initialize sodium.\n");
        return -1;
    }

    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[crypto_secretbox_NONCEBYTES];

    // Loading the key
    FILE* keyFile = fopen("dfs_user_client/client_storage/chunks/key", "rb");

    if (!keyFile) {
        printf("Unable to open key file.\n");
        return -1;
    }

    fread(key, 1, crypto_secretbox_KEYBYTES, keyFile);
    fclose(keyFile);

    // Loading the nonce
    FILE* nonceFile = fopen("dfs_user_client/client_storage/chunks/nonce", "rb");

    if (!nonceFile) {
        printf("Unable to open nonce file.\n");
        return -1;
    }

    fread(nonce, 1, crypto_secretbox_NONCEBYTES, nonceFile);
    fclose(nonceFile);

    // Decrypt the chunks and reconstruct the file

    const char *chunksPaths[] = {
        "dfs_user_client/client_storage/chunks/text1.txt.0.dat",
        "dfs_user_client/client_storage/chunks/text1.txt.1.dat",
        "dfs_user_client/client_storage/chunks/text1.txt.2.dat",
        "dfs_user_client/client_storage/chunks/text1.txt.3.dat",
        "dfs_user_client/client_storage/chunks/text1.txt.4.dat"
    };

    const int decryptionResult = dfs_decryptAndReconstructFile_secretbox(
        "dfs_user_client/client_storage/decrypted.txt", 17, 5, chunksPaths, key, nonce);

    if (decryptionResult < 0) {
        printf("Decryption and reconstruction failed.\n");
        return -1;
    }

    printf("Test completed.\n");
    return 0;
}