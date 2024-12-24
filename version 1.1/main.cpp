#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <functional>
using namespace std;

class Block {
public:
    int index;
    string previousHash;
    string hash;
    string data;
    time_t timestamp;


    Block(int i, string d, string prehash)
  //      : index(i), data(d), previousHash(prehash)
        {
            index=i;
            data=d;
            previousHash=prehash;
        timestamp = time(NULL);
        hash = calculateHash();
    }

    // Calculate the hash for the block
    string calculateHash() const {
        stringstream ss;
        ss << index << timestamp << previousHash << data;
        return simpleHash(ss.str());
    }

    // Static hash function
    static string simpleHash(const string &str) {
        std::hash <string> hasher;
        auto hashValue = hasher(str);
        stringstream ss;
        ss << std::hex << hashValue;
        return ss.str();
    }
};

class Blockchain {
private:
    vector<Block> chain;

public:
    // Constructor to initialize the blockchain with a genesis block
    Blockchain() {
        chain.push_back(createGenesisBlock());
    }

    // Create the genesis block
    Block createGenesisBlock() {
        return Block(0, "Genesis Block", "0");
    }

    // Get the latest block in the chain
    Block getLatestBlock() const {
        return chain.back();
    }

    // Add a new block to the chain
    void addBlock(const string &data) {
        Block newBlock(chain.size(), data, getLatestBlock().hash);
        chain.push_back(newBlock);
    }

    // Validate the blockchain
    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            Block currentBlock = chain[i];
            Block previousBlock = chain[i - 1];

            // Check if the current block's hash is correct
            if (currentBlock.hash != currentBlock.calculateHash()) {
                return false;
            }

            // Check if the current block's previous hash matches the previous block's hash
            if (currentBlock.previousHash != previousBlock.hash) {
                return false;
            }
        }
        return true;
    }

    // Get the entire blockchain
    vector<Block> getChain() const {
        return chain;
    }
};

int main() {
    Blockchain blockchain;
    int numBlocks;

    cout << "How many transactions do you want to add?" << endl;
    cin >> numBlocks;
    cin.ignore();

    for (int i = 0; i < numBlocks; ++i) {
        string data;
        cout << "Enter the note for transaction to write in block " << i + 1 << ": ";
        getline(cin, data);
        blockchain.addBlock(data);
    }

    for (const auto &block : blockchain.getChain()) {
        cout << "Index: " << block.index << endl;
        cout << "Data: " << block.data << endl;
        cout << "Timestamp: " << block.timestamp << endl;
        cout << "Previous Hash: " << block.previousHash << endl;
        cout << "Hash: " << block.hash << endl;
        cout << "------------------------" << endl;
    }
    if (blockchain.isChainValid()) {
        cout << "Blockchain is valid." << endl;
    } else {
        cout << "Blockchain is invalid!" << endl;
    }

    return 0;
}
