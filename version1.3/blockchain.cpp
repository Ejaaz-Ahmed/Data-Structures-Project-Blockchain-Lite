#include <iostream>
#include <sstream>
#include <ctime>
#include <functional>
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cstring>
#include <chrono>
#include <climits>
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

// Transaction data structure using union instead of variant
struct TransactionData {
    union DataValue {
        int intValue;
        double doubleValue;
        char stringValue[256];
        
        DataValue() {} // Default constructor
    } value;
    
    string dataType;  // "int", "double", or "string"
    string description;

    // Helper method to get value as string for display and hashing
    string getValueAsString() const {
        stringstream ss;
        if (dataType == "int")
            ss << value.intValue;
        else if (dataType == "double")
            ss << value.doubleValue;
        else if (dataType == "string")
            ss << value.stringValue;
        return ss.str();
    }
};

// User identity structure
struct UserIdentity {
    string publicKey;
    string username;
    time_t createdAt;
};

class Block {
public:
    int index;
    string previousHash;
    string hash;
    TransactionData data;
    time_t timestamp;
    UserIdentity creator;
    Block* next;

    Block(int i, TransactionData d, string prehash, UserIdentity user) {
        index = i;
        data = d;
        previousHash = prehash;
        timestamp = time(NULL);
        creator = user;
        hash = calculateHash();
        next = nullptr;
    }

    string calculateHash() const {
        stringstream ss;
        ss << index << timestamp << previousHash;
        ss << data.getValueAsString();
        ss << data.description << creator.publicKey;
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
    Block* head;
    int size;
    map<string, UserIdentity> users;

public:
    Blockchain() {
        head = nullptr;
        size = 0;
        // Create genesis block with system user
        UserIdentity systemUser = {"SYSTEM", "SYSTEM", time(NULL)};
        TransactionData genesisData;
        genesisData.dataType = "string";
        strncpy(genesisData.value.stringValue, "Genesis Block", 255);
        genesisData.description = "System Generated";
        addBlock(genesisData, systemUser);
    }

    ~Blockchain() {
        Block* current = head;
        while (current != nullptr) {
            Block* next = current->next;
            delete current;
            current = next;
        }
    }

    string generatePublicKey(const string& username) {
    random_device rd;
    mt19937_64 gen(rd() ^ (
        static_cast<unsigned long long>(chrono::high_resolution_clock::now()
            .time_since_epoch()
            .count()) + 
        hash<string>{}(username)
    ));
    
    uniform_int_distribution<unsigned long long> dis(0, ULLONG_MAX);
    
    stringstream ss;
    ss << username << "-";
    
    // Generate a longer hex string
    unsigned long long random_value = dis(gen);
    ss << hex << setfill('0') << setw(16) << random_value;
    
    // Add timestamp component
    ss << "-" << hex << chrono::system_clock::now().time_since_epoch().count() % 10000;
    
    return ss.str();
    }

    UserIdentity registerUser(const string& username) {
        UserIdentity newUser;
        newUser.username = username;
        newUser.publicKey = generatePublicKey(username);
        newUser.createdAt = time(NULL);
        users[newUser.publicKey] = newUser;
        return newUser;
    }

    bool verifyUser(const string& publicKey) {
        return users.find(publicKey) != users.end();
    }

    Block* getLatestBlock() const {
        if (!head) return nullptr;
        Block* current = head;
        while (current->next != nullptr) {
            current = current->next;
        }
        return current;
    }

    void addBlock(const TransactionData& data, const UserIdentity& creator) {
        Block* latestBlock = getLatestBlock();
        string prevHash = latestBlock ? latestBlock->hash : "0";
        Block* newBlock = new Block(size, data, prevHash, creator);

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

    void forEach(function<void(const Block&)> callback) const {
        Block* current = head;
        while (current != nullptr) {
            callback(*current);
            current = current->next;
        }
    }

    Block* getBlockAtIndex(int index) const {
        if (index < 0 || index >= size) return nullptr;
        Block* current = head;
        int currentIndex = 0;
        while (current != nullptr && currentIndex < index) {
            current = current->next;
            currentIndex++;
        }
        return current;
    }

    UserIdentity getUserByPublicKey(const string& publicKey) const {
        auto it = users.find(publicKey);
        if (it != users.end()) {
            return it->second;
        }
        return {"", "", 0}; // Return empty user if not found
    }
    void modifyBlockAsNew(int targetIndex, const TransactionData& newData, const UserIdentity& modifier) {
        Block* targetBlock = getBlockAtIndex(targetIndex);
        if (!targetBlock) {
            cout << "Block not found!" << endl;
            return;
        }

        // Create modification record
        TransactionData modData;
        modData.dataType = "string";
        string modificationRecord = "MODIFIED BLOCK " + to_string(targetIndex) + ": ";
        modificationRecord += "New Value: " + newData.getValueAsString();
        modificationRecord += " (Original: " + targetBlock->data.getValueAsString() + ")";
        
        strncpy(modData.value.stringValue, modificationRecord.c_str(), 255);
        modData.value.stringValue[255] = '\0';  // Ensure null termination
        modData.description = "Modification by " + modifier.username;

        // Add as a new block
        addBlock(modData, modifier);
    }
};

void displayBlock(const Block& block) {
    BLUE_COLOR;
    cout << "\n--------------------Block #" << block.index << "--------------------\n";
    YELLOW_COLOR;
    cout << "| Creator: " << block.creator.username << endl;
    cout << "| Public Key: " << block.creator.publicKey << endl;
    GREEN_COLOR;
    cout << "| Data Type: " << block.data.dataType << endl;
    cout << "| Description: " << block.data.description << endl;
    cout << "| Value: " << block.data.getValueAsString() << endl;
    cout << "| Timestamp: " << put_time(localtime(&block.timestamp), "%Y-%m-%d %H:%M:%S") << endl;
    BLUE_COLOR;
    cout << "| Previous Hash: " << block.previousHash.substr(0, 20) << "..." << endl;
    cout << "| Current Hash:  " << block.hash.substr(0, 20) << "..." << endl;
    cout << "------------------------------------------------\n";
    RESET_COLOR;
}

void displayMenu() {
    system("cls||clear");
    PURPLE_COLOR;
    cout << "\n---------------------------------------------------\n";
    cout << "  |               BLOCKCHAIN SIMULATOR        |";
    cout << "\n---------------------------------------------------\n";
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
    UserIdentity currentUser{"", "", 0};
    displayMenu();

    while (true) {
        GREEN_COLOR;
        cout << "\n1. Register new user";
        cout << "\n2. Login with public key";
        cout << "\n3. Add new transaction";
        cout << "\n4. View blockchain";
        cout << "\n5. Validate blockchain";
        cout << "\n6. View specific block";
        cout << "\n7. Modify block";  // Added option
        cout << "\n8. Exit";          // Changed to 8
        RESET_COLOR;

        string choice = getInput("\n\nEnter your choice (1-8): ");

        if (choice == "1") {
            string username = getInput("\nEnter username: ");
            UserIdentity newUser = blockchain.registerUser(username);
            GREEN_COLOR;
            cout << "\n => User registered successfully!";
            cout << "\n => Your public key is: " << newUser.publicKey << endl;
            cout << "\n => Please save this key for future login!" << endl;
            RESET_COLOR;
        }
        else if (choice == "2") {
            string publicKey = getInput("\nEnter your public key: ");
            if (blockchain.verifyUser(publicKey)) {
                currentUser = blockchain.getUserByPublicKey(publicKey);
                GREEN_COLOR;
                cout << "\n => Login successful! Welcome " << currentUser.username << "!" << endl;
                RESET_COLOR;
            } else {
                RED_COLOR;
                cout << "\n => Invalid public key!" << endl;
                RESET_COLOR;
            }
        }
        else if (choice == "3") {
            if (currentUser.publicKey.empty()) {
                RED_COLOR;
                cout << "\n => Please login first!" << endl;
                RESET_COLOR;
                continue;
            }

            cout << "\nSelect data type to add in Transaction:";
            cout << "\n1. Numeric (Integer)";
            cout << "\n2. Decimal (Double)";
            cout << "\n3. Alphabetic (String)\n";
            string typeChoice = getInput("Enter choice (1-3): ");

            TransactionData tData;
            tData.description = getInput("Enter description: ");

            if (typeChoice == "1") {
                tData.dataType = "int";
                try {
                    tData.value.intValue = stoi(getInput("Enter integer value: "));
                } catch (...) {
                    RED_COLOR;
                    cout << "\nInvalid integer!" << endl;
                    RESET_COLOR;
                    continue;
                }
            }
            else if (typeChoice == "2") {
                tData.dataType = "double";
                try {
                    tData.value.doubleValue = stod(getInput("Enter double value: "));
                } catch (...) {
                    RED_COLOR;
                    cout << "\nInvalid double!" << endl;
                    RESET_COLOR;
                    continue;
                }
            }
            else if (typeChoice == "3") {
                tData.dataType = "string";
                string strValue = getInput("Enter string value: ");
                strncpy(tData.value.stringValue, strValue.c_str(), 255);
                tData.value.stringValue[255] = '\0';  // Ensure null termination
            }
            else {
                RED_COLOR;
                cout << "\nInvalid choice!" << endl;
                RESET_COLOR;
                continue;
            }

            blockchain.addBlock(tData, currentUser);
            GREEN_COLOR;
            cout << "\n => Transaction added successfully!\n";
            RESET_COLOR;
        }
        else if (choice == "4") {
            cout << "\nCurrent Blockchain State:\n";
            blockchain.forEach([](const Block& block) {
                displayBlock(block);
            });
        }
        else if (choice == "5") {
            if (blockchain.isChainValid()) {
                GREEN_COLOR;
                cout << "\n=> Blockchain is valid and secure!\n";
            } else {
                RED_COLOR;
                cout << "\n => WARNING: Blockchain has been tampered with!\n";
            }
            RESET_COLOR;
        }
        else if (choice == "6") {
            string indexStr = getInput("\nEnter block index to view: ");
            try {
                int index = stoi(indexStr);
                Block* block = blockchain.getBlockAtIndex(index);
                if (block) {
                    displayBlock(*block);
                } else {
                    RED_COLOR;
                    cout << "\nBlock not found at index " << index << endl;
                    RESET_COLOR;
                }
            } catch (...) {
                RED_COLOR;
                cout << "\nInvalid index format!" << endl;
                RESET_COLOR;
            }
        }
        else if (choice == "7") {
            if (currentUser.publicKey.empty()) {
                RED_COLOR;
                cout << "\n => Please login first!" << endl;
                RESET_COLOR;
                continue;
            }

            string indexStr = getInput("\nEnter block index to modify: ");
            try {
                int index = stoi(indexStr);
                Block* block = blockchain.getBlockAtIndex(index);
                if (block) {
                    cout << "\nCurrent block data: " << block->data.getValueAsString() << endl;
                    
                    TransactionData newData;
                    cout << "\nSelect new data type:";
                    cout << "\n1. Integer";
                    cout << "\n2. Double";
                    cout << "\n3. String\n";
                    string typeChoice = getInput("Enter choice (1-3): ");

                    newData.description = getInput("Enter new description: ");

                    if (typeChoice == "1") {
                        newData.dataType = "int";
                        try {
                            newData.value.intValue = stoi(getInput("Enter integer value: "));
                        } catch (...) {
                            RED_COLOR;
                            cout << "\nInvalid integer!" << endl;
                            RESET_COLOR;
                            continue;
                        }
                    }
                    else if (typeChoice == "2") {
                        newData.dataType = "double";
                        try {
                            newData.value.doubleValue = stod(getInput("Enter double value: "));
                        } catch (...) {
                            RED_COLOR;
                            cout << "\nInvalid double!" << endl;
                            RESET_COLOR;
                            continue;
                        }
                    }
                    else if (typeChoice == "3") {
                        newData.dataType = "string";
                        string strValue = getInput("Enter string value: ");
                        strncpy(newData.value.stringValue, strValue.c_str(), 255);
                        newData.value.stringValue[255] = '\0';
                    }
                    else {
                        RED_COLOR;
                        cout << "\nInvalid choice!" << endl;
                        RESET_COLOR;
                        continue;
                    }

                    blockchain.modifyBlockAsNew(index, newData, currentUser);
                    GREEN_COLOR;
                    cout << "\n => Modification added as new block!\n";
                    RESET_COLOR;
                } else {
                    RED_COLOR;
                    cout << "\nBlock not found at index " << index << endl;
                    RESET_COLOR;
                }
            } catch (...) {
                RED_COLOR;
                cout << "\nInvalid index format!" << endl;
                RESET_COLOR;
            }
        }
        else if (choice == "8") { 
            PURPLE_COLOR;
            cout << "\nThank you for using Enhanced Blockchain Simulator! Goodbye!\n\n";
            RESET_COLOR;
            break;
        }
        else {
            RED_COLOR;
            cout << "\nInvalid choice! Please try again...";
            RESET_COLOR;
        }

        cout << "\nPress Enter to continue...";
        cin.get();
    }
    return 0;
}