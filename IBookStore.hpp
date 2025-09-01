#pragma once
#include "Book.hpp"
#include <vector>
#include <optional>
#include <string>

enum class SortKey { Title, Year, Author, Id };

class IBookStore {
public:
    virtual ~IBookStore() = default;

    virtual bool add(const Book& b) = 0;
    virtual std::vector<Book> getAll() = 0;
    virtual std::optional<Book> findById(int id) = 0;
    virtual bool update(int id, const Book& updated) = 0;
    virtual bool remove(int id) = 0;
    virtual bool exportCsv(const std::string& path) = 0;
    virtual std::vector<Book> sortBy(SortKey key, bool ascending = true) = 0;
};
