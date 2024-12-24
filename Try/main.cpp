#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <functional>
using namespace std;
class Block {
public:
    std::string previousHash;
    std::string hash;
    std::string data;
    time_t timestamp;
    int index;
//C++ timestamp implementation
    Block(int idx, std::string data, std::string prevHash) : index(idx), data(data), previousHash(prevHash) {
        timestamp = time(NULL);
        hash = calculateHash();
    }

    std::string calculateHash() const {
        std::stringstream ss;
        ss << index << timestamp << previousHash << data;
        return simpleHash(ss.str());
    }

    static std::string simpleHash(const std::string& str) {
        std::hash<std::string> hasher;
        auto hash = hasher(str);
        std::stringstream ss;
        ss << std::hex << hash;
        return ss.str();
    }
};

class Blockchain {
public:
    Blockchain() {
        chain.push_back(createGenesisBlock());
    }

    Block createGenesisBlock() {
        return Block(0, "Genesis Block", "0");
    }

    Block getLatestBlock() const {
        return chain.back();
    }

    void addBlock(std::string data) {
        Block newBlock = Block(chain.size(), data, getLatestBlock().hash);
        chain.push_back(newBlock);
    }

    bool isChainValid() const {
        for (size_t i = 1; i < chain.size(); ++i) {
            Block currentBlock = chain[i];
            Block previousBlock = chain[i - 1];

            if (currentBlock.hash != currentBlock.calculateHash()) {
                return false;
            }

            if (currentBlock.previousHash != previousBlock.hash) {
                return false;
            }
        }
        return true;
    }

    std::vector<Block> getChain() const {
        return chain;
    }
private:
    std::vector<Block> chain;
};
int main() {
    Blockchain blockchain;
    int numBlocks;
    std::cout << "Enter the number of blocks to add: ";
    std::cin >> numBlocks;
    std::cin.ignore();
    for (int i = 0; i < numBlocks; ++i) {
        std::string data;
        std::cout << "Enter data for block " << i + 1 << ": ";
        std::getline(std::cin, data);
        blockchain.addBlock(data);
    }
    std::cout << "Blockchain valid: " << blockchain.isChainValid() << std::endl;
    for (const auto& block : blockchain.getChain()) {
        std::cout << "Index: " << block.index << std::endl;
        std::cout << "Data: " << block.data << std::endl;
        std::cout << "Timestamp: " << block.timestamp << std::endl;
        std::cout << "Previous Hash: " << block.previousHash << std::endl;
        std::cout << "Hash: " << block.hash << std::endl;
        std::cout << "------------------------" << std::endl;
    }
    return 0;
}
