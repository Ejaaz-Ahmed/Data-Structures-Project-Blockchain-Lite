#include <iostream>
#include <vector>
#include <sstream>
#include <ctime>
#include <functional>
#include <iomanip>
#ifdef _WIN32
    #include <windows.h>
#endif
using namespace std;

// Color codes for different platforms
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    #define SET_COLOR(color) SetConsoleTextAttribute(hConsole, color)
    #define RESET_COLOR SET_COLOR(7)
    #define BLUE_COLOR SET_COLOR(9)
    #define GREEN_COLOR SET_COLOR(10)
    #define RED_COLOR SET_COLOR(12)
    #define YELLOW_COLOR SET_COLOR(14)
    #define PURPLE_COLOR SET_COLOR(13)
#else
    #define SET_COLOR(color) cout << color
    #define RESET_COLOR cout << "\033[0m"
    #define BLUE_COLOR SET_COLOR("\033[34m")
    #define GREEN_COLOR SET_COLOR("\033[32m")
    #define RED_COLOR SET_COLOR("\033[31m")
    #define YELLOW_COLOR SET_COLOR("\033[33m")
    #define PURPLE_COLOR SET_COLOR("\033[35m")
#endif

class Block {
public:
    int index;
    string previousHash;
    string hash;
    string data;
    time_t timestamp;

    Block(int i, string d, string prehash) {
        index = i;
        data = d;
        previousHash = prehash;
        timestamp = time(NULL);
        hash = calculateHash();
    }

    string calculateHash() const {
        stringstream ss;
        ss << index << timestamp << previousHash << data;
        return simpleHash(ss.str());
    }

    static string simpleHash(const string &str) {
        std::hash<string> hasher;
        auto hashValue = hasher(str);
        stringstream ss;
        ss << hex << hashValue;
        return ss.str();
    }
};

class Blockchain {
private:
    vector<Block> chain;

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

    void addBlock(const string &data) {
        Block newBlock(chain.size(), data, getLatestBlock().hash);
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

    vector<Block> getChain() const {
        return chain;
    }
};

void displayMenu() {
    system("cls||clear");
    PURPLE_COLOR;
    cout << "\n---------------------------------------------------\n";
    cout << "     |       BLOCKCHAIN SIMULATOR               |";
     cout <<"\n---------------------------------------------------\n";
    RESET_COLOR;
}

void displayBlock(const Block& block) {
    BLUE_COLOR;
    cout << "\n--------------------Block #" << block.index << "--------------------\n";
    YELLOW_COLOR;
    cout << "| Data: " << block.data << endl;
    GREEN_COLOR;
    cout << "| Timestamp: " << put_time(localtime(&block.timestamp), "%Y-%m-%d %H:%M:%S") << endl;
    BLUE_COLOR;
    cout << "| Previous Hash: " << block.previousHash.substr(0, 20) << "..." << endl;
    cout << "| Current Hash:  " << block.hash.substr(0, 20) << "..." << endl;
    cout << "------------------------------------------------\n";
    RESET_COLOR;
}

string getInput(const string& prompt) {
    string input;
    YELLOW_COLOR;
    cout << prompt;
    RESET_COLOR;
    getline(cin, input);
    return input;
}

int main() {
    Blockchain blockchain;
    displayMenu();
    while (true) {
        

        YELLOW_COLOR;
        cout << "\n1. Add new transaction";
        cout << "\n2. View blockchain";
        cout << "\n3. Validate blockchain";
        cout << "\n4. Exit";

        string choice = getInput("\n\nEnter your choice (1-4): ");

        if (choice == "1") {
            string data = getInput("\nEnter transaction data: ");
            blockchain.addBlock(data);
            GREEN_COLOR;
            cout << "\n => Transaction added successfully!\n";
            RESET_COLOR;
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == "2") {
            cout << "\nCurrent Blockchain State:\n";
            for (const auto &block : blockchain.getChain()) {
                displayBlock(block);
            }
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == "3") {
            if (blockchain.isChainValid()) {
                GREEN_COLOR;
                cout << "\n=> Blockchain is valid and secure!\n";
            } else {
                RED_COLOR;
                cout << "\n => WARNING: Blockchain has been tampered with!\n";
            }
            RESET_COLOR;
            cout << "\nPress Enter to continue...";
            cin.get();
        }
        else if (choice == "4") {
            PURPLE_COLOR;
            cout << "\nThank you for using Blockchain Simulator! Goodbye!\n\n";
            RESET_COLOR;
            break;
        }
        else {
            RED_COLOR;
            cout << "\nInvalid choice! Press Enter to try again...";
            RESET_COLOR;
            cin.get();
        }
    }
    return 0;
}
