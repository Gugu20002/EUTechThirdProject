#pragma once
#include "IBookStore.hpp"
#include <string>

class FileBookStore : public IBookStore {
public:
    explicit FileBookStore(std::string dataFilePath);

    bool add(const Book& b) override;
    std::vector<Book> getAll() override;
    std::optional<Book> findById(int id) override;
    bool update(int id, const Book& updated) override;
    bool remove(int id) override;
    bool exportCsv(const std::string& path) override;
    std::vector<Book> sortBy(SortKey key, bool ascending) override;

private:
    std::string path;
};
