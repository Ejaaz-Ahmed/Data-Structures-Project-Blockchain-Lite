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
