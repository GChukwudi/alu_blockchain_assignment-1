## Simple Blockchain Implementation in C

This project walks through building a basic blockchain system using C. It is divided into four progressive tasks that introduce key concepts like block structure, cryptographic hashing, transaction simulation, and persistent storage.

### Task 1: Defining the Block Structure
Implement the foundational structure for blockchain blocks.
- Define a Block structure that includes:
- Block index
- Timestamp
- Data field
- Previous block's hash
- Current block’s hash

#### How to Compile & Run
```bash
gcc block_structure.c -o block_structure -lssl -lcrypto
./block_structure
```

### Task 2: Creating a Blockchain
Establishes a linked list of blocks to simulate a blockchain.
- Add new blocks to the blockchain while preserving hash integrity.
- Use SHA-256 hashing to link blocks securely.
- Validate the entire blockchain structure by checking hashes.

#### How to Compile & Run
```bash
gcc blockchain.c -o blockchain -lssl -lcrypto
./blockchain
```

### Task 3: Simulating Transactions
Enhances each block with transaction capabilities.

- Extend the Block structure to support multiple transactions.
- Implement a structure for each transaction:
- Sender
- Receiver
- Amount
- Timestamp
- Ensure that transaction changes reflect in the block's hash.

#### How to Compile & Run
```bash
gcc blockchain_transaction.c -o blockchain_transaction -lssl -lcrypto
./blockchain_transaction
```

### Task 4: Blockchain Persistence
Adds the ability to save and load the blockchain from a file.
- Implement serialization to store the blockchain to disk.
- Implement deserialization to reconstruct the blockchain from a file.
- Revalidate hashes on load to ensure no tampering has occurred.

#### How to Compile & Run
```bash
gcc blockchain_full_persistent.c -o blockchain_full_persistent -lssl -lcrypto
./blockchain_full_persistent
```
