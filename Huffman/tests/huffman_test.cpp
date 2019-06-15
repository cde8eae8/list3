//
// Created by nikita on 12.06.19.
//

#include <gtest/gtest.h>

#include "../huffman.h"
#include "../bit_operations.h"


void test_encode(std::string test) {
    std::cout << "Test for " << (test.size() > 100 ? test.substr(0, 100) : test) << std::endl;
    HuffmanCoder hc;
    auto hd = hc.encode(test.data(), test.size());
    char* buffer = static_cast<char*>(operator new(test.size()));
    size_t length = test.size();
    hc.decode(hd, buffer, length);
    std::string result(buffer, test.size());
    EXPECT_EQ(result, test);
}

std::string randomString(size_t length) {
    std::string res;
    for (size_t i = 0; i < length; ++i) {
        res.push_back(rand());
    }
    return res;
}

TEST(huffman_tree_test, encode) {
    test_encode("123457890");
    test_encode("123");
    test_encode("12345678901234567890");
    test_encode(randomString(5));
    test_encode(randomString(10));
    test_encode("11111111111111111");
    test_encode("11111111222222222");
    test_encode("12121212211121212");
    test_encode("1");
    test_encode("");
    for (size_t i = 0; i < 30; ++i) {
        test_encode(randomString(30));
    }

    for (size_t j = 0; j < 10; ++j) {
        test_encode(randomString(10000));
    }
}
