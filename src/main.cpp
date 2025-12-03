#include <iostream>
#include "../include/header.hpp"
#include "../include/list.hpp"

int main() {
    customMemoryResource mem;
    CustomList<int> lst(&mem);

    lst.push_back(10);
    lst.push_back(20);
    lst.push_front(5);
    lst.push_front(1);

    std::cout << "Список после push_front / push_back:\n";
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\nРазмер: " << lst.size() << "\n\n";

    lst.pop_front();
    std::cout << "После pop_front:\n";
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\nРазмер: " << lst.size() << "\n\n";

    lst.pop_back();
    std::cout << "После pop_back:\n";
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\nРазмер: " << lst.size() << "\n\n";

    std::cout << "Итоговое состояние:\n";
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\nРазмер: " << lst.size() << "\n";

    return 0;
}
