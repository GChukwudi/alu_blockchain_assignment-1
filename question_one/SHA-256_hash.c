/**
 * SHA-256 Hashing Algorithm
 * 
 * This program implements the SHA-256 hashing algorithm in C.
 * It includes a test case with the string "Blockchain Cryptography"
 * and allows the user to input their own string to compute its SHA-256 hash.

 * The program uses the OpenSSL library for SHA-256 hashing.
 * The hash is displayed in hexadecimal format.
 */

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>  // For SHA-256 functionality

// Function to print hash in hexadecimal format
void print_sha256_hash(unsigned char hash[]) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

int main() {
    // --------- Test Case: "Blockchain Cryptography" ---------
    const char *test_input = "Blockchain Cryptography";
    unsigned char test_hash[SHA256_DIGEST_LENGTH];

    // Hash the test input
    SHA256((const unsigned char *)test_input, strlen(test_input), test_hash);

    // Display the test result
    printf("Test Input: %s\n", test_input);
    printf("SHA-256 Hash: ");
    print_sha256_hash(test_hash);

    // --------- User Input Section ---------
    char input[1024];  // Buffer for user input
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // Prompt for user input
    printf("\nNow enter your own string to hash:\n");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        return 1;
    }

    // Remove newline character from input if present
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }

    // Compute SHA-256 hash of user input
    SHA256((const unsigned char *)input, strlen(input), hash);

    // Display results
    printf("Input: %s\n", input);
    printf("SHA-256 Hash: ");
    print_sha256_hash(hash);

    return 0;
}
