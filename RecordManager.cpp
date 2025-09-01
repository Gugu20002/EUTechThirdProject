#include "FileBookStore.hpp"
#include "App.hpp"

int main() {
    FileBookStore store("books.dat");
    App app(store);
    app.run();
    return 0;
}
