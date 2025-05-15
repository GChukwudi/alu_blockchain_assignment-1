/**
 * This program implements a simple blockchain structure in C.
 * 
 * It defines a Block structure that contains the following fields:
 * - index: The index of the block in the blockchain.
 * - timestamp: The time when the block was created.
 * - data: The data stored in the block.
 * - previousHash: The hash of the previous block in the blockchain.
 * - hash: The hash of the current block.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_DATA_SIZE 256
#define HASH_SIZE 65

// Define the structure for a Block
typedef struct {
    int index;
    char timestamp[100];
    char data[MAX_DATA_SIZE];
    char previousHash[HASH_SIZE];
    char hash[HASH_SIZE];
} Block;

// Helper function to get the current timestamp
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

// Function to calculate the SHA-256 hash of a block
void calculate_hash(Block *block, char *outputHash) {
    char input[1024];
    snprintf(input, sizeof(input), "%d%s%s%s",
             block->index, block->timestamp, block->data, block->previousHash);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(outputHash + (i * 2), "%02x", hash[i]);
    }
    outputHash[HASH_SIZE - 1] = '\0';
}

// Function to display block information
void print_block(Block *block) {
    printf("\nBlock Information:\n");
    printf("=============================\n");
    printf("Index         : %d\n", block->index);
    printf("Timestamp     : %s\n", block->timestamp);
    printf("Data          : %s\n", block->data);
    printf("Previous Hash : %s\n", block->previousHash);
    printf("Hash          : %s\n", block->hash);
    printf("=============================\n");
}

int main() {
    Block block;

    // Initialize genesis block
    block.index = 0;
    get_timestamp(block.timestamp, sizeof(block.timestamp));

    printf("Enter data for this genesis block: ");
    fgets(block.data, MAX_DATA_SIZE, stdin);
    block.data[strcspn(block.data, "\n")] = 0;  // Remove newline

    strncpy(block.previousHash, "0", HASH_SIZE - 1);
    block.previousHash[HASH_SIZE - 1] = '\0';

    // Compute hash
    calculate_hash(&block, block.hash);

    // Output
    printf("\nGenesis block created!\n");
    print_block(&block);

    return 0;
}
