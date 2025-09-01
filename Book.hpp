#pragma once
#include <string>

struct Book {
    int id{};
    std::string title;
    std::string author;
    int year{};
    std::string isbn;
    std::string status; // "available" / "loaned"
};
