#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

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

void mapFileToMemory(const char *fileName, char **words, int **permutations, int *numWords, int encrypt) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    *numWords = 0;
    char line[MAX_WORD_LENGTH];
    if (fgets(line, sizeof(line), file) != NULL) {
        // Calculează numărul de cuvinte în linie
        char *token = strtok(line, " \t\n"); // consideră spațiile, tabulările și newline-uri ca delimitatori
        while (token != NULL) {
            (*numWords)++;
            token = strtok(NULL, " \t\n");
        }
    }
    rewind(file);

    // Map the file into memory
    *words = mmap(NULL, *numWords * MAX_WORD_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    *permutations = mmap(NULL, *numWords * MAX_WORD_LENGTH * sizeof(int), PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    // Read words into memory
    for (int i = 0; i < *numWords; ++i) {
        fscanf(file, "%s", (*words) + i * MAX_WORD_LENGTH);
    }

    // If decrypting, read permutations from the file
    if (!encrypt) {
        for (int i = 0; i < *numWords; ++i) {
            for (int j = 0; j < strlen((*words) + i * MAX_WORD_LENGTH); ++j) {
                fscanf(file, "%d", &((*permutations)[i * MAX_WORD_LENGTH + j]));
            }
        }
    }

    fclose(file);
}

void processWord(char *word, int *permutation, int encrypt) {
    int length = strlen(word);

    if (encrypt) {
        generatePermutation(permutation, length);
        encryptWord(word, permutation, length);
    } else {
        decryptWord(word, permutation, length);
    }
}

void createProcesses(char *words, int *permutations, int numWords, int encrypt) {
    pid_t pid;

    for (int i = 0; i < numWords; ++i) {
        pid = fork();

        if (pid == -1) {
            perror("Fork error");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Proces copil
            processWord(words + i * MAX_WORD_LENGTH, permutations + i * MAX_WORD_LENGTH, encrypt);
            exit(EXIT_SUCCESS);
        }
    }

    // Așteaptă ca toate procesele copil să se termine
    for (int i = 0; i < numWords; ++i) {
        wait(NULL);
    }
}

void writeOutputFile(const char *outputFileName, char *words, int *permutations, int numWords, int encrypt) {
    FILE *outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < numWords; ++i) {
        fprintf(outputFile, "%s ", words + i * MAX_WORD_LENGTH);
    }
    if (encrypt) {
        fprintf(outputFile, "\n");
        for (int i = 0; i < numWords; ++i) {
            for (int j = 0; j < strlen(words + i * MAX_WORD_LENGTH); ++j) {
                fprintf(outputFile, "%d ", permutations[i * MAX_WORD_LENGTH + j]);
            }
            fprintf(outputFile, "\n");
        }
    }

    fclose(outputFile);
}

void cleanupMemory(char *words, int *permutations, int numWords) {
    munmap(words, numWords * MAX_WORD_LENGTH);
    munmap(permutations, numWords * MAX_WORD_LENGTH * sizeof(int));
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

    mapFileToMemory(inputFileName, &words, &permutations, &numWords, encrypt);
    createProcesses(words, permutations, numWords, encrypt);
    writeOutputFile(outputFileName, words, permutations, numWords, encrypt);
    cleanupMemory(words, permutations, numWords);

    if (encrypt) {
        printf("Encryption successful!\n");
    } else {
        printf("Decryption successful!\n");
    }
    return EXIT_SUCCESS;
}