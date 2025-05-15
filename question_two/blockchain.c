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
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

// Set the maximum data size and hash size
#define MAX_DATA_SIZE 256
#define HASH_SIZE 65

// Define the structure for each Block
typedef struct Block {
    int index;
    char timestamp[100];
    char data[MAX_DATA_SIZE];
    char previousHash[HASH_SIZE];
    char hash[HASH_SIZE];
    struct Block *next;
} Block;

// Global variable to keep track of the head of the blockchain
Block *head = NULL;

//Function to extract the timestamp
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

// Function to create a new block
Block *create_block(const char *data) {
    Block *new_block = (Block *)malloc(sizeof(Block));

    if (head == NULL) {
    new_block->index = 0;
        } else {
            Block *temp = head;
            while (temp->next != NULL)
                temp = temp->next;
            new_block->index = temp->index + 1;
        }

    get_timestamp(new_block->timestamp, sizeof(new_block->timestamp));
    strncpy(new_block->data, data, MAX_DATA_SIZE - 1);
    new_block->data[MAX_DATA_SIZE - 1] = '\0';

    if (head == NULL) {
        strncpy(new_block->previousHash, "0", HASH_SIZE - 1);
    } else {
        strncpy(new_block->previousHash, head->hash, HASH_SIZE - 1);
    }
    new_block->previousHash[HASH_SIZE - 1] = '\0';

    calculate_hash(new_block, new_block->hash);
    new_block->next = NULL;

    return new_block;
}

// Function to add a new block to the blockchain
void add_block(const char *data) {
    Block *new_block = create_block(data);

    if (head == NULL) {
        head = new_block;
    } else {
        Block *temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = new_block;
    }
    printf("✅ Block %d added successfully!\n", new_block->index);
}

// Function to validate the blockchain
// Check if the hash of each block matches the previous block's hash
int validate_chain() {
    Block *current = head;
    while (current && current->next) {
        char expectedHash[HASH_SIZE];
        calculate_hash(current, expectedHash);

        if (strcmp(current->hash, expectedHash) != 0)
            return 0;
        if (strcmp(current->hash, current->next->previousHash) != 0)
            return 0;

        current = current->next;
    }
    return 1;
}

// Function to display the blockchain
void display_chain() {
    Block *temp = head;
    printf("\n================= 📦 BLOCKCHAIN LEDGER =================\n\n");
    while (temp != NULL) {
        printf("┌───────────────────────────────────────────────────────┐\n");
        printf("│ 🧱 Block #%d\n", temp->index);
        printf("│ ──────────────────────────────────────────────────────\n");
        printf("│ 📅 Timestamp     : %s\n", temp->timestamp);
        printf("│ ✉️  Data          : %s\n", temp->data);
        printf("│ 🔗 Prev. Hash    : %.20s...%s\n", temp->previousHash, &temp->previousHash[44]);
        printf("│ 🧾 Hash          : %.20s...%s\n", temp->hash, &temp->hash[44]);
        printf("└───────────────────────────────────────────────────────┘\n");

        if (temp->next != NULL)
            printf("                  ⬇️\n");

        temp = temp->next;
    }
    printf("\n=========================================================\n");
}


int main() {
    char data[MAX_DATA_SIZE];
    int choice;

    printf("Creating the genesis block...\n");
    Block *genesis = create_block("Genesis Block");
    head = genesis;
    printf("Genesis block created successfully!\n");

    while (1) {
        printf("\n📌 Blockchain Menu:\n");
        printf("1. Add new block\n");
        printf("2. Display blockchain\n");
        printf("3. Validate blockchain\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar(); // clear newline

        switch (choice) {
            case 1:
                printf("Enter data for new block: ");
                fgets(data, MAX_DATA_SIZE, stdin);
                data[strcspn(data, "\n")] = '\0'; // remove newline
                add_block(data);
                break;

            case 2:
                display_chain();
                break;

            case 3:
                if (validate_chain()) {
                    printf("✅ Blockchain is valid!\n");
                } else {
                    printf("❌ Blockchain is INVALID!\n");
                }
                break;

            case 4:
                printf("Exiting...\n");
                exit(0);

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    return 0;
}
