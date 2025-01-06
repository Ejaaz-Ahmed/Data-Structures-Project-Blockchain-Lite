#include <iostream>
#include <sstream>
#include <ctime>
#include <functional>
#include <iomanip>
#ifdef _WIN32
    #include <windows.h>
#endif
using namespace std;

// Color codes remain the same
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

// Modified Block class to include next pointer
class Block {
public:
    int index;
    string previousHash;
    string hash;
    string data;
    time_t timestamp;
    Block* next;  // Added for linked list implementation

    Block(int i, string d, string prehash) {
        index = i;
        data = d;
        previousHash = prehash;
        timestamp = time(NULL);
        hash = calculateHash();
        next = nullptr;  // Initialize next pointer
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
    Block* head;  // Head of the linked list
    int size;     // Keep track of the chain size

public:
    Blockchain() {
        head = nullptr;
        size = 0;
        // Create genesis block
        addBlock("Genesis Block");
    }

    ~Blockchain() {
        // Cleanup linked list
        Block* current = head;
        while (current != nullptr) {
            Block* next = current->next;
            delete current;
            current = next;
        }
    }

    Block* getLatestBlock() const {
        if (!head) return nullptr;
        Block* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        return current;
    }

    void addBlock(const string &data) {
        Block* latestBlock = getLatestBlock();
        string prevHash = latestBlock ? latestBlock->hash : "0";
        Block* newBlock = new Block(size, data, prevHash);

        if (!head) {
            head = newBlock;
        } else {
            latestBlock->next = newBlock;
        }
        size++;
    }

    bool isChainValid() const {
        if (!head || !head->next) return true;

        Block* current = head->next;
        Block* previous = head;

        while (current != nullptr) {
            if (current->hash != current->calculateHash()) {
                return false;
            }
            if (current->previousHash != previous->hash) {
                return false;
            }
            previous = current;
            current = current->next;
        }
        return true;
    }

    // Iterator function to simulate vector-like access
    void forEach(function<void(const Block&)> callback) const {
        Block* current = head;
        while (current != nullptr) {
            callback(*current);
            current = current->next;
        }
    }
};

// Display functions remain the same
void displayMenu() {
    system("cls||clear");
    PURPLE_COLOR;
    cout << "\n---------------------------------------------------\n";
    cout << "     |       BLOCKCHAIN SIMULATOR               |";
    cout << "\n---------------------------------------------------\n";
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

    while (true) {
        displayMenu();

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
            blockchain.forEach([](const Block& block) {
                displayBlock(block);
            });
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
