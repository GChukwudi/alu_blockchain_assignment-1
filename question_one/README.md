## Overview

This project demonstrates two fundamental concepts in blockchain development using C:
- SHA-256 Hashing — Understanding how data is securely hashed.
- Simple Blockchain Simulation — Building a minimal blockchain that validates data integrity using cryptographic hashes.

### Task 1: Implement SHA-256 Hashing
To understand how cryptographic hashing ensures data integrity in a blockchain.
- Accepts a string input from the user.
- Computes and displays its SHA-256 hash.
- Internally tests the hash of "Blockchain Cryptography" before the user's input.
- Utilizes OpenSSL’s SHA-256 implementation.

#### How to Compile & Run
```bash
gcc SHA-256_hash.c -o hasher -lssl -lcrypto
./hasher
```

### Task 2: Simple Blockchain Simulation
Simulate a small blockchain system in C, where each block contains:
- Index
- Timestamp
- Data
- Previous block's hash
- Current block’s hash
The system validates the entire chain before any new block is added.

#### How it works?
- Genesis block created automatically.
- Prompts user to enter data for 3 subsequent blocks.
- Validates chain integrity before each block is added.
- Uses SHA-256 to hash each block’s contents.
- Alerts user if tampering or corruption breaks the chain.

#### How to Compile & Run
```bash
gcc blockchain_simulation.c -o blockchain -lssl -lcrypto
./blockchain
```

### Dependencies
OpenSSL — For SHA-256 hashing.
