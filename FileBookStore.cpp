#include "FileBookStore.hpp"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <system_error>

using namespace std;

static void writeString(ofstream& out, const string& s) {
    size_t len = s.size();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    out.write(s.data(), static_cast<streamsize>(len));
}

static bool readString(ifstream& in, string& s) {
    size_t len{};
    if (!in.read(reinterpret_cast<char*>(&len), sizeof(len))) return false;
    s.resize(len);
    return static_cast<bool>(in.read(s.data(), static_cast<streamsize>(len)));
}

FileBookStore::FileBookStore(string dataFilePath)
    : path(std::move(dataFilePath)) {
    // Create parent dir if needed, and ensure clean empty file exists
    filesystem::path p(path);
    if (!p.parent_path().empty())
        filesystem::create_directories(p.parent_path());
    if (!filesystem::exists(path)) {
        ofstream out(path, ios::binary | ios::trunc);
    }
}

bool FileBookStore::add(const Book& b) {
    // Duplicate ID check
    for (const auto& ex : getAll()) if (ex.id == b.id) return false;

    ofstream out(path, ios::binary | ios::app);
    if (!out) return false;

    out.write(reinterpret_cast<const char*>(&b.id), sizeof(b.id));
    out.write(reinterpret_cast<const char*>(&b.year), sizeof(b.year));
    writeString(out, b.title);
    writeString(out, b.author);
    writeString(out, b.isbn);
    writeString(out, b.status);

    return static_cast<bool>(out);
}

vector<Book> FileBookStore::getAll() {
    vector<Book> books;
    ifstream in(path, ios::binary);
    if (!in) return books;

    while (true) {
        Book b;
        if (!in.read(reinterpret_cast<char*>(&b.id), sizeof(b.id))) break;
        if (!in.read(reinterpret_cast<char*>(&b.year), sizeof(b.year))) break;
        if (!readString(in, b.title)) break;
        if (!readString(in, b.author)) break;
        if (!readString(in, b.isbn)) break;
        if (!readString(in, b.status)) break;

        // Defensive: skip a fully blank/zero record if any (corrupt start)
        if (b.id == 0 && b.year == 0 && b.title.empty() && b.author.empty() && b.isbn.empty() && b.status.empty())
            continue;

        books.push_back(std::move(b));
    }
    return books;
}

optional<Book> FileBookStore::findById(int id) {
    ifstream in(path, ios::binary);
    if (!in) return nullopt;

    while (true) {
        Book b;
        if (!in.read(reinterpret_cast<char*>(&b.id), sizeof(b.id))) break;
        if (!in.read(reinterpret_cast<char*>(&b.year), sizeof(b.year))) break;
        if (!readString(in, b.title)) break;
        if (!readString(in, b.author)) break;
        if (!readString(in, b.isbn)) break;
        if (!readString(in, b.status)) break;

        if (b.id == id) return b;
    }
    return nullopt;
}

static bool rewriteAll(const string& path, const vector<Book>& all) {
    string tmp = path + ".tmp";
    ofstream out(tmp, ios::binary | ios::trunc);
    if (!out) return false;
    for (const auto& b : all) {
        out.write(reinterpret_cast<const char*>(&b.id), sizeof(b.id));
        out.write(reinterpret_cast<const char*>(&b.year), sizeof(b.year));
        writeString(out, b.title);
        writeString(out, b.author);
        writeString(out, b.isbn);
        writeString(out, b.status);
    }
    out.close();
    std::error_code ec;
    filesystem::rename(tmp, path, ec);
    return !ec;
}

bool FileBookStore::update(int id, const Book& updated) {
    auto all = getAll();
    bool found = false;
    for (auto& b : all) {
        if (b.id == id) { b = updated; found = true; break; }
    }
    return found ? rewriteAll(path, all) : false;
}

bool FileBookStore::remove(int id) {
    auto all = getAll();
    const size_t before = all.size();
    all.erase(std::remove_if(all.begin(), all.end(),
        [&](const Book& b) { return b.id == id; }), all.end());
    if (all.size() == before) return false;
    return rewriteAll(path, all);
}

bool FileBookStore::exportCsv(const string& outPath) {
    ofstream out(outPath);
    if (!out) return false;
    out << "id,title,author,year,isbn,status\n";

    auto esc = [](const string& s) {
        string r = s;
        size_t pos = 0;
        while ((pos = r.find('"', pos)) != string::npos) { r.insert(pos, 1, '"'); pos += 2; }
        return '"' + r + '"';
        };

    for (const auto& b : getAll()) {
        out << b.id << "," << esc(b.title) << "," << esc(b.author) << ","
            << b.year << "," << esc(b.isbn) << "," << esc(b.status) << "\n";
    }
    return true;
}

vector<Book> FileBookStore::sortBy(SortKey key, bool ascending) {
    auto v = getAll();
    auto cmp = [&](const Book& a, const Book& b) {
        switch (key) {
        case SortKey::Id:    return ascending ? a.id    < b.id : a.id    > b.id;
        case SortKey::Year:  return ascending ? a.year  < b.year : a.year  > b.year;
        case SortKey::Title: return ascending ? a.title < b.title : a.title > b.title;
        case SortKey::Author:return ascending ? a.author< b.author : a.author> b.author;
        }
        return true;
        };
    std::sort(v.begin(), v.end(), cmp);
    return v;
}
