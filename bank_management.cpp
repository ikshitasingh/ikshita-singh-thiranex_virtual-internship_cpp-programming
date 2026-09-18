/*
    ============================================================
    BANK MANAGEMENT APPLICATION
    ============================================================
    Language   : C++
    Concepts   : Object-Oriented Programming, File Handling
    Features   : Create Account, Deposit, Withdraw, Balance Check,
                 Display All Accounts, Modify Account, Delete Account
    Storage    : Binary file (bank_data.dat) for persistent records
    ============================================================
*/

#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <cstring>

using namespace std;

// ------------------------------------------------------------
// Account Class
// Encapsulates all data & behaviour of a single bank account.
// ------------------------------------------------------------
class Account {
private:
    int    accountNumber;
    char   name[50];
    char   accountType[15];   // "Savings" or "Current"
    double balance;

public:
    // ---- Used when creating a brand-new account ----
    void createAccount() {
        cout << "\n----- Create New Account -----\n";
        cout << "Enter Account Number : ";
        cin  >> accountNumber;
        cin.ignore();
        cout << "Enter Name           : ";
        cin.getline(name, 50);
        cout << "Enter Account Type (Savings/Current) : ";
        cin.getline(accountType, 15);
        cout << "Enter Initial Deposit : ";
        cin  >> balance;

        cout << "\nAccount created successfully!\n";
    }

    // ---- Display account details ----
    void showAccount() const {
        cout << left;
        cout << setw(15) << accountNumber
             << setw(20) << name
             << setw(12) << accountType
             << setw(10) << fixed << setprecision(2) << balance << "\n";
    }

    // ---- Deposit money ----
    void deposit(double amount) {
        balance += amount;
    }

    // ---- Withdraw money (returns false if insufficient funds) ----
    bool withdraw(double amount) {
        if (amount > balance) {
            return false;
        }
        balance -= amount;
        return true;
    }

    // ---- Accessors ----
    int getAccountNumber() const { return accountNumber; }
    string getName() const { return string(name); }
    double getBalance() const { return balance; }

    // ---- Used when modifying an existing account's name/type ----
    void modify() {
        cout << "Enter new Name           : ";
        cin.getline(name, 50);
        cout << "Enter new Account Type   : ";
        cin.getline(accountType, 15);
    }
};

// ------------------------------------------------------------
// Bank Class
// Handles all file operations and higher-level banking logic.
// ------------------------------------------------------------
class Bank {
private:
    const char* fileName = "bank_data.dat";

public:
    void openAccount() {
        Account acc;
        acc.createAccount();

        fstream file(fileName, ios::app | ios::binary | ios::out);
        if (!file) {
            cout << "Error opening file!\n";
            return;
        }
        file.write(reinterpret_cast<char*>(&acc), sizeof(Account));
        file.close();
    }

    void depositMoney() {
        int accNo;
        double amount;
        cout << "\nEnter Account Number : ";
        cin  >> accNo;
        cout << "Enter Amount to Deposit : ";
        cin  >> amount;

        fstream file(fileName, ios::in | ios::out | ios::binary);
        if (!file) {
            cout << "No records found!\n";
            return;
        }

        Account acc;
        bool found = false;

        while (file.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() == accNo) {
                acc.deposit(amount);
                // move file pointer back to overwrite this record
                file.seekp(-static_cast<int>(sizeof(Account)), ios::cur);
                file.write(reinterpret_cast<char*>(&acc), sizeof(Account));
                found = true;
                cout << "Amount deposited successfully. New Balance: "
                     << fixed << setprecision(2) << acc.getBalance() << "\n";
                break;
            }
        }
        file.close();
        if (!found) cout << "Account not found!\n";
    }

    void withdrawMoney() {
        int accNo;
        double amount;
        cout << "\nEnter Account Number : ";
        cin  >> accNo;
        cout << "Enter Amount to Withdraw : ";
        cin  >> amount;

        fstream file(fileName, ios::in | ios::out | ios::binary);
        if (!file) {
            cout << "No records found!\n";
            return;
        }

        Account acc;
        bool found = false;

        while (file.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() == accNo) {
                found = true;
                if (acc.withdraw(amount)) {
                    file.seekp(-static_cast<int>(sizeof(Account)), ios::cur);
                    file.write(reinterpret_cast<char*>(&acc), sizeof(Account));
                    cout << "Amount withdrawn successfully. New Balance: "
                         << fixed << setprecision(2) << acc.getBalance() << "\n";
                } else {
                    cout << "Insufficient balance!\n";
                }
                break;
            }
        }
        file.close();
        if (!found) cout << "Account not found!\n";
    }

    void checkBalance() {
        int accNo;
        cout << "\nEnter Account Number : ";
        cin  >> accNo;

        fstream file(fileName, ios::in | ios::binary);
        if (!file) {
            cout << "No records found!\n";
            return;
        }

        Account acc;
        bool found = false;

        while (file.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() == accNo) {
                found = true;
                cout << "\nAccount Holder : " << acc.getName()
                     << "\nBalance        : " << fixed << setprecision(2)
                     << acc.getBalance() << "\n";
                break;
            }
        }
        file.close();
        if (!found) cout << "Account not found!\n";
    }

    void displayAll() {
        fstream file(fileName, ios::in | ios::binary);
        if (!file) {
            cout << "No records found!\n";
            return;
        }

        Account acc;
        cout << "\n" << left
             << setw(15) << "Acc No"
             << setw(20) << "Name"
             << setw(12) << "Type"
             << setw(10) << "Balance" << "\n";
        cout << "--------------------------------------------------------\n";

        bool any = false;
        while (file.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            acc.showAccount();
            any = true;
        }
        file.close();
        if (!any) cout << "No accounts to display.\n";
    }

    void modifyAccount() {
        int accNo;
        cout << "\nEnter Account Number to Modify : ";
        cin  >> accNo;
        cin.ignore();

        fstream file(fileName, ios::in | ios::out | ios::binary);
        if (!file) {
            cout << "No records found!\n";
            return;
        }

        Account acc;
        bool found = false;

        while (file.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() == accNo) {
                found = true;
                acc.modify();
                file.seekp(-static_cast<int>(sizeof(Account)), ios::cur);
                file.write(reinterpret_cast<char*>(&acc), sizeof(Account));
                cout << "Account updated successfully!\n";
                break;
            }
        }
        file.close();
        if (!found) cout << "Account not found!\n";
    }

    void deleteAccount() {
        int accNo;
        cout << "\nEnter Account Number to Delete : ";
        cin  >> accNo;

        fstream file(fileName, ios::in | ios::binary);
        if (!file) {
            cout << "No records found!\n";
            return;
        }

        fstream temp("temp.dat", ios::out | ios::binary);
        Account acc;
        bool found = false;

        while (file.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() != accNo) {
                temp.write(reinterpret_cast<char*>(&acc), sizeof(Account));
            } else {
                found = true;
            }
        }
        file.close();
        temp.close();

        remove(fileName);
        rename("temp.dat", fileName);

        if (found) cout << "Account deleted successfully!\n";
        else cout << "Account not found!\n";
    }
};

// ------------------------------------------------------------
// Utility: clears bad input from cin
// ------------------------------------------------------------
void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ------------------------------------------------------------
// Main Menu
// ------------------------------------------------------------
int main() {
    Bank bank;
    int choice;

    do {
        cout << "\n================ BANK MANAGEMENT SYSTEM ================\n";
        cout << "1. Create New Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Check Balance\n";
        cout << "5. Display All Accounts\n";
        cout << "6. Modify Account\n";
        cout << "7. Delete Account\n";
        cout << "8. Exit\n";
        cout << "==========================================================\n";
        cout << "Enter your choice : ";
        cin  >> choice;

        if (cin.fail()) {
            clearInput();
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: bank.openAccount();     break;
            case 2: bank.depositMoney();    break;
            case 3: bank.withdrawMoney();   break;
            case 4: bank.checkBalance();    break;
            case 5: bank.displayAll();      break;
            case 6: bank.modifyAccount();   break;
            case 7: bank.deleteAccount();   break;
            case 8: cout << "\nThank you for using the Bank Management System!\n"; break;
            default: cout << "Invalid choice! Try again.\n";
        }

    } while (choice != 8);

    return 0;
}