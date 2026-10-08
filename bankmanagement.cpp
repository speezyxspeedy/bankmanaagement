#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>

using namespace std;

class BankAccount {
private:
    int accountNumber;
    char accountHolderName[50];
    char accountType; // 'S' for Savings, 'C' for Current
    double balance;

public:
    // Naya account create karne ke liye
    void createAccount() {
        cout << "\nEnter Account Number: ";
        cin >> accountNumber;
        cin.ignore();

        cout << "Enter Account Holder Name: ";
        cin.getline(accountHolderName, 50);

        cout << "Enter Account Type (S for Savings / C for Current): ";
        cin >> accountType;
        accountType = toupper(accountType);

        cout << "Enter Initial Deposit Amount (Min 500 for S, 1000 for C): ";
        cin >> balance;

        cout << "\nAccount created successfully!\n";
    }

    // Account details display karne ke liye
    void displayAccount() const {
        cout << "\n----------------------------------------";
        cout << "\nAccount Number : " << accountNumber;
        cout << "\nAccount Holder : " << accountHolderName;
        cout << "\nAccount Type   : " << (accountType == 'S' ? "Savings" : "Current");
        cout << "\nBalance        : Rs. " << fixed << setprecision(2) << balance;
        cout << "\n----------------------------------------\n";
    }

    // Modifying existing details
    void modifyAccount() {
        cout << "\nAccount Number: " << accountNumber;
        cin.ignore();
        cout << "\nEnter New Account Holder Name: ";
        cin.getline(accountHolderName, 50);

        cout << "Enter New Account Type (S/C): ";
        cin >> accountType;
        accountType = toupper(accountType);

        cout << "Enter Updated Balance: ";
        cin >> balance;
    }

    // Deposit operation
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "\nDeposit Successful! Current Balance: Rs. " << fixed << setprecision(2) << balance << "\n";
        } else {
            cout << "\nInvalid deposit amount.\n";
        }
    }

    // Withdrawal operation
    void withdraw(double amount) {
        if (amount <= 0) {
            cout << "\nInvalid withdrawal amount.\n";
            return;
        }

        double minBalance = (accountType == 'S') ? 500.0 : 1000.0;

        if (balance - amount < minBalance) {
            cout << "\nTransaction Failed! Minimum balance requirement not met (Min: Rs. " << minBalance << ").\n";
        } else {
            balance -= amount;
            cout << "\nWithdrawal Successful! Remaining Balance: Rs. " << fixed << setprecision(2) << balance << "\n";
        }
    }

    // Tabular display for all accounts
    void displayTabular() const {
        cout << setw(12) << accountNumber 
             << setw(25) << accountHolderName 
             << setw(10) << accountType 
             << setw(15) << fixed << setprecision(2) << balance << "\n";
    }

    int getAccountNumber() const {
        return accountNumber;
    }

    double getBalance() const {
        return balance;
    }
};

// Global File Handling Utility Functions
const char FILE_NAME[] = "bank_records.dat";

void writeAccountToFile() {
    BankAccount ac;
    ofstream outFile(FILE_NAME, ios::binary | ios::app);
    if (!outFile) {
        cout << "\nError opening file!\n";
        return;
    }

    ac.createAccount();
    outFile.write(reinterpret_cast<char*>(&ac), sizeof(BankAccount));
    outFile.close();
}

void displaySpecificAccount(int accNo) {
    BankAccount ac;
    ifstream inFile(FILE_NAME, ios::binary);
    if (!inFile) {
        cout << "\nFile could not be opened. No records found.\n";
        return;
    }

    bool found = false;
    while (inFile.read(reinterpret_cast<char*>(&ac), sizeof(BankAccount))) {
        if (ac.getAccountNumber() == accNo) {
            ac.displayAccount();
            found = true;
            break;
        }
    }
    inFile.close();

    if (!found) {
        cout << "\nAccount Number " << accNo << " does not exist.\n";
    }
}

void displayAllAccounts() {
    BankAccount ac;
    ifstream inFile(FILE_NAME, ios::binary);
    if (!inFile) {
        cout << "\nNo records found or file cannot be opened.\n";
        return;
    }

    cout << "\n=================================================================\n";
    cout << setw(12) << "A/C No." << setw(25) << "Account Holder" << setw(10) << "Type" << setw(15) << "Balance\n";
    cout << "=================================================================\n";

    while (inFile.read(reinterpret_cast<char*>(&ac), sizeof(BankAccount))) {
        ac.displayTabular();
    }
    cout << "=================================================================\n";
    inFile.close();
}

void performTransaction(int accNo, int transactionType) {
    BankAccount ac;
    fstream file(FILE_NAME, ios::binary | ios::in | ios::out);
    if (!file) {
        cout << "\nFile could not be opened.\n";
        return;
    }

    bool found = false;
    double amount;

    while (!file.eof() && file.read(reinterpret_cast<char*>(&ac), sizeof(BankAccount))) {
        if (ac.getAccountNumber() == accNo) {
            ac.displayAccount();

            if (transactionType == 1) { // Deposit
                cout << "\nEnter amount to Deposit: Rs. ";
                cin >> amount;
                ac.deposit(amount);
            } else if (transactionType == 2) { // Withdraw
                cout << "\nEnter amount to Withdraw: Rs. ";
                cin >> amount;
                ac.withdraw(amount);
            }

            // File pointer ko update karne ke liye piche le jana
            int pos = (-1) * static_cast<int>(sizeof(BankAccount));
            file.seekp(pos, ios::cur);
            file.write(reinterpret_cast<char*>(&ac), sizeof(BankAccount));

            found = true;
            break;
        }
    }
    file.close();

    if (!found) {
        cout << "\nAccount Number " << accNo << " not found.\n";
    }
}

void deleteAccount(int accNo) {
    BankAccount ac;
    ifstream inFile(FILE_NAME, ios::binary);
    if (!inFile) {
        cout << "\nFile could not be opened.\n";
        return;
    }

    ofstream outFile("temp.dat", ios::binary);
    bool found = false;

    while (inFile.read(reinterpret_cast<char*>(&ac), sizeof(BankAccount))) {
        if (ac.getAccountNumber() != accNo) {
            outFile.write(reinterpret_cast<char*>(&ac), sizeof(BankAccount));
        } else {
            found = true;
        }
    }

    inFile.close();
    outFile.close();

    remove(FILE_NAME);
    rename("temp.dat", FILE_NAME);

    if (found) {
        cout << "\nAccount Number " << accNo << " deleted successfully.\n";
    } else {
        cout << "\nAccount Number " << accNo << " not found.\n";
    }
}

int main() {
    int choice;
    int accNo;

    do {
        cout << "\n==========================================";
        cout << "\n        BANK MANAGEMENT SYSTEM";
        cout << "\n==========================================";
        cout << "\n1. Open New Account";
        cout << "\n2. Deposit Amount";
        cout << "\n3. Withdraw Amount";
        cout << "\n4. Balance Inquiry / Account Details";
        cout << "\n5. Display All Accounts List";
        cout << "\n6. Close / Delete Account";
        cout << "\n7. Exit";
        cout << "\n------------------------------------------";
        cout << "\nSelect Option (1-7): ";
        cin >> choice;

        switch (choice) {
            case 1:
                writeAccountToFile();
                break;
            case 2:
                cout << "\nEnter Account Number: ";
                cin >> accNo;
                performTransaction(accNo, 1);
                break;
            case 3:
                cout << "\nEnter Account Number: ";
                cin >> accNo;
                performTransaction(accNo, 2);
                break;
            case 4:
                cout << "\nEnter Account Number: ";
                cin >> accNo;
                displaySpecificAccount(accNo);
                break;
            case 5:
                displayAllAccounts();
                break;
            case 6:
                cout << "\nEnter Account Number to Delete: ";
                cin >> accNo;
                deleteAccount(accNo);
                break;
            case 7:
                cout << "\nExiting System. Thank you!\n";
                break;
            default:
                cout << "\nInvalid selection. Try again.\n";
        }
    } while (choice != 7);

    return 0;
}