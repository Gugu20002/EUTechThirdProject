#pragma once
#include "IBookStore.hpp"

class App {
public:
    explicit App(IBookStore& store) : store(store) {}
    void run();
private:
    IBookStore& store;
    int menu();
    void addRecord();
    void displayAll();
    void searchById();
    void deleteRecord();
    void updateRecord();
    void sortRecords();
    void exportCsv();
};
