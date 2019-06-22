//
// Created by nikita on 22.06.19.
//

#include <gtest/gtest.h>
#include <vector>
#include "list.h"



template <typename T>
bool cmp(list<T> const& a, list<T> const& b) {
    auto it1 = a.begin(), it2 = b.begin();
    for ( ; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
        if (*it1 != *it2) return false;
    }
    return it1 == a.end() && it2 == b.end();
}


template <typename T>
bool cmp(list<T> const& a, std::vector<T> const& b) {
    auto it1 = a.begin(), it2 = b.begin();
    for ( ; it1 != a.end() && it2 != b.end(); ++it1, ++it2) {
        EXPECT_EQ(*it1, *it2);
        if (*it1 != *it2) return false;
    }
    EXPECT_TRUE(it1 == a.end());
    EXPECT_TRUE(it2 == b.end());
    if (it1 != a.end() || it2 != b.end()) {
        return false;
    }

    auto rit1 = a.rbegin(), rit2 = b.rbegin();
    for ( ; rit1 != a.rend() && rit2 != b.rend(); ++rit1, ++rit2) {
        EXPECT_EQ(*rit1, *rit2);
        if (*rit1 != *rit2) return false;
    }
    EXPECT_TRUE(rit1 == a.rend());
    EXPECT_TRUE(rit2 == b.rend());
    return rit1 == a.rend() && rit2 == b.rend();
}


TEST(correctness, push_pop_back) {
    list<size_t> l;
    std::vector<size_t> expected;
    for (size_t i = 0; i < 10; ++i) {
        l.push_back(i);
        expected.push_back(i);
        EXPECT_TRUE(cmp(l, expected));
    }
    for (size_t i = 0; i < 10; ++i) {
        l.pop_back();
        expected.pop_back();
        EXPECT_TRUE(cmp(l, expected));
    }
    EXPECT_TRUE(l.empty());
    for (size_t i = 0; i < 10; ++i) {
        l.push_back(i);
        expected.push_back(i);
        EXPECT_TRUE(cmp(l, expected));
    }
    for (size_t i = 0; i < 10; ++i) {
        l.pop_back();
        expected.pop_back();
        EXPECT_TRUE(cmp(l, expected));
    }
    EXPECT_TRUE(l.empty());
}

template <typename T>
auto pos(list<T>& container, size_t shift) {
    auto it = container.begin();
    for (size_t i = 0; i < shift; ++i) {
        ++it;
    }
    return it;
}

template <typename T>
auto pos(std::vector<T> container, size_t shift) {
    return container.begin() + shift;
}

template <typename T>
void insert_both(list<T>& l, std::vector<T>& exp) {
    std::cout << "insert" << std::endl;
    auto p = rand() % (exp.size() + 1);
    size_t val = rand();
    std::cout << exp.data() << std::endl;
    l.insert(std::next(l.begin(), p), val);
//    exp.insert(pos(exp, p), val);
    exp.insert(exp.begin() + p, val);
}

template <typename T>
void erase_both(list<T>& l, std::vector<T>& exp) {
    std::cout << "erase" << std::endl;
    if (exp.size() == 0) return;
    auto p = rand() % (exp.size());
    l.erase(pos(l, p));
    exp.erase(exp.begin() + p);
}

TEST(correctness, insert) {
    list<size_t> l;
    std::vector<size_t> expected;
    for (size_t i = 0; i < 10; ++i) {
        insert_both(l, expected);
        EXPECT_TRUE(cmp(l, expected));
    }
    for (size_t i = 0; i < 10; ++i) {
        erase_both(l, expected);
        EXPECT_TRUE(cmp(l, expected));
    }
    EXPECT_TRUE(l.empty());
    for (size_t i = 0; i < 10; ++i) {
        insert_both(l, expected);
        EXPECT_TRUE(cmp(l, expected));
    }
    for (size_t i = 0; i < 10; ++i) {
        erase_both(l, expected);
        EXPECT_TRUE(cmp(l, expected));
    }
    EXPECT_TRUE(l.empty());
}
