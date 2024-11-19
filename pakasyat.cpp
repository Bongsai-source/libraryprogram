
#include <iostream> // Enables input and output operations (e.g., cout, cin)
#include <string> // Provides support for string manipulation
#include <vector> // Allows the use of dynamic arrays (vectors)
#include <ctime> // Provides functions to handle and manipulate time/date
#include <iomanip> // Provides utilities like std::setw for formatting output
#include <limits> // Enables use of numeric_limits to handle data type limits
#include <sstream> // Allows string streams for text processing and conversions
#include <algorithm> // Provides functions for operations like sort, transform
#include <fstream> // Enables file operations (e.g., reading/writing files)

using namespace std;

class Book {
public:
    int id;
    string title;
    string author;
    string category;
    bool isAvailable;
    // Initializes a book with given details; defaults to available
    Book(int bookID, string bookTitle, string bookAuthor, string bookCategory, bool available = true)
        : id(bookID), title(bookTitle), author(bookAuthor), category(bookCategory), isAvailable(available) {}
};

class Member {
public:
    int id;
    string name;
    int booksBorrowed;
    // Initializes a member with an ID, name, and borrowed books; defaults to 0
    Member(int memberID, string memberName, int borrowed = 0) : id(memberID), name(memberName), booksBorrowed(borrowed) {}
};

class Library {
private:
    vector<Book> books;
    vector<Member> members;
    //categories for books
    const vector<string> categories = {"Fiction", "Non-Fiction", "Science", "Biography", "History", "Children"};
    const int BORROW_PERIOD = 3; // Days until the book should be returned
    const int FINE_PER_DAY = 5;  // Fine per overdue day

    // Convert a date string (YYYY-MM-DD) to time_t
    time_t parseDate(const string& dateStr) {
        struct tm tm = {0};
        tm.tm_year = stoi(dateStr.substr(0, 4)) - 1900;
        tm.tm_mon = stoi(dateStr.substr(5, 2)) - 1;
        tm.tm_mday = stoi(dateStr.substr(8, 2));
        return mktime(&tm);
    }

    // Convert a string to lowercase for case-insensitive comparison
    string toLowercase(const string& str) {
        string lowerStr = str;
        transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
        return lowerStr;
    }

public:
    //loads books and members from csv files
    Library() {
        loadBooksFromCSV("books.csv");
        loadMembersFromCSV("members.csv");
    }
    //ensures valid integer input from the user
    int getValidIntegerInput(const string& prompt) {
        int input;
        while (true) {
            cout << prompt;
            cin >> input;
            if (cin.fail()) {
                // Handle invalid input
                cout << "Invalid input. Please enter a number." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            } else {
                return input;
            }
        }
    }
    //displays the list of available book categories
    void displayCategories() {
        cout << "Available Categories:" << endl;
        for (size_t i = 0; i < categories.size(); ++i) {
            cout << i + 1 << ". " << categories[i] << endl;
        }
    }
    //adds a new book to the library
    void addBook(int id, string title, string author, int categoryIndex) {
        // Check if a book with the same ID already exists
        for (const auto& book : books) {
            if (book.id == id) {
                cout << "A book with ID " << id << " already exists. Please choose a different ID." << endl;
                return;
            }
        }

        if (categoryIndex < 1 || categoryIndex > categories.size()) {
            cout << "Invalid category selection." << endl;
            return;
        }
        // Add the new book to the library's collection
        string category = categories[categoryIndex - 1];
        books.push_back(Book(id, title, author, category));
        cout << "Book added successfully!" << endl;
    }
     //deletes a book by its specific ID
    void deleteBook(int id) {
        for (size_t i = 0; i < books.size(); ++i) {
            // Find the book with the specified ID
            if (books[i].id == id) {
                books.erase(books.begin() + i);
                cout << "Book deleted successfully!" << endl;
                return;
            }
        }
        cout << "Book not found!" << endl;
    }
    //shows the books that are available and uses a format to look clear and presentable
    void displayAvailableBooks() {
        bool anyAvailable = false;
        cout << left << setw(5) << "ID" << setw(25) << "Title" << setw(20) << "Author" << setw(15) << "Category" << endl;
        cout << string(65, '-') << endl;

        for (const auto& book : books) {
            // Display details of available books
            if (book.isAvailable) {
                cout << left << setw(5) << book.id << setw(25) << book.title << setw(20) << book.author << setw(15) << book.category << endl;
                anyAvailable = true;
            }
        }
        if (!anyAvailable) {
            cout << "There are no books available to borrow." << endl;
        }
    }
    //checks if there are any books available to borrow
    bool hasAvailableBooks() {
        for (const auto& book : books) {
            if (book.isAvailable) {
                return true;
            }
        }
        return false;
    }
    //displays all books in the library, including their availability
    void viewBooks() {
        if (books.empty()) {
            cout << "There are no books in the library." << endl;
        } else {
            cout << left << setw(5) << "ID" << setw(25) << "Title" << setw(20) << "Author" << setw(15) << "Category" << setw(12) << "Available" << endl;
            cout << string(77, '-') << endl;

            for (const auto& book : books) {
                cout << left << setw(5) << book.id << setw(25) << book.title << setw(20) << book.author << setw(15) << book.category << setw(12) << (book.isAvailable ? "Yes" : "No") << endl;
            }
        }
    }
    // Registers new members
    void registerMember(int id, string name) {
        for (const auto& member : members) {
            // Loop through all members to check if the ID already exists
            if (member.id == id) {
                cout << "A member with ID " << id << " already exists. Please choose a different ID." << endl;
                return;
            }
        }
        members.push_back(Member(id, name));
        cout << "Member registered successfully!" << endl;
    }
    //Displays all registered members and amount of books they've borrowed
    void viewMembers() {
        if (members.empty()) {
            cout << "There are no registered members." << endl;
        } else {
            // Print table headers
            cout << left << setw(5) << "ID" << setw(25) << "Name" << setw(15) << "Books Borrowed" << endl;
            cout << string(45, '-') << endl;

            for (const auto& member : members) {
                cout << left << setw(5) << member.id << setw(25) << member.name << setw(15) << member.booksBorrowed << endl;
            }
        }
    }
    //Lets a member borrow a book if it's available
    void borrowBook(int memberID, int bookID) {
        for (auto& book : books) {
            // Loop through all books to find the specified book ID
            if (book.id == bookID && book.isAvailable) {
                for (auto& member : members) {
                    if (member.id == memberID) {
                        book.isAvailable = false;
                        member.booksBorrowed++;
                        cout << "Book borrowed successfully!" << endl;
                        cout << "Please return this book within " << BORROW_PERIOD << " days to avoid fines." << endl;
                        return;
                    }
                }
                cout << "Member not found!" << endl;
                return;
            }
        }
        cout << "Book is not available or does not exist!" << endl;
    }
    // Processes the return of a borrowed book and calculates fines if overdue
    void returnBook(int memberID, int bookID) {
        for (auto& book : books) {
            if (book.id == bookID && !book.isAvailable) {
                for (auto& member : members) {
                    if (member.id == memberID) {
                        string borrowDateStr;
                        cout << "Enter the borrow date (YYYY-MM-DD): ";
                        cin >> borrowDateStr;
                        time_t borrowDate = parseDate(borrowDateStr);
                        time_t returnDate = time(0);

                        int daysBorrowed = (returnDate - borrowDate) / (24 * 60 * 60);
                        int overdueDays = max(0, daysBorrowed - BORROW_PERIOD);
                        int fine = overdueDays * FINE_PER_DAY;

                        book.isAvailable = true;
                        member.booksBorrowed--;

                        cout << "Book returned successfully!" << endl;
                        if (overdueDays > 0) {
                            cout << "Overdue by " << overdueDays << " day(s). Fine: $" << fine << endl;
                        } else {
                            cout << "Book returned on time. No fine." << endl;
                        }
                        return;
                    }
                }
                cout << "Member not found!" << endl;
                return;
            }
        }
        cout << "Book is not borrowed!" << endl;
    }
    // Searches for books by ID, title, author, or category using a keyword
    void searchBooks(const string& keyword) {
        bool found = false;
        string lowerKeyword = toLowercase(keyword);
        // Print search results header
        cout << "Search results for \"" << keyword << "\":" << endl;
        cout << left << setw(5) << "ID" << setw(25) << "Title" << setw(20) << "Author" << setw(15) << "Category" << setw(12) << "Available" << endl;
        cout << string(77, '-') << endl;

        for (const auto& book : books) {
            // Loop through all books and check if the keyword matches any field
            string lowerTitle = toLowercase(book.title);
            string lowerAuthor = toLowercase(book.author);
            string lowerCategory = toLowercase(book.category);

            if (to_string(book.id) == lowerKeyword ||
                lowerTitle.find(lowerKeyword) != string::npos ||
                lowerAuthor.find(lowerKeyword) != string::npos ||
                lowerCategory.find(lowerKeyword) != string::npos) {
                
                cout << left << setw(5) << book.id << setw(25) << book.title << setw(20) << book.author << setw(15) << book.category << setw(12) << (book.isAvailable ? "Yes" : "No") << endl;
                found = true;
            }
        }

        if (!found) {
            cout << "No books found matching the keyword \"" << keyword << "\"." << endl;
        }
    }
    // Loads book data from a CSV file
    void loadBooksFromCSV(const string& filename) {
        ifstream file(filename);
        if (!file) return;

        string line;
        // Reads each line from the file
        while (getline(file, line)) {
            stringstream ss(line);
            int id;
            string title, author, category;
            bool isAvailable;
            char delim;
            // Extract book details from the line using string stream
            ss >> id >> delim;
            getline(ss, title, ',');
            getline(ss, author, ',');
            getline(ss, category, ',');
            ss >> isAvailable;
            // Add the book to the collection
            books.emplace_back(id, title, author, category, isAvailable);
        }
        file.close();
    }
    // Loads member data from a CSV file
    void loadMembersFromCSV(const string& filename) {
        ifstream file(filename);
        if (!file) return;

        string line;
        // Reads each line from the file
        while (getline(file, line)) {
            stringstream ss(line);
            int id;
            string name;
            int booksBorrowed;
            char delim;
            // Extract member details from the line using string stream
            ss >> id >> delim;
            getline(ss, name, ',');
            ss >> booksBorrowed;
            // Add the member to the collection
            members.emplace_back(id, name, booksBorrowed);
        }
        file.close();
    }
    // Saves the current book collection to a CSV file
    void saveBooksToCSV(const string& filename) {
        ofstream file(filename);
        for (const auto& book : books) {
            // Write each book's details to the file in CSV format
            file << book.id << "," << book.title << "," << book.author << "," << book.category << "," << book.isAvailable << endl;
        }
        file.close();
    }
    // Saves the current member collection to a CSV file
    void saveMembersToCSV(const string& filename) {
        ofstream file(filename);
        for (const auto& member : members) {
            // Write each member's details to the file in CSV format
            file << member.id << "," << member.name << "," << member.booksBorrowed << endl;
        }
        file.close();
    }
    // Destructor saves the current state of books and members to CSV files
    ~Library() {
        saveBooksToCSV("books.csv");
        saveMembersToCSV("members.csv");
    }
};

int main() {
    Library library;
    int choice;
    
    while (true) {
        // Main menu loop for the Library Management System
        cout << "\nLibrary Management System" << endl;
        cout << "1. Add Book" << endl;
        cout << "2. Delete Book" << endl;
        cout << "3. View Books" << endl;
        cout << "4. Register Member" << endl;
        cout << "5. View Members" << endl;
        cout << "6. Borrow Book" << endl;
        cout << "7. Return Book" << endl;
        cout << "8. Search Books" << endl;
        cout << "0. Exit" << endl;
        // Prompt the user for a menu choice
        choice = library.getValidIntegerInput("Enter your choice: ");

        switch (choice) {
            case 1: {
                int id = library.getValidIntegerInput("Enter Book ID: ");
                string title, author;
                cout << "Enter Book Title: ";
                cin.ignore();
                getline(cin, title);
                cout << "Enter Book Author: ";
                getline(cin, author);

                library.displayCategories();
                int categoryIndex = library.getValidIntegerInput("Select a category by number: ");
                library.addBook(id, title, author, categoryIndex);
                break;
            }
            case 2: {
                library.displayAvailableBooks();
                int id = library.getValidIntegerInput("Enter Book ID to delete: ");
                library.deleteBook(id);
                break;
            }
            case 3:
                library.viewBooks();
                break;
            case 4: {
                int id = library.getValidIntegerInput("Enter Member ID: ");
                string name;
                cout << "Enter Member Name: ";
                cin.ignore();
                getline(cin, name);
                library.registerMember(id, name);
                break;
            }
            case 5:
                library.viewMembers();
                break;
            case 6: {
                int memberID = library.getValidIntegerInput("Enter Member ID: ");
                library.displayAvailableBooks();
                int bookID = library.getValidIntegerInput("Enter Book ID to borrow: ");
                library.borrowBook(memberID, bookID);
                break;
            }
            case 7: {
                int memberID = library.getValidIntegerInput("Enter Member ID: ");
                int bookID = library.getValidIntegerInput("Enter Book ID: ");
                library.returnBook(memberID, bookID);
                break;
            }
            case 8: {
                string keyword;
                cout << "Enter keyword to search (ID, title, author, or category): ";
                cin.ignore();
                getline(cin, keyword);
                library.searchBooks(keyword);
                break;
            }
            case 0:
                cout << "Exiting the system..." << endl;
                return 0;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    }
    return 0;
}
