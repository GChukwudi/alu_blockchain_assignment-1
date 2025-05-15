#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/sha.h>

#define MAX_DATA_SIZE 256
#define HASH_SIZE 64
#define MAX_TRANSACTIONS 3
#define MAX_SENDER_SIZE 50
#define MAX_RECEIVER_SIZE 50
#define TRANS_STR_SIZE 150
#define INPUT_BUFFER_SIZE 1024


typedef struct Transaction {
    char sender[MAX_SENDER_SIZE];
    char receiver[MAX_RECEIVER_SIZE];
    double amount;
    time_t timestamp;
} Transaction;

typedef struct Block {
    int index;
    time_t timestamp;
    char data[MAX_DATA_SIZE];
    Transaction transactions[MAX_TRANSACTIONS];
    int transaction_count;
    char previous_hash[HASH_SIZE + 1];
    char hash[HASH_SIZE + 1];
    struct Block *next;
} Block;

typedef struct Blockchain {
    Block *head;
    int length;
} Blockchain;

/* ===== Helper Functions ===== */

// Format timestamp to human-readable string
void format_time(time_t raw_time, char *buffer, size_t size) {
    struct tm *time_info = localtime(&raw_time);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", time_info);
}

// SHA256 hash generator
void calculate_hash(Block *block, char *output_hash) {
    char input[2048] = {0};
    char transaction_buffer[1024] = {0};
    char time_str[30];

    for (int i = 0; i < block->transaction_count; i++) {
        Transaction *tx = &block->transactions[i];
        snprintf(transaction_buffer + strlen(transaction_buffer),
                 sizeof(transaction_buffer) - strlen(transaction_buffer),
                 "%s->%s:%.2f@%ld|",
                 tx->sender, tx->receiver, tx->amount, tx->timestamp);
    }

    snprintf(input, sizeof(input), "%d%ld%s%s%s",
             block->index,
             block->timestamp,
             block->data,
             transaction_buffer,
             block->previous_hash);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output_hash + (i * 2), "%02x", hash[i]);
    }
    output_hash[HASH_SIZE] = '\0';
}

/* ===== Blockchain Core Functions ===== */

// Create the genesis block
Block *create_genesis_block(Transaction *txs, int tx_count) {
    Block *block = (Block *)malloc(sizeof(Block));
    block->index = 0;
    block->timestamp = time(NULL);
    strcpy(block->data, "Genesis Block");

    memcpy(block->transactions, txs, sizeof(Transaction) * tx_count);
    block->transaction_count = tx_count;

    strcpy(block->previous_hash, "0");
    calculate_hash(block, block->hash);
    block->next = NULL;
    return block;
}

// Create a new block from pending transactions
Block *create_block(Block *prev, Transaction *txs, int tx_count) {
    Block *block = (Block *)malloc(sizeof(Block));
    block->index = prev->index + 1;
    block->timestamp = time(NULL);
    strcpy(block->data, "Transaction Block");

    memcpy(block->transactions, txs, sizeof(Transaction) * tx_count);
    block->transaction_count = tx_count;

    strcpy(block->previous_hash, prev->hash);
    calculate_hash(block, block->hash);
    block->next = NULL;
    return block;
}

// Add a block to the blockchain
void add_block(Blockchain *chain, Block *block) {
    Block *temp = chain->head;
    while (temp->next) temp = temp->next;
    temp->next = block;
    chain->length++;
    printf("✅ Block #%d added successfully!\n", block->index);
}

/* ===== Transaction Input and Chain Display ===== */

// Collect transactions from user
int collect_transactions(Transaction *txs) {
    int count = 0;
    char buffer[INPUT_BUFFER_SIZE];

    printf("💰 Enter up to %d transactions:\n", MAX_TRANSACTIONS);
    while (count < MAX_TRANSACTIONS) {
        printf("\nTransaction #%d\n", count + 1);
        printf("Sender    : ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';
        if (strlen(buffer) == 0) break;
        strncpy(txs[count].sender, buffer, MAX_SENDER_SIZE);

        printf("Receiver  : ");
        fgets(txs[count].receiver, MAX_RECEIVER_SIZE, stdin);
        txs[count].receiver[strcspn(txs[count].receiver, "\n")] = '\0';

        printf("Amount    : ");
        fgets(buffer, sizeof(buffer), stdin);
        txs[count].amount = atof(buffer);
        txs[count].timestamp = time(NULL);

        count++;
    }

    return count;
}

// Print the entire blockchain
void print_chain(Blockchain *chain) {
    Block *current = chain->head;
    char time_buf[30];

    printf("\n================ 📦 BLOCKCHAIN LEDGER ================\n\n");

    while (current) {
        format_time(current->timestamp, time_buf, sizeof(time_buf));

        printf("┌───────────────────────────────────────────────────────┐\n");
        printf("│ 🧱 Block #%d\n", current->index);
        printf("│ ──────────────────────────────────────────────────────\n");
        printf("│ 🕒 Timestamp     : %s\n", time_buf);
        printf("│ 📄 Data          : %s\n", current->data);
        printf("│ 🔗 Prev. Hash    : %.20s...%s\n", current->previous_hash, &current->previous_hash[44]);
        printf("│ 🧾 Hash          : %.20s...%s\n", current->hash, &current->hash[44]);
        printf("│ 💸 Transactions  : %d\n", current->transaction_count);
        for (int i = 0; i < current->transaction_count; i++) {
            Transaction *tx = &current->transactions[i];
            format_time(tx->timestamp, time_buf, sizeof(time_buf));
            printf("│    → %s sent %.2f to %s at %s\n",
                   tx->sender, tx->amount, tx->receiver, time_buf);
        }
        printf("└───────────────────────────────────────────────────────┘\n");

        if (current->next)
            printf("                    ⬇️\n");
        current = current->next;
    }

    printf("\n========================================================\n");
}

// Validate blockchain integrity
int validate_chain(Blockchain *chain) {
    Block *curr = chain->head;
    char recalculated_hash[HASH_SIZE + 1];

    while (curr->next) {
        calculate_hash(curr, recalculated_hash);
        if (strcmp(curr->hash, recalculated_hash) != 0) return 0;
        if (strcmp(curr->hash, curr->next->previous_hash) != 0) return 0;
        curr = curr->next;
    }
    return 1;
}

/* ===== Main Function ===== */

int main() {
    Blockchain chain;
    Transaction txs[MAX_TRANSACTIONS];
    printf("🔧 Initializing Genesis Block...\n");
    int tx_count = collect_transactions(txs);
    chain.head = create_genesis_block(txs, tx_count);
    chain.length = 1;

    int choice;
    // Transaction txs[MAX_TRANSACTIONS];

    while (1) {
        printf("\n📌 Menu:\n");
        printf("1. Add new block\n");
        printf("2. Display blockchain\n");
        printf("3. Validate blockchain\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // clear newline

        switch (choice) {
            case 1: {
                int tx_count = collect_transactions(txs);
                if (tx_count == 0) {
                    printf("⚠️  No transactions entered. Block not added.\n");
                } else {
                    Block *new_block = create_block(chain.head, txs, tx_count);
                    add_block(&chain, new_block);
                }
                break;
            }

            case 2:
                print_chain(&chain);
                break;

            case 3:
                if (validate_chain(&chain))
                    printf("✅ Blockchain is valid!\n");
                else
                    printf("❌ Blockchain is INVALID!\n");
                break;

            case 4:
                printf("👋 Exiting...\n");
                return 0;

            default:
                printf("❌ Invalid choice. Try again.\n");
        }
    }

    return 0;
}
