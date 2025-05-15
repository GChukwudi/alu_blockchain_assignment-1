#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_BLOCKS 10
#define MAX_DATA_SIZE 256
#define HASH_SIZE 65  // 64 hex digits + null terminator

// ----------- Block Structure -----------
typedef struct Block {
    int index;
    char timestamp[64];
    char data[MAX_DATA_SIZE];
    char previousHash[HASH_SIZE];
    char hash[HASH_SIZE];
} Block;

// ----------- SHA-256 Hashing -----------
void compute_sha256(const char *input, char output[HASH_SIZE]) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)input, strlen(input), hash);
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    output[HASH_SIZE - 1] = '\0';
}

// ----------- Timestamp Helper -----------
void get_timestamp(char *buffer, size_t size) {
    time_t now = time(NULL);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", localtime(&now));
}

// ----------- Calculate Hash for Block -----------
void calculate_block_hash(Block *block, char output[HASH_SIZE]) {
    char temp[1024];
    snprintf(temp, sizeof(temp), "%d%s%s%s",
             block->index, block->timestamp, block->data, block->previousHash);
    compute_sha256(temp, output);
}

// ----------- Validate Entire Blockchain -----------
int is_chain_valid(Block chain[], int size) {
    for (int i = 1; i < size; i++) {
        char expected_hash[HASH_SIZE];
        calculate_block_hash(&chain[i - 1], expected_hash);
        if (strcmp(chain[i].previousHash, expected_hash) != 0) {
            return 0;
        }
    }
    return 1;
}

// ----------- Create a New Block -----------
Block create_block(int index, const char *data, const char *prev_hash) {
    Block block;
    block.index = index;

    strncpy(block.data, data, MAX_DATA_SIZE - 1);
    block.data[MAX_DATA_SIZE - 1] = '\0'; // Ensure null termination

    get_timestamp(block.timestamp, sizeof(block.timestamp));

    strncpy(block.previousHash, prev_hash, HASH_SIZE - 1);
    block.previousHash[HASH_SIZE - 1] = '\0'; // Ensure null termination
    
    calculate_block_hash(&block, block.hash);
    return block;
}

// ----------- Main Simulation -----------
int main() {
    Block blockchain[MAX_BLOCKS];
    int numBlocks = 0;

    // Create Genesis Block
    blockchain[0] = create_block(0, "Genesis Block", "0");
    numBlocks++;

    // Let the user add 3 new blocks
    for (int i = 1; i <= 3; i++) {
        printf("\nEnter data for Block %d: ", i);
        char inputData[MAX_DATA_SIZE];
        fgets(inputData, sizeof(inputData), stdin);
        inputData[strcspn(inputData, "\n")] = '\0'; // remove newline

        // Validate chain before adding
        if (is_chain_valid(blockchain, numBlocks)) {
            printf("✅ Chain valid. Adding Block %d...\n", i);
            blockchain[numBlocks] = create_block(numBlocks, inputData, blockchain[numBlocks - 1].hash);
            numBlocks++;
        } else {
            printf("❌ Blockchain is invalid. Cannot add Block %d.\n", i);
            break;
        }
    }

    // Display blockchain
    printf("\n=========== Blockchain ===========\n");
    for (int i = 0; i < numBlocks; i++) {
        printf("\nBlock %d\n", blockchain[i].index);
        printf("Timestamp     : %s\n", blockchain[i].timestamp);
        printf("Data          : %s\n", blockchain[i].data);
        printf("Previous Hash : %s\n", blockchain[i].previousHash);
        printf("Hash          : %s\n", blockchain[i].hash);
    }

    return 0;
}
