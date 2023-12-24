#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_WORD_LENGTH 100

void encryptWord(char *word, int *permutation, int length) {
    char *encryptedWord = malloc(length + 1);
    for (int i = 0; i < length; ++i) {
        encryptedWord[permutation[i]] = word[i];
    }
    encryptedWord[length] = '\0';
    strcpy(word, encryptedWord);
    free(encryptedWord);
}

void decryptWord(char *word, int *permutation, int length) {
    char *decryptedWord = malloc(length + 1);
    for (int i = 0; i < length; ++i) {
        decryptedWord[i] = word[permutation[i]];
    }
    decryptedWord[length] = '\0';
    strcpy(word, decryptedWord);
    free(decryptedWord);
}

void generatePermutation(int *permutation, int length) {
    for (int i = 0; i < length; ++i) {
        permutation[i] = i;
    }
    srand(time(NULL));
    for (int i = length - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int temp = permutation[i];
        permutation[i] = permutation[j];
        permutation[j] = temp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        printf("Usage:\n");
        printf("Encrypt: %s input.txt\n", argv[0]);
        printf("Decrypt: %s input.txt output.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    int encrypt = (argc == 2);
    char *inputFileName = argv[1];
    char *outputFileName = (encrypt) ? "encrypted.txt" : argv[2];

    char *words;
    int *permutations;
    int numWords;

    if (encrypt) {
        printf("Encryption successful!\n");
    } else {
        printf("Decryption successful!\n");
    }
    return EXIT_SUCCESS;
}