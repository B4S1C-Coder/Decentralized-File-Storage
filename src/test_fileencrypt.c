#include "dfs/fileencrypt.h"
#include <string.h>
#include <sodium.h>

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
        argv[1], argv[2], 5, ownershipToken, sizeof(ownershipToken), argv[3], key, nonce
    );

    if (fileWasSplit < 0) {
        printf("[ FAIL ] Test failed. Exit code = %d\n", fileWasSplit);
        return 1;
    }

    printf("[ PASS ] Test successful. Exit code = %d\n", fileWasSplit);
    return 0;
}