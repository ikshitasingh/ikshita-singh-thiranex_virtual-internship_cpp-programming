/*
    STUDENT MANAGEMENT SYSTEM (CSV version)
    -----------------------------------------
    A console-based C++ program that manages student records
    using file handling and a menu-driven interface.

    Data is stored in a plain text CSV file called "students.csv"
    in the same folder as the program, so you can open it directly
    in Notepad, Excel, or Google Sheets and it will look correct.

    File format (one student per line):
        ID,Name,Age,Course,Marks

    Note: Because commas separate the fields, please avoid typing
    commas inside Name or Course when entering data.

    Features:
      1. Add a student
      2. Display all students
      3. Update a student
      4. Delete a student
      5. Search a student
      0. Exit
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

using namespace std;

const char* FILENAME = "students.csv";
const string CSV_HEADER = "ID,Name,Age,Course,Marks";

// ---------------------------------------------------------
// 1. DATA STRUCTURE
// ---------------------------------------------------------
// One Student record. Using std::string here (instead of fixed
// char arrays) because it's simpler to work with when reading
// and writing comma-separated text.
struct Student {
    int id;
    string name;
    int age;
    string course;
    float marks;
};

// ---------------------------------------------------------
// Helper: clear bad input from cin
// ---------------------------------------------------------
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// ---------------------------------------------------------
// Helper: split a CSV line into its comma-separated fields
// ---------------------------------------------------------
vector<string> splitCSV(const string& line) {
    vector<string> fields;
    stringstream ss(line);
    string field;
    while (getline(ss, field, ',')) {
        fields.push_back(field);
    }
    return fields;
}

// ---------------------------------------------------------
// Helper: ensure the CSV file contains a header row. If the
// file exists but is missing the header, rewrite it with
// the header inserted at the top.
// ---------------------------------------------------------
void ensureCsvHeader() {
    ifstream inFile(FILENAME);
    if (!inFile) {
        return; // file doesn't exist yet
    }

    string firstLine;
    if (!getline(inFile, firstLine)) {
        // empty file -> write header
        inFile.close();
        ofstream outFile(FILENAME, ios::trunc);
        outFile << CSV_HEADER << "\n";
        return;
    }

    if (firstLine == CSV_HEADER) {
        return; // header already present
    }

    vector<string> lines;
    lines.push_back(firstLine);
    while (getline(inFile, firstLine)) {
        lines.push_back(firstLine);
    }
    inFile.close();

    ofstream outFile(FILENAME, ios::trunc);
    outFile << CSV_HEADER << "\n";
    for (const auto& line : lines) {
        if (!line.empty()) {
            outFile << line << "\n";
        }
    }
}

// ---------------------------------------------------------
// Helper: read every student record from the CSV file into
// a list we can work with in memory. Returns an empty list
// if the file doesn't exist yet.
// ---------------------------------------------------------
vector<Student> loadAll() {
    vector<Student> students;
    ifstream inFile(FILENAME);
    if (!inFile) {
        return students; // file doesn't exist yet -> no records
    }

    string line;
    bool firstLine = true;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        if (firstLine && line == CSV_HEADER) {
            firstLine = false;
            continue;
        }
        firstLine = false;

        vector<string> f = splitCSV(line);
        if (f.size() < 5) continue; // skip malformed lines

        Student s;
        s.id = stoi(f[0]);
        s.name = f[1];
        s.age = stoi(f[2]);
        s.course = f[3];
        s.marks = stof(f[4]);
        students.push_back(s);
    }
    inFile.close();
    return students;
}

// ---------------------------------------------------------
// Helper: write the full list of students back to the CSV
// file, overwriting whatever was there before. Used after
// any update or delete, since CSV lines can differ in length
// and can't easily be edited in place.
// ---------------------------------------------------------
void saveAll(const vector<Student>& students) {
    ofstream outFile(FILENAME, ios::trunc);
    outFile << CSV_HEADER << "\n";
    for (const auto& s : students) {
        outFile << s.id << "," << s.name << "," << s.age << ","
                << s.course << "," << s.marks << "\n";
    }
    outFile.close();
}

// ---------------------------------------------------------
// 2. ADD STUDENT
// ---------------------------------------------------------
void addStudent() {
    Student s;

    cout << "\n--- Add New Student ---\n";

    cout << "Enter ID: ";
    while (!(cin >> s.id)) {
        cout << "Invalid input. Enter a numeric ID: ";
        clearInputBuffer();
    }
    clearInputBuffer();

    cout << "Enter Name: ";
    getline(cin, s.name);

    cout << "Enter Age: ";
    while (!(cin >> s.age)) {
        cout << "Invalid input. Enter a numeric age: ";
        clearInputBuffer();
    }
    clearInputBuffer();

    cout << "Enter Course: ";
    getline(cin, s.course);

    cout << "Enter Marks: ";
    while (!(cin >> s.marks)) {
        cout << "Invalid input. Enter numeric marks: ";
        clearInputBuffer();
    }
    clearInputBuffer();

    // Ensure the CSV file has a header before appending this record.
    ifstream inFile(FILENAME);
    bool needHeader = false;
    if (!inFile) {
        needHeader = true;
    } else {
        string firstLine;
        if (!getline(inFile, firstLine) || firstLine.empty()) {
            needHeader = true;
        } else if (firstLine != CSV_HEADER) {
            inFile.close();
            ensureCsvHeader();
        }
    }

    ofstream outFile(FILENAME, ios::app);
    if (!outFile) {
        cout << "Error opening file!\n";
        return;
    }
    if (needHeader) {
        outFile << CSV_HEADER << "\n";
    }
    outFile << s.id << "," << s.name << "," << s.age << ","
            << s.course << "," << s.marks << "\n";
    outFile.close();

    cout << "Student added successfully!\n";
}

// ---------------------------------------------------------
// 3. DISPLAY ALL STUDENTS
// ---------------------------------------------------------
void displayAll() {
    vector<Student> students = loadAll();

    if (students.empty()) {
        cout << "\nNo records to display.\n";
        return;
    }

    cout << "\n--- All Student Records ---\n";
    cout << left << setw(6) << "ID" << setw(20) << "Name"
         << setw(6) << "Age" << setw(20) << "Course"
         << setw(8) << "Marks" << "\n";
    cout << string(60, '-') << "\n";

    for (const auto& s : students) {
        cout << left << setw(6) << s.id << setw(20) << s.name
             << setw(6) << s.age << setw(20) << s.course
             << setw(8) << s.marks << "\n";
    }
}

// ---------------------------------------------------------
// 4. UPDATE STUDENT
// ---------------------------------------------------------
void updateStudent() {
    int targetId;
    cout << "\nEnter the ID of the student to update: ";
    while (!(cin >> targetId)) {
        cout << "Invalid input. Enter a numeric ID: ";
        clearInputBuffer();
    }
    clearInputBuffer();

    vector<Student> students = loadAll();
    bool found = false;

    for (auto& s : students) {
        if (s.id == targetId) {
            found = true;

            cout << "Current Name: " << s.name << " | Enter new name: ";
            getline(cin, s.name);

            cout << "Current Age: " << s.age << " | Enter new age: ";
            while (!(cin >> s.age)) {
                cout << "Invalid input. Enter a numeric age: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            cout << "Current Course: " << s.course << " | Enter new course: ";
            getline(cin, s.course);

            cout << "Current Marks: " << s.marks << " | Enter new marks: ";
            while (!(cin >> s.marks)) {
                cout << "Invalid input. Enter numeric marks: ";
                clearInputBuffer();
            }
            clearInputBuffer();

            break;
        }
    }

    if (found) {
        saveAll(students); // rewrite the whole file with the updated data
        cout << "Student updated successfully!\n";
    } else {
        cout << "Student with ID " << targetId << " not found.\n";
    }
}

// ---------------------------------------------------------
// 5. DELETE STUDENT
// ---------------------------------------------------------
void deleteStudent() {
    int targetId;
    cout << "\nEnter the ID of the student to delete: ";
    while (!(cin >> targetId)) {
        cout << "Invalid input. Enter a numeric ID: ";
        clearInputBuffer();
    }
    clearInputBuffer();

    vector<Student> students = loadAll();
    vector<Student> remaining;
    bool found = false;

    for (const auto& s : students) {
        if (s.id == targetId) {
            found = true; // skip this one -> effectively deletes it
        } else {
            remaining.push_back(s);
        }
    }

    if (found) {
        saveAll(remaining); // rewrite the file without the deleted record
        cout << "Student deleted successfully!\n";
    } else {
        cout << "Student with ID " << targetId << " not found.\n";
    }
}

// ---------------------------------------------------------
// 6. SEARCH STUDENT
// ---------------------------------------------------------
void searchStudent() {
    int targetId;
    cout << "\nEnter the ID of the student to search: ";
    while (!(cin >> targetId)) {
        cout << "Invalid input. Enter a numeric ID: ";
        clearInputBuffer();
    }
    clearInputBuffer();

    vector<Student> students = loadAll();

    for (const auto& s : students) {
        if (s.id == targetId) {
            cout << "\n--- Student Found ---\n";
            cout << "ID: " << s.id << "\n";
            cout << "Name: " << s.name << "\n";
            cout << "Age: " << s.age << "\n";
            cout << "Course: " << s.course << "\n";
            cout << "Marks: " << s.marks << "\n";
            return;
        }
    }

    cout << "Student with ID " << targetId << " not found.\n";
}

// ---------------------------------------------------------
// 7. MAIN MENU
// ---------------------------------------------------------
int main() {
    int choice;

    do {
        cout << "\n========= STUDENT MANAGEMENT SYSTEM =========\n";
        cout << "1. Add Student\n";
        cout << "2. Display All Students\n";
        cout << "3. Update Student\n";
        cout << "4. Delete Student\n";
        cout << "5. Search Student\n";
        cout << "0. Exit\n";
        cout << "===============================================\n";
        cout << "Enter your choice: ";

        while (!(cin >> choice)) {
            cout << "Invalid input. Enter a number: ";
            clearInputBuffer();
        }
        clearInputBuffer();

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayAll(); break;
            case 3: updateStudent(); break;
            case 4: deleteStudent(); break;
            case 5: searchStudent(); break;
            case 0: cout << "Exiting program. Goodbye!\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }

    } while (choice != 0);

    return 0;
}