#include "addons/circularbuffer.h"

#include <iostream>
#include <vector>

template<typename T>
void print_buffer(addons::CircularBuffer<T>& buf) {
    for (auto i: buf) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
}

int main() {
    addons::CircularBuffer<std::string> a(4, "11");
    addons::CircularBuffer<int> b = {3, 2, 1, 4};
    addons::CircularBuffer<float> c(2);
    std::cout << a.size() << std::endl;
    std::cout << b.capacity() << std::endl;
    std::cout << c.capacity() << std::endl;

    const auto s = a.begin();
    auto p = b.begin();
    p++;

    for (auto k = b.begin(); k < b.end(); k++) {
        std::cout << *k << ' ';
    }
    std::cout << '\n';
    for (auto i: b) {
        std::cout << i << ' ';
    }

    std::cout << "\nSorted: ";
    std::sort(b.begin(), b.end());
    print_buffer(b);

    addons::CircularBuffer<int> copied = {12, 21, 11, 22};
    b.assign(copied.begin(), copied.end());
    print_buffer(b);
    b.assign({1, 2, 3});
    print_buffer(b);

    std::cout << b.max_size();

    std::vector<int> x = {1,2,3};
    x.erase(x.cbegin() + 1);
}
