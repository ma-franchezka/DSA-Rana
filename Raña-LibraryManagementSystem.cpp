#include <iostream>
#include <fstream> // for file manipulation
#include <vector> // for flexible array
using namespace std;

// Book class
class Book {
public:
    string title, author, isbn;
    bool availability;

    Book(string t, string a, string i, bool avail = true)
        : title(t), author(a), isbn(i), availability(avail) {}

    void display() const {
        cout<<"Title: "<<title<<", Author: "<<author
             <<", ISBN: "<<isbn<<", Status: "
             << (availability ? "Available" : "Borrowed") <<endl;
    }
};

// LibraryUser class
class LibraryUser {
public:
    string userID, name;
    vector<string> borrowedISBN;

    LibraryUser(string id, string n) : userID(id), name(n) {}

    void borrowBook(const string& isbn) {
        for (const auto& b : borrowedISBN)
            if (b == isbn) return;
        borrowedISBN.push_back(isbn);
    }

    void returnBook(const string& isbn) {
        vector<string> updated;
        for (const auto& b : borrowedISBN)
            if (b != isbn)
                updated.push_back(b);
        borrowedISBN = updated;
    }

    void displayBorrowedBooks() const {
        cout << "User ID: " << userID << ", Name: " << name << "\nBorrowed Books:\n";
        for (const auto& isbn : borrowedISBN)
            cout << " - " << isbn << endl;
    }
};

// Library class
class Library {
private:
    vector<Book> books;
    vector<LibraryUser> users;
    const string booksFile = "BooksFile.txt"; // the file where book details will be stored
    const string usersFile = "UsersFile.txt"; // the file were user details will be stored

    Book* findBookByISBN(const string& isbn) {
        for (size_t i = 0; i < books.size(); ++i)
            if (books[i].isbn == isbn)
                return &books[i];
        return nullptr;
    }

    LibraryUser* findUserByID(const string& id) {
        for (size_t i = 0; i < users.size(); ++i)
            if (users[i].userID == id)
                return &users[i];
        return nullptr;
    }

public:
    Library() {
        loadBooksFromFile();
        loadUsersFromFile();
    }

    ~Library() {
        saveBooksToFile();
        saveUsersToFile();
    }

    void addBook(const string& title, const string& author, const string& isbn) {
        books.push_back(Book(title, author, isbn));
        cout << "Book added successfully.\n";
    }

    void removeBook(const string& isbn) {
        vector<Book> updated;
        for (auto& b : books)
            if (b.isbn != isbn)
                updated.push_back(b);
        books = updated;
        cout << "Book removed (if it existed).\n";
    }

    void registerUser(const string& id, const string& name) {
        users.push_back(LibraryUser(id, name));
        cout << "User registered successfully.\n";
    }

    void removeUser(const string& id) {
        vector<LibraryUser> updated;
        for (auto& u : users)
            if (u.userID != id)
                updated.push_back(u);
        users = updated;
        cout << "User removed (if existed).\n";
    }

    void displayBooks() const {
        cout<< "\n--- All Books ---\n";
        for (const auto& b : books)
            b.display();
        cout<< "\n-----------------\n";
    }

    void displayUsers() const {
        cout << "\n--- All Users ---\n";
        for (const auto& u : users)
            u.displayBorrowedBooks();
            cout<<endl;
        cout<< "\n-----------------\n";
    }

    void borrowBook(const string& userID, const string& isbn) {
        LibraryUser* user = findUserByID(userID);
        Book* book = findBookByISBN(isbn);

        if (!user || !book) {
            cout << "User or Book not found.\n";
            return;
        }

        if (!book->availability) {
            cout << "Book is already borrowed.\n";
            return;
        }

        user->borrowBook(isbn);
        book->availability = false;
        cout << "Book borrowed successfully.\n";
    }

    void returnBook(const string& userID, const string& isbn) {
        LibraryUser* user = findUserByID(userID);
        Book* book = findBookByISBN(isbn);

        if (!user || !book) {
            cout << "User or Book not found.\n";
            return;
        }

        user->returnBook(isbn);
        book->availability = true;
        cout << "Book returned successfully.\n";
    }

    void loadBooksFromFile() {
        ifstream file(booksFile);
        string title, author, isbn, status;
        while (getline(file, title, '|')) {
            getline(file, author, '|');
            getline(file, isbn, '|');
            getline(file, status);
            books.push_back(Book(title, author, isbn, status == "1"));
        }
    }

    void saveBooksToFile() {
        ofstream file(booksFile);
        for (const auto& b : books)
            file << b.title << " | " << b.author << " | " << b.isbn << " | "
                 << (b.availability ? "1" : "0") << "\n";
    }

    void loadUsersFromFile() {
        ifstream file(usersFile);
        string line;
        while (getline(file, line)) {
            int p1 = line.find('|');
            int p2 = line.find('|', p1 + 1);
            if (p1 == string::npos || p2 == string::npos) continue;
            string id = line.substr(0, p1);
            string name = line.substr(p1 + 1, p2 - p1 - 1);
            string rest = line.substr(p2 + 1);
            LibraryUser user(id, name);
            string temp;
            for (char ch : rest) {
                if (ch == ',') {
                    if (!temp.empty()) user.borrowedISBN.push_back(temp);
                    temp = "";
                } else {
                    temp += ch;
                }
            }
            if (!temp.empty()) user.borrowedISBN.push_back(temp);
            users.push_back(user);
        }
    }

    void saveUsersToFile() {
        ofstream file(usersFile);
        for (const auto& u : users) {
            file << u.userID << " | " << u.name << " | ";
            for (const auto& isbn : u.borrowedISBN)
                file << isbn << ",";
            file << "\n";
        }
    }
};

void showLibrarianMenu() {
    cout << "\n\t============Library Management System (Librarian)============\t\n"
         << "[1] Register User\n"
         << "[2] Add Book\n"
         << "[3] Delete Book\n"
         << "[4] Remove User\n"
         << "[5] Borrow Book\n"
         << "[6] Return Book\n"
         << "[7] Display Books\n"
         << "[8] Display Users\n"
         << "[9] Exit" << endl;
    cout << "\n\t=============================================================\t\n";
}
void showUser() {
    cout << "\n\t============Library Management System (User )============\t\n"
         << "[1] Borrow Book\n"
         << "[2] Return Book\n"
         << "[3] Display Books\n"
         << "[4] Exit" << endl;
    cout << "\n\t=========================================================\t\n";
}

void clearScreen() {
    system("cls");
}

int main() {
    Library library;
    int choice;
    string title, author, isbn, userID, name;
    char role;

    while (true) {
        // MAIN MENU
        do {
            cout << "\n====== Library System Main Menu ======\n";
            cout << "[A] Librarian\n";
            cout << "[B] User\n";
            cout << "[C] Exit Program\n";
            cout << "Enter your choice (A/B/C): ";
            cin >> role;
            role = toupper(role);
            cin.ignore(); // clear newline from buffer
        } while (role != 'A' && role != 'B' && role != 'C');

        if (role == 'C') {
            cout << "\nThank you for using the system. Goodbye!\n";
            break; // Exit the entire program
        }

        // Librarian's Menu
        if (role == 'A') {
            do {
                showLibrarianMenu();
                cout << "Enter choice: ";
                cin >> choice;
                cin.ignore();

                switch (choice) {
                    case 1:
                        cout << "Enter User ID: ";
                        getline(cin, userID);
                        cout << "Enter Name: ";
                        getline(cin, name);
                        library.registerUser(userID, name);
                        break;
                    case 2:
                        cout << "Enter Title: ";
                        getline(cin, title);
                        cout << "Enter Author: ";
                        getline(cin, author);
                        cout << "Enter ISBN: ";
                        getline(cin, isbn);
                        library.addBook(title, author, isbn);
                        break;
                    case 3:
                        library.displayBooks();
                        cout << "Enter ISBN to remove: ";
                        getline(cin, isbn);
                        library.removeBook(isbn);
                        break;
                    case 4:
                        library.displayUsers();
                        cout << "Enter User ID to remove: ";
                        getline(cin, userID);
                        library.removeUser(userID);
                        break;
                    case 5:
                        library.displayBooks();
                        cout << "Enter User ID: ";
                        getline(cin, userID);
                        cout << "Enter ISBN: ";
                        getline(cin, isbn);
                        library.borrowBook(userID, isbn);
                        break;
                    case 6:
                        cout << "Enter User ID: ";
                        getline(cin, userID);
                        cout << "Enter ISBN: ";
                        getline(cin, isbn);
                        library.returnBook(userID, isbn);
                        break;
                    case 7:
                        library.displayBooks();
                        break;
                    case 8:
                        library.displayUsers();
                        break;
                    case 9:
                        cout << "Logging out librarian... Returning to main menu.\n";
                        clearScreen();
                        break;
                    default:
                        cout << "Invalid choice.\n";
                }
            } while (choice != 9);
        }

        // User's Menu
        else if (role == 'B') {
            do {
                showUser();
                cout << "Enter choice: ";
                cin >> choice;
                cin.ignore();

                switch (choice) {
                    case 1:
                        library.displayBooks();
                        cout << "Enter User ID: ";
                        getline(cin, userID);
                        cout << "Enter ISBN: ";
                        getline(cin, isbn);
                        library.borrowBook(userID, isbn);
                        break;
                    case 2:
                        cout << "Enter User ID: ";
                        getline(cin, userID);
                        cout << "Enter ISBN: ";
                        getline(cin, isbn);
                        library.returnBook(userID, isbn);
                        break;
                    case 3:
                        library.displayBooks();
                        break;
                    case 4:
                        cout << "Logging out user... Returning to main menu.\n";
                        clearScreen();
                        break;
                    default:
                        cout << "Invalid choice.\n";
                }
            } while (choice != 4);
        }
    }

    return 0;
}

