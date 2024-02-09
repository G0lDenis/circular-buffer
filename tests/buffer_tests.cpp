#include "addons/circularbuffer.cpp"
#include <gtest/gtest.h>
#include <vector>

using namespace addons;

TEST(CircularBufferTestSuit, ConstructorsTest) {
    addons::CircularBuffer<std::string> a(4, "abc");
    addons::CircularBuffer<int> b = {3, 2, 1, 4, 5};
    addons::CircularBuffer<float> c(2);

    class MyClass {
    private:
        int a{2};
        std::string b{"124"};
    };

    addons::CircularBuffer<MyClass> d(7, MyClass());

    ASSERT_EQ(a.size(), 4);
    ASSERT_EQ(a.capacity(), 4);
    ASSERT_EQ(b.size(), 5);
    ASSERT_EQ(b.capacity(), 5);
    ASSERT_EQ(c.size(), 0);
    ASSERT_EQ(c.capacity(), 2);
    ASSERT_EQ(d.size(), 7);
    ASSERT_EQ(d.capacity(), 7);
}

TEST(CircularBufferTestSuit, IteratorsTest) {
    addons::CircularBuffer<std::string> a(4, "abc");
    addons::CircularBuffer<int> b = {3, 2, 1, 4, 5};
    addons::CircularBuffer<float> c(2);

    class MyClass {
    private:
        int a{2};
        std::string b{"124"};
    public:
        [[nodiscard]] int foo() const {
            return 228 + a;
        }
    };

    addons::CircularBuffer<MyClass> d(7, MyClass());

    int n = 0;
    for (auto i = a.begin(); i < a.end(); i++, n++) {
        ASSERT_EQ(*i, "abc");
    }
    ASSERT_EQ(n, a.size());

    n = 0;
    int b_values[] = {3, 2, 1, 4, 5};
    for (auto i = b.begin(); i < b.end(); i++, n++) {
        ASSERT_EQ(*i, b_values[n]);
    }
    ASSERT_EQ(n, b.size());

    n = 0;
    for (auto i = c.begin(); i < c.end(); i++, n++) {}
    ASSERT_EQ(n, c.size());

    n = 0;
    for (auto i = d.begin(); i < d.end(); i++, n++) {
        ASSERT_EQ((*i).foo(), i->foo());
    }
    ASSERT_EQ(n, d.size());
}

template<typename T>
std::vector<T> init_sort(std::initializer_list<T> _init) {
    std::vector<T> sorted(_init);
    std::sort(sorted.begin(), sorted.end());
    return sorted;
}

TEST(CircularBufferTestSuit, SortingTest) {
    std::initializer_list<std::string> init_a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", ""};
    addons::CircularBuffer<std::string> a = init_a;
    auto init_b = {3, 2, 1, 4, 5, 23, -12, 32333};
    addons::CircularBuffer<int> b{init_b};

    auto sorted_a = init_sort(init_a);
    auto sorted_b = init_sort(init_b);

    std::sort(a.begin(), a.end());

    ASSERT_EQ(a.size(), sorted_a.size());
    std::cout << a[0];
    for (auto i = 0; i < a.size(); i++) {
        ASSERT_EQ(a[i], sorted_a[i]);
    }

    std::sort(b.begin(), b.end());

    ASSERT_EQ(b.size(), init_b.size());
    for (auto i = 0; i < b.size(); i++) {
        ASSERT_EQ(b[i], sorted_b[i]);
    }
}

TEST(CircularBufferTestSuit, GetOperatorsTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    addons::CircularBuffer<int> b = {3, 2, 1, 4, 5, 23, -12, 32333};

    ASSERT_EQ(*a.begin(), "12");
    ASSERT_EQ(*b.begin(), 3);

    ASSERT_EQ(a.front(), "12");
    ASSERT_EQ(b.front(), 3);

    ASSERT_EQ(a.back(), "This is end...");
    ASSERT_EQ(b.back(), 32333);
}

TEST(CircularBufferTestSuit, AssignTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    a.assign({"First", "Second", "Third", "Fourth", "Fifth"});
    std::string init_a[] = {"First", "Second", "Third", "Fourth", "Fifth", "", "This is end..."};

    ASSERT_EQ(a.size(), 7);
    for (auto i = 0; i < a.size(); i++) {
        ASSERT_EQ(a[i], init_a[i]);
    }

    addons::CircularBuffer<double> b = {3.21, 6.54, 3.22, 1213.3232, -473843.2, 3242.0001};
    addons::CircularBuffer<double> new_b = {1.01, 2.02, -3.03, -4.04, -5.05, 6.06, 7.07, 8.08, 9.09};
    b.assign(new_b.begin(), new_b.end());
    double init_b[] = {7.07, 8.08, 9.09, -4.04, -5.05, 6.06};

    ASSERT_EQ(b.size(), 6);
    for (auto i = 0; i < b.size(); i++) {
        ASSERT_EQ(b[i], init_b[i]);
    }
}

TEST(CircularBufferTestSuit, ReverseTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    addons::CircularBuffer<double> b = {3.21, 6.54, 3.22, 1213.3232, -473843.2, 3242.0001};

    std::string init_a[] = {"This is end...", "", "Lakes", "Leeks", "aBCCD", "ABc", "12"};
    double init_b[] = {3242.0001, -473843.2, 1213.3232, 3.22, 6.54, 3.21};

    int i = 0;
    auto k = a.rbegin();
    for (; k < a.rend(); i++, k++) {
        ASSERT_EQ(*k, init_a[i]);
    }
    ASSERT_EQ(i, 7);

    i = 0;
    auto j = b.rbegin();
    for (; j < b.rend(); i++, j++) {
        ASSERT_EQ(*j, init_b[i]);
    }
    ASSERT_EQ(i, 6);
}

TEST(CircularBufferTestSuit, ReserveShrinkingTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    addons::CircularBuffer<uint32_t> b(1011);

    a.reserve(12);
    ASSERT_EQ(a.size(), 7);
    ASSERT_EQ(a.capacity(), 12);
    ASSERT_EQ(b.size(), 0);
    ASSERT_EQ(b.capacity(), 1011);

    a.shrink_to_fit();
    ASSERT_EQ(a.size(), 7);
    ASSERT_EQ(a.capacity(), 7);

    b.shrink_to_fit();
    ASSERT_EQ(b.size(), 0);
    ASSERT_EQ(b.capacity(), 0);

    b.reserve(23923911);
    ASSERT_EQ(b.capacity(), 23923911);
}

TEST(CircularBufferTestSuit, ResizeShrinkingTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    addons::CircularBuffer<uint32_t> b(1011);

    a.resize(4);
    ASSERT_EQ(a.size(), 4);
    ASSERT_EQ(a.capacity(), 7);

    std::string init_a[] = {"12", "ABc", "aBCCD", "Leeks"};
    for (auto i = 0; i < 4; i++) {
        ASSERT_EQ(a[i], init_a[i]);
    }

    a.shrink_to_fit();
    ASSERT_EQ(a.size(), a.capacity());

    b.resize(2023, 676);
    ASSERT_EQ(b.size(), 2023);
    ASSERT_EQ(b.capacity(), 2023);

    for (auto i = 0; i < 2023; i++) {
        ASSERT_EQ(b[i], 676);
    }
}

TEST(CircularBufferTestSuit, EqOperatorTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    addons::CircularBuffer<std::string> b = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};

    addons::CircularBuffer<int> a1 = {1, 2, 1};
    addons::CircularBuffer<int> b1 = {1, 2, 1};

    // ASSERT_EQ(a1, b1);
}

TEST(CircularBufferTestSuit, PushBackTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    addons::CircularBuffer<int> b = {1, 2, 1};

    a.push_back("New end! (Oh no, start)");
    b.push_back(0);

    ASSERT_EQ(a.back(), "New end! (Oh no, start)");
    ASSERT_EQ(b.back(), 0);

    a.reserve(a.capacity() + 10);
    a.push_back("New end 1");
    a.push_back("New end 2");
    a.push_back("New end 3");

    std::string init_a[] = {"ABc", "aBCCD", "Leeks", "Lakes", "",
                            "This is end...", "New end! (Oh no, start)", "New end 1", "New end 2", "New end 3"};
    int n = 0;
    for (auto i = a.begin(); i < a.end(); ++i, ++n) {
        ASSERT_EQ(*i, init_a[n]);
    }
}

TEST(CircularBufferTestSuit, PushFrontTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    addons::CircularBuffer<int> b = {1, 2, 1};

    a.push_front("New start!");
    b.push_front(0);

    ASSERT_EQ(a.front(), "New start!");
    ASSERT_EQ(b.front(), 0);
}

TEST(CircularBufferTestSuit, PopBackTest) {
    addons::CircularBuffer<std::string> a = {"12", "Front_value", "aBCCD", "Leeks", "Lakes", "Back_value",
                                             "This is end..."};
    addons::CircularBuffer<int> b = {0, 1, 2, 3, 4};

    a.pop_back();
    b.pop_back();

    ASSERT_EQ(a.back(), "Back_value");
    ASSERT_EQ(b.back(), 3);
}

TEST(CircularBufferTestSuit, PopFrontTest) {
    addons::CircularBuffer<std::string> a = {"12", "Front_value", "aBCCD", "Leeks", "Lakes", "Back_value",
                                             "This is end..."};
    addons::CircularBuffer<int> b = {0, 1, 2, 3, 4};

    a.pop_front();
    b.pop_front();

    ASSERT_EQ(a.front(), "Front_value");
    ASSERT_EQ(b.front(), 1);
}

TEST(CircularBufferTestSuit, SwapTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    addons::CircularBuffer<std::string> b = {"x1", "x2", "x3", "x4"};

    a.swap(b);

    std::string init_a[] = {"x1", "x2", "x3", "x4"};
    int n = 0;
    for (auto i = a.begin(); i < a.end(); ++i, ++n) {
        ASSERT_EQ(*i, init_a[n]);
    }

    std::string init_b[] = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};
    n = 0;
    for (auto i = b.begin(); i < b.end(); ++i, ++n) {
        ASSERT_EQ(*i, init_b[n]);
    }
}

TEST(CircularBufferTestSuit, InsertTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};

    a.insert(a.begin() + 2, "key");
    std::string init_a[] = {"12", "ABc", "key", "aBCCD", "Leeks", "Lakes", ""};

    int n = 0;
    for (auto i = a.begin(); i < a.end(); ++i, ++n) {
        ASSERT_EQ(*i, init_a[n]);
    }

    a.insert(a.begin() + 4, "key2");
    std::string init_a2[] = {"12", "ABc", "key", "aBCCD", "key2", "Leeks", "Lakes", ""};

    n = 0;
    for (auto i = a.begin(); i < a.end(); ++i, ++n) {
        ASSERT_EQ(*i, init_a2[n]);
    }
}

TEST(CircularBufferTestSuit, EraseTest) {
    addons::CircularBuffer<std::string> a = {"12", "ABc", "Key", "aBCCD", "Leeks", "Lakes", "", "This is end..."};

    a.erase(a.begin() + 2);
    std::string init_a[] = {"12", "ABc", "aBCCD", "Leeks", "Lakes", "", "This is end..."};


    int n = 0;
    for (auto i = a.begin(); i < a.end(); ++i, ++n) {
        ASSERT_EQ(*i, init_a[n]);
    }
}

