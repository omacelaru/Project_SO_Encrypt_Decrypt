# Encryptor/Decryptor Program

## Overview

This C program provides a simple text file encryption and decryption utility using a permutation-based algorithm. It
reads words from an input file, applies a random permutation to each word for encryption or uses a provided permutation
for decryption, and then writes the results to an output file. The program utilizes multiprocessing to process each word
concurrently, enhancing performance.

## Compilation

To compile the program, use the following command:

```bash
gcc -o encryptor_decryptor main.c
```

## Usage

#### Encryption

To encrypt a file, use the following command:

```bash
./encryptor_decryptor <input_file>
```

```bash
./encryptor_decryptor ToBeEncrypted.txt
```

The encrypted file will be written [encrypted.txt](encrypted.txt) in to the current directory.

#### Decryption

To decrypt a file, use the following command:

```bash
./encryptor_decryptor <input_file> <output_file>
```

```bash
./encryptor_decryptor encrypted.txt decrypted.txt
```

Note that the output file must be provided and already created. The decrypted file will be written to the specified
output file in the current directory.

## Input File Format

The input file must be a text file containing words separated by spaces only in the first line.\
The program will read the first line of the input file and encrypt/decrypt the words in the line.

#### Encryption

The program will ignore any other lines in the input file.

#### Decryption

The input file must also contain a list that contains the permutations used to encrypt each words in the first line.\
Each permutation must be a sequence of numbers separated by spaces. The permutation must be the same length as the
number of character in each word of the first line of the input file.