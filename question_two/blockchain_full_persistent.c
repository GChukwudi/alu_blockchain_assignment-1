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

void format_time(time_t raw_time, char *buffer, size_t size) {
    struct tm *timeinfo = localtime(&raw_time);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", timeinfo);
}

void calculate_hash(Block *block, char *output_hash) {
    char input[1024] = {0};
    char transaction_data[512] = {0};
    char time_buffer[64];

    for (int i = 0; i < block->transaction_count; i++) {
        char trans[256];
        format_time(block->transactions[i].timestamp, time_buffer, sizeof(time_buffer));
        snprintf(trans, sizeof(trans), "%s%s%.2f%s",
                 block->transactions[i].sender,
                 block->transactions[i].receiver,
                 block->transactions[i].amount,
                 time_buffer);
        strcat(transaction_data, trans);
    }

    snprintf(input, sizeof(input), "%d%ld%s%s%s",
             block->index, block->timestamp,
             block->data, transaction_data, block->previous_hash);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char *)input, strlen(input), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output_hash + (i * 2), "%02x", hash[i]);
    }
    output_hash[HASH_SIZE] = '\0';
}

Block *create_genesis_block(Transaction *txs, int count) {
    Block *block = (Block *)malloc(sizeof(Block));
    block->index = 0;
    block->timestamp = time(NULL);
    strcpy(block->data, "Genesis Block");
    memcpy(block->transactions, txs, count * sizeof(Transaction));
    block->transaction_count = count;
    strcpy(block->previous_hash, "0");
    calculate_hash(block, block->hash);
    block->next = NULL;
    return block;
}

Block *create_block(Block *prev, const char *data, Transaction *txs, int count) {
    Block *block = (Block *)malloc(sizeof(Block));
    block->index = prev->index + 1;
    block->timestamp = time(NULL);
    strncpy(block->data, data, MAX_DATA_SIZE - 1);
    memcpy(block->transactions, txs, count * sizeof(Transaction));
    block->transaction_count = count;
    strncpy(block->previous_hash, prev->hash, HASH_SIZE);
    calculate_hash(block, block->hash);
    block->next = NULL;
    return block;
}

void add_block(Blockchain *chain, const char *data, Transaction *txs, int count) {
    Block *new_block = create_block(chain->head, data, txs, count);
    Block *temp = chain->head;
    while (temp->next) temp = temp->next;
    temp->next = new_block;
    chain->length++;
    printf("✅ Block %d added.", new_block->index);
}

int validate_chain(Blockchain *chain) {
    Block *current = chain->head;
    while (current && current->next) {
        char expected_hash[HASH_SIZE + 1];
        calculate_hash(current, expected_hash);
        if (strcmp(current->hash, expected_hash) != 0) return 0;
        if (strcmp(current->hash, current->next->previous_hash) != 0) return 0;
        current = current->next;
    }
    return 1;
}

void display_blockchain(Blockchain *chain) {
    Block *current = chain->head;
    char buffer[64];
    while (current) {
        format_time(current->timestamp, buffer, sizeof(buffer));
        printf("\n🔷 Block #%d\n", current->index);
        printf("Time: %s\n", buffer);
        printf("Data: %s\n", current->data);
        printf("PrevHash: %.20s...\n", current->previous_hash);
        printf("Hash    : %.20s...\n", current->hash);
        printf("Transactions (%d):\n", current->transaction_count);
        for (int i = 0; i < current->transaction_count; i++) {
            format_time(current->transactions[i].timestamp, buffer, sizeof(buffer));
            printf(" - %s -> %s : %.2f at %s\n",
                   current->transactions[i].sender,
                   current->transactions[i].receiver,
                   current->transactions[i].amount,
                   buffer);
        }
        current = current->next;
    }
}

void save_blockchain_to_file(Blockchain *chain, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return;
    Block *current = chain->head;
    while (current) {
        fprintf(fp, "BLOCK\n");
        fprintf(fp, "%d\n%ld\n%s\n%d\n%s\n%s\n",
                current->index, current->timestamp, current->data,
                current->transaction_count, current->previous_hash, current->hash);
        for (int i = 0; i < current->transaction_count; i++) {
            fprintf(fp, "%s;%s;%.2f;%ld\n",
                    current->transactions[i].sender,
                    current->transactions[i].receiver,
                    current->transactions[i].amount,
                    current->transactions[i].timestamp);
        }
        current = current->next;
    }
    fclose(fp);
    printf("✅ Blockchain saved.\n");
}

void free_chain(Block *head) {
    while (head) {
        Block *temp = head;
        head = head->next;
        free(temp);
    }
}

Blockchain load_blockchain_from_file(const char *filename) {
    Blockchain chain = {NULL, 0};
    FILE *fp = fopen(filename, "r");
    if (!fp) return chain;

    char line[256];
    Block *prev = NULL;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "BLOCK", 5) == 0) {
            Block *block = (Block *)malloc(sizeof(Block));
            fscanf(fp, "%d\n%ld\n", &block->index, &block->timestamp);
            fgets(block->data, sizeof(block->data), fp);
            block->data[strcspn(block->data, "\n")] = '\0';
            fscanf(fp, "%d\n", &block->transaction_count);
            fgets(block->previous_hash, sizeof(block->previous_hash), fp);
            block->previous_hash[strcspn(block->previous_hash, "\n")] = '\0';
            fgets(block->hash, sizeof(block->hash), fp);
            block->hash[strcspn(block->hash, "\n")] = '\0';
            for (int i = 0; i < block->transaction_count; i++) {
                fscanf(fp, "%49[^;];%49[^;];%lf;%ld\n",
                       block->transactions[i].sender,
                       block->transactions[i].receiver,
                       &block->transactions[i].amount,
                       &block->transactions[i].timestamp);
            }
            block->next = NULL;
            char expected[HASH_SIZE + 1];
            calculate_hash(block, expected);
            if (strcmp(expected, block->hash) != 0) {
                printf("❌ Hash mismatch at block %d\n", block->index);
                free_chain(chain.head);
                chain.head = NULL;
                chain.length = 0;
                fclose(fp);
                return chain;
            }
            if (!prev) {
                chain.head = block;
            } else {
                prev->next = block;
            }
            prev = block;
            chain.length++;
        }
    }

    fclose(fp);
    return chain;
}

int main() {
    Blockchain chain = {NULL, 0};
    char filename[] = "blockchain.txt";
    int load = 0;

    printf("Load existing blockchain? (1 = yes): ");
    scanf("%d", &load);
    getchar();

    if (load) {
        chain = load_blockchain_from_file(filename);
        if (!chain.head) {
            printf("Failed to load. Creating new chain.\n");
        }
    }

    if (!chain.head) {
        Transaction txs[1] = {{"system", "alice", 100.0, time(NULL)}};
        chain.head = create_genesis_block(txs, 1);
        chain.length = 1;
    }

    int choice;
    while (1) {
        printf("\nMenu:\n1. Add block\n2. View blockchain\n3. Validate\n4. Save\n5. Exit\nChoice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 1) {
            char data[MAX_DATA_SIZE];
            Transaction txs[MAX_TRANSACTIONS];
            int tx_count;

            printf("Enter block data: ");
            fgets(data, sizeof(data), stdin);
            data[strcspn(data, "\n")] = '\0';

            printf("Number of transactions (max %d): ", MAX_TRANSACTIONS);
            scanf("%d", &tx_count);
            getchar();

            if (tx_count > MAX_TRANSACTIONS) tx_count = MAX_TRANSACTIONS;
            for (int i = 0; i < tx_count; i++) {
                printf("Transaction %d:\n", i + 1);
                printf("Sender: "); fgets(txs[i].sender, MAX_SENDER_SIZE, stdin);
                txs[i].sender[strcspn(txs[i].sender, "\n")] = '\0';
                printf("Receiver: "); fgets(txs[i].receiver, MAX_RECEIVER_SIZE, stdin);
                txs[i].receiver[strcspn(txs[i].receiver, "\n")] = '\0';
                printf("Amount: "); scanf("%lf", &txs[i].amount); getchar();
                txs[i].timestamp = time(NULL);
            }

            add_block(&chain, data, txs, tx_count);
        } else if (choice == 2) {
            display_blockchain(&chain);
        } else if (choice == 3) {
            if (validate_chain(&chain)) printf("✅ Blockchain valid.\n");
            else printf("❌ Blockchain invalid.\n");
        } else if (choice == 4) {
            save_blockchain_to_file(&chain, filename);
        } else {
            printf("Goodbye!\n");
            break;
        }
    }

    free_chain(chain.head);
    return 0;
}
