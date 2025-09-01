#include "App.hpp"
#include <iostream>
#include <limits>
#include <string>

static int readIntPrompt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        int x;
        if (std::cin >> x) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        }
        std::cout << "Invalid number. Try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int App::menu() {
    std::cout << "\n=== Record Management System ===\n"
        << "1) Add book\n"
        << "2) Display all\n"
        << "3) Search by ID\n"
        << "4) Update book\n"
        << "5) Delete book\n"
        << "6) Sort & display\n"
        << "7) Export CSV\n"
        << "0) Exit\n"
        << "Choose: ";
    int c{};
    if (!(std::cin >> c)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return c;
}

void App::run() {
    while (true) {
        switch (menu()) {
        case 1: addRecord();    break;
        case 2: displayAll();   break;
        case 3: searchById();   break;
        case 4: updateRecord(); break;
        case 5: deleteRecord(); break;
        case 6: sortRecords();  break;
        case 7: exportCsv();    break;
        case 0: std::cout << "Bye.\n"; return;
        default: std::cout << "Invalid option.\n"; break;
        }
    }
}

void App::addRecord() {
    Book b;

    b.id = readIntPrompt("Enter ID (number): ");

    std::cout << "Enter Title: ";
    std::getline(std::cin, b.title);

    std::cout << "Enter Author: ";
    std::getline(std::cin, b.author);

    b.year = readIntPrompt("Enter Year: ");

    std::cout << "Enter ISBN: ";
    std::getline(std::cin, b.isbn);

    std::cout << "Enter Status (available/loaned): ";
    std::getline(std::cin, b.status);

    if (store.add(b))
        std::cout << "Book added successfully!\n";
    else
        std::cout << "Failed to add book (duplicate ID or file error).\n";
}

void App::displayAll() {
    auto all = store.getAll();
    if (all.empty()) {
        std::cout << "No books found.\n";
        return;
    }
    std::cout << "\n--- Book Records ---\n";
    for (const auto& b : all) {
        std::cout << "ID: " << b.id
            << " | Title: " << b.title
            << " | Author: " << b.author
            << " | Year: " << b.year
            << " | ISBN: " << b.isbn
            << " | Status: " << b.status << "\n";
    }
}

void App::searchById() {
    int id = readIntPrompt("Enter ID to search: ");
    auto res = store.findById(id);
    if (!res) {
        std::cout << "Not found.\n";
        return;
    }
    const auto& b = *res;
    std::cout << "Found:\n"
        << "ID: " << b.id
        << " | Title: " << b.title
        << " | Author: " << b.author
        << " | Year: " << b.year
        << " | ISBN: " << b.isbn
        << " | Status: " << b.status << "\n";
}

void App::deleteRecord() {
    int id = readIntPrompt("Enter ID to delete: ");
    std::cout << "Are you sure? (y/n): ";
    char ch; std::cin >> ch;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    if (ch != 'y' && ch != 'Y') { std::cout << "Cancelled.\n"; return; }

    if (store.remove(id)) std::cout << "Deleted.\n";
    else std::cout << "Not found.\n";
}

void App::updateRecord() {
    int id = readIntPrompt("Enter ID to update: ");
    auto cur = store.findById(id);
    if (!cur) { std::cout << "Not found.\n"; return; }

    Book upd = *cur;
    std::string s;

    std::cout << "Leave blank to keep current.\n";

    std::cout << "Title [" << cur->title << "]: ";
    std::getline(std::cin, s); if (!s.empty()) upd.title = s;

    std::cout << "Author [" << cur->author << "]: ";
    std::getline(std::cin, s); if (!s.empty()) upd.author = s;

    std::cout << "Year [" << cur->year << "]: ";
    std::getline(std::cin, s);
    if (!s.empty()) {
        try { upd.year = std::stoi(s); }
        catch (...) { std::cout << "Invalid year. Keeping old.\n"; }
    }

    std::cout << "ISBN [" << cur->isbn << "]: ";
    std::getline(std::cin, s); if (!s.empty()) upd.isbn = s;

    std::cout << "Status [" << cur->status << "]: ";
    std::getline(std::cin, s); if (!s.empty()) upd.status = s;

    if (store.update(id, upd)) std::cout << "Updated.\n";
    else std::cout << "Update failed.\n";
}

void App::sortRecords() {
    std::cout << "Sort by: 1) Title  2) Year  3) Author  4) ID\n";
    int key = readIntPrompt("Choose: ");
    std::cout << "Ascending? 1=yes 0=no: ";
    int asc;
    if (!(std::cin >> asc)) { std::cin.clear(); asc = 1; }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    SortKey k = SortKey::Title;
    if (key == 2) k = SortKey::Year;
    else if (key == 3) k = SortKey::Author;
    else if (key == 4) k = SortKey::Id;

    auto v = store.sortBy(k, asc != 0);
    if (v.empty()) { std::cout << "No books found.\n"; return; }

    std::cout << "\n--- Sorted Books ---\n";
    for (const auto& b : v) {
        std::cout << "ID: " << b.id
            << " | Title: " << b.title
            << " | Author: " << b.author
            << " | Year: " << b.year
            << " | ISBN: " << b.isbn
            << " | Status: " << b.status << "\n";
    }
}

void App::exportCsv() {
    std::string path;
    std::cout << "Output CSV path (e.g., books.csv): ";
    std::getline(std::cin, path);
    if (path.empty()) { std::cout << "Cancelled.\n"; return; }
    if (store.exportCsv(path)) std::cout << "Exported to " << path << "\n";
    else std::cout << "Export failed.\n";
}
