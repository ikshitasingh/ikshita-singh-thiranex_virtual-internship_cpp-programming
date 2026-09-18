/*
    Library Management System
    --------------------------
    A console-based Library Management System in C++ (OOP).

    Features:
      - Add / display books (structured via Book class)
      - Add / display members (Member class)
      - Issue and return books
      - Search books by title or author
      - Data persists between runs using plain text files
        (books.txt and members.txt) in the same folder as the executable.

    Compile:
        g++ -std=c++17 -o library library_management_system.cpp
    Run:
        ./library
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

using namespace std;

// ---------- Utility: CSV field handling ----------
static string escapeCsvField(const string& field) {
    string escaped;
    for (char c : field) {
        if (c == '"') {
            escaped += "\"\"";
        } else {
            escaped += c;
        }
    }
    return string("\"") + escaped + string("\"");
}

static vector<string> parseCsvLine(const string& line) {
    vector<string> fields;
    string field;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    field.push_back('"');
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                field.push_back(c);
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                fields.push_back(field);
                field.clear();
            } else {
                field.push_back(c);
            }
        }
    }
    fields.push_back(field);
    return fields;
}

// ---------- Book ----------
class Book {
public:
    int id;
    string title;
    string author;
    bool isIssued;
    int issuedToMemberId; // -1 if not issued

    Book() : id(0), isIssued(false), issuedToMemberId(-1) {}

    Book(int id_, const string& title_, const string& author_,
         bool isIssued_ = false, int issuedTo_ = -1)
        : id(id_), title(title_), author(author_),
          isIssued(isIssued_), issuedToMemberId(issuedTo_) {}

    string toFileLine() const {
        ostringstream oss;
        oss << id << "," << escapeCsvField(title) << "," << escapeCsvField(author) << ","
            << (isIssued ? 1 : 0) << "," << issuedToMemberId;
        return oss.str();
    }

    static Book fromFileLine(const string& line) {
        if (line.find('|') != string::npos) {
            stringstream ss(line);
            string idStr, title, author, issuedStr, issuedToStr;
            getline(ss, idStr, '|');
            getline(ss, title, '|');
            getline(ss, author, '|');
            getline(ss, issuedStr, '|');
            getline(ss, issuedToStr, '|');
            Book b;
            b.id = stoi(idStr);
            b.title = title;
            b.author = author;
            b.isIssued = (issuedStr == "1");
            b.issuedToMemberId = stoi(issuedToStr);
            return b;
        }

        vector<string> fields = parseCsvLine(line);
        if (fields.size() < 5) return Book();
        Book b;
        b.id = stoi(fields[0]);
        b.title = fields[1];
        b.author = fields[2];
        b.isIssued = (fields[3] == "1");
        b.issuedToMemberId = stoi(fields[4]);
        return b;
    }

    void display() const {
        cout << left
             << "ID: " << id
             << " | Title: " << title
             << " | Author: " << author
             << " | Status: " << (isIssued ? ("Issued (Member ID " + to_string(issuedToMemberId) + ")") : "Available")
             << "\n";
    }
};

// ---------- Member ----------
class Member {
public:
    int id;
    string name;
    string contact;

    Member() : id(0) {}
    Member(int id_, const string& name_, const string& contact_)
        : id(id_), name(name_), contact(contact_) {}

    string toFileLine() const {
        ostringstream oss;
        oss << id << "," << escapeCsvField(name) << "," << escapeCsvField(contact);
        return oss.str();
    }

    static Member fromFileLine(const string& line) {
        if (line.find('|') != string::npos) {
            stringstream ss(line);
            string idStr, name, contact;
            getline(ss, idStr, '|');
            getline(ss, name, '|');
            getline(ss, contact, '|');
            Member m;
            m.id = stoi(idStr);
            m.name = name;
            m.contact = contact;
            return m;
        }

        vector<string> fields = parseCsvLine(line);
        if (fields.size() < 3) return Member();
        Member m;
        m.id = stoi(fields[0]);
        m.name = fields[1];
        m.contact = fields[2];
        return m;
    }

    void display() const {
        cout << "ID: " << id << " | Name: " << name << " | Contact: " << contact << "\n";
    }
};

// ---------- Library (manages everything) ----------
class Library {
private:
    vector<Book> books;
    vector<Member> members;
    int nextBookId;
    int nextMemberId;

    const string booksFile = "books.csv";
    const string membersFile = "members.csv";
    const string legacyBooksFile = "books.txt";
    const string legacyMembersFile = "members.txt";

public:
    Library() : nextBookId(1), nextMemberId(1) {
        loadFromFile();
    }

    // ---------- Persistence ----------
    void loadFromFile() {
        ifstream bIn(booksFile);
        if (!bIn) {
            bIn.open(legacyBooksFile);
        }
        if (bIn) {
            string line;
            while (getline(bIn, line)) {
                if (line.empty()) continue;
                if (line.rfind("ID,", 0) == 0) continue;
                if (line.rfind("ID|", 0) == 0) continue;
                Book b = Book::fromFileLine(line);
                if (b.id == 0) continue;
                books.push_back(b);
                nextBookId = max(nextBookId, b.id + 1);
            }
        }

        ifstream mIn(membersFile);
        if (!mIn) {
            mIn.open(legacyMembersFile);
        }
        if (mIn) {
            string line;
            while (getline(mIn, line)) {
                if (line.empty()) continue;
                if (line.rfind("ID,", 0) == 0) continue;
                if (line.rfind("ID|", 0) == 0) continue;
                Member m = Member::fromFileLine(line);
                if (m.id == 0) continue;
                members.push_back(m);
                nextMemberId = max(nextMemberId, m.id + 1);
            }
        }
    }

    void saveToFile() const {
        ofstream bOut(booksFile, ios::trunc);
        bOut << "ID,Title,Author,IsIssued,IssuedToMemberId\n";
        for (const auto& b : books) bOut << b.toFileLine() << "\n";

        ofstream mOut(membersFile, ios::trunc);
        mOut << "ID,Name,Contact\n";
        for (const auto& m : members) mOut << m.toFileLine() << "\n";
    }

    // ---------- Book operations ----------
    void addBook(const string& title, const string& author) {
        Book b(nextBookId++, title, author);
        books.push_back(b);
        cout << "Book added successfully with ID " << b.id << ".\n";
    }

    void displayAllBooks() const {
        if (books.empty()) {
            cout << "No books in the library.\n";
            return;
        }
        cout << "\n--- All Books ---\n";
        for (const auto& b : books) b.display();
    }

    Book* findBookById(int id) {
        for (auto& b : books) if (b.id == id) return &b;
        return nullptr;
    }

    void searchByTitle(const string& query) const {
        cout << "\n--- Search Results (Title contains \"" << query << "\") ---\n";
        bool found = false;
        string q = query;
        transform(q.begin(), q.end(), q.begin(), ::tolower);
        for (const auto& b : books) {
            string t = b.title;
            transform(t.begin(), t.end(), t.begin(), ::tolower);
            if (t.find(q) != string::npos) {
                b.display();
                found = true;
            }
        }
        if (!found) cout << "No books found.\n";
    }

    void searchByAuthor(const string& query) const {
        cout << "\n--- Search Results (Author contains \"" << query << "\") ---\n";
        bool found = false;
        string q = query;
        transform(q.begin(), q.end(), q.begin(), ::tolower);
        for (const auto& b : books) {
            string a = b.author;
            transform(a.begin(), a.end(), a.begin(), ::tolower);
            if (a.find(q) != string::npos) {
                b.display();
                found = true;
            }
        }
        if (!found) cout << "No books found.\n";
    }

    void deleteBook(int bookId) {
        Book* b = findBookById(bookId);
        if (!b) { cout << "Book ID not found.\n"; return; }
        if (b->isIssued) {
            cout << "Cannot delete \"" << b->title << "\" - it is currently issued. "
                 << "Please return it first.\n";
            return;
        }
        string title = b->title;
        books.erase(remove_if(books.begin(), books.end(),
                               [bookId](const Book& bk) { return bk.id == bookId; }),
                    books.end());
        cout << "Book \"" << title << "\" (ID " << bookId << ") deleted successfully.\n";
    }

    // ---------- Member operations ----------
    void addMember(const string& name, const string& contact) {
        Member m(nextMemberId++, name, contact);
        members.push_back(m);
        cout << "Member added successfully with ID " << m.id << ".\n";
    }

    void displayAllMembers() const {
        if (members.empty()) {
            cout << "No members registered.\n";
            return;
        }
        cout << "\n--- All Members ---\n";
        for (const auto& m : members) m.display();
    }

    Member* findMemberById(int id) {
        for (auto& m : members) if (m.id == id) return &m;
        return nullptr;
    }

    // ---------- Issue / Return ----------
    void issueBook(int bookId, int memberId) {
        Book* b = findBookById(bookId);
        if (!b) { cout << "Book ID not found.\n"; return; }
        if (b->isIssued) { cout << "Book is already issued.\n"; return; }

        Member* m = findMemberById(memberId);
        if (!m) { cout << "Member ID not found.\n"; return; }

        b->isIssued = true;
        b->issuedToMemberId = memberId;
        cout << "Book \"" << b->title << "\" issued to " << m->name << ".\n";
    }

    void returnBook(int bookId) {
        Book* b = findBookById(bookId);
        if (!b) { cout << "Book ID not found.\n"; return; }
        if (!b->isIssued) { cout << "Book was not issued.\n"; return; }

        b->isIssued = false;
        b->issuedToMemberId = -1;
        cout << "Book \"" << b->title << "\" has been returned.\n";
    }
};

// ---------- Input helpers ----------
static int readInt(const string& prompt) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "Invalid input. Please enter a number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

static string readLine(const string& prompt) {
    cout << prompt;
    string line;
    getline(cin, line);
    return line;
}

// ---------- Menu ----------
static void printMenu() {
    cout << "\n===== Library Management System =====\n";
    cout << "1. Add Book\n";
    cout << "2. Add Member\n";
    cout << "3. Issue Book\n";
    cout << "4. Return Book\n";
    cout << "5. Search Book by Title\n";
    cout << "6. Search Book by Author\n";
    cout << "7. Display All Books\n";
    cout << "8. Display All Members\n";
    cout << "9. Delete Book\n";
    cout << "10. Save and Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    Library library;
    bool running = true;

    cout << "Library Management System started. Existing data (if any) has been loaded.\n";

    while (running) {
        printMenu();
        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Try again.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                string title = readLine("Enter book title: ");
                string author = readLine("Enter book author: ");
                library.addBook(title, author);
                break;
            }
            case 2: {
                string name = readLine("Enter member name: ");
                string contact = readLine("Enter member contact (phone/email): ");
                library.addMember(name, contact);
                break;
            }
            case 3: {
                int bookId = readInt("Enter Book ID to issue: ");
                int memberId = readInt("Enter Member ID: ");
                library.issueBook(bookId, memberId);
                break;
            }
            case 4: {
                int bookId = readInt("Enter Book ID to return: ");
                library.returnBook(bookId);
                break;
            }
            case 5: {
                string q = readLine("Enter title (or part of it) to search: ");
                library.searchByTitle(q);
                break;
            }
            case 6: {
                string q = readLine("Enter author (or part of it) to search: ");
                library.searchByAuthor(q);
                break;
            }
            case 7:
                library.displayAllBooks();
                break;
            case 8:
                library.displayAllMembers();
                break;
            case 9: {
                int bookId = readInt("Enter Book ID to delete: ");
                string confirm = readLine("Are you sure you want to delete this book? (y/n): ");
                if (!confirm.empty() && (confirm[0] == 'y' || confirm[0] == 'Y')) {
                    library.deleteBook(bookId);
                } else {
                    cout << "Deletion cancelled.\n";
                }
                break;
            }
            case 10:
                library.saveToFile();
                cout << "Data saved. Goodbye!\n";
                running = false;
                break;
            default:
                cout << "Invalid choice. Please select 1-10.\n";
        }

        // Auto-save after every operation so data isn't lost on a crash.
        if (running) library.saveToFile();
    }

    return 0;
}