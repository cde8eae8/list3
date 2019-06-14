//
// Created by nikita on 12.06.19.
//

#include <gtest/gtest.h>

#include "../bitwriter.h"
#include "../bit_operations.h"

TEST(bitwriter_test, highPart) {
    using bits::highPart;
    bitwriter bw;
    EXPECT_EQ(highPart(0b11111111, 0), 0b0);
    EXPECT_EQ(highPart(0b11111111, 1), 0b10000000);
    EXPECT_EQ(highPart(0b11111111, 2), 0b11000000);
    EXPECT_EQ(highPart(0b11111111, 3), 0b11100000);
    EXPECT_EQ(highPart(0b11111111, 4), 0b11110000);
    EXPECT_EQ(highPart(0b11111111, 5), 0b11111000);
    EXPECT_EQ(highPart(0b11111111, 6), 0b11111100);
    EXPECT_EQ(highPart(0b11111111, 7), 0b11111110);
}

TEST(bitwriter_test, lowPart) {
    using bits::lowPart;
    bitwriter bw;
    EXPECT_EQ(lowPart(0b11111111, 7), 0b1);
    EXPECT_EQ(lowPart(0b11111111, 6), 0b11);
    EXPECT_EQ(lowPart(0b11111111, 5), 0b111);
    EXPECT_EQ(lowPart(0b11111111, 4), 0b1111);
    EXPECT_EQ(lowPart(0b11111111, 3), 0b11111);
    EXPECT_EQ(lowPart(0b11111111, 2), 0b111111);
    EXPECT_EQ(lowPart(0b11111111, 1), 0b1111111);
    EXPECT_EQ(lowPart(0b11111111, 0), 0b11111111);
}

namespace bits_generator {
    using namespace bits;
    std::vector<unsigned char> string_to_bits(std::string const& s) {
        std::vector<unsigned char> result;
        result.resize(s.length() / 8 + (s.length() % 8 ? 1 : 0));
        size_t cur_bit = 0;
        size_t pos = 0;
        for (size_t i = 0; i < s.size(); ++i) {
            result[pos] |= (s[i] - '0') << (7 - cur_bit);
            ++cur_bit;
            if (cur_bit == 8) {
                ++pos;
                cur_bit = 0;
            }
        }
        return result;
    }

    bool cmp_results(std::string expected, std::string actual) {
        if (expected.size() > actual.size()) return false;
        for (size_t i = 0; i < expected.size(); ++i) {
            if (expected[i] != actual[i]) {
                return false;
            }
        }
        for (size_t j = expected.size(); j < actual.size(); ++j) {
            if (actual[j] != '0') return false;
        }
        return true;
    }

    std::string gen_random_string(size_t length) {
        std::string r;
        for (size_t i = 0; i < length; ++i) {
            r += (rand()) % 2 + '0';
        }
        return r;
    }

    void test_bitwriter() {
        bitwriter bw;
        std::string s = gen_random_string(rand() % 1000);
        for (size_t i = 0; i < s.length(); ) {
           size_t chunk_size = rand() % 18;
           chunk_size = 1;
           if (i + chunk_size > s.length()) {
               chunk_size = s.length() - i;
           }
            bw.write(string_to_bits(s.substr(i, i + chunk_size)).data(), chunk_size);
           i += chunk_size;
        }
        auto res = bw.reset();
        EXPECT_EQ(res.length(), s.size() / 8);
        EXPECT_EQ(res.tail(), s.size() % 8);
        EXPECT_TRUE(cmp_results(s, bits_to_string(res.data(), res.bit_length())));
    }

    void test_bitreader() {
        bitwriter bw;
        std::string s = gen_random_string(rand() % 1000);
        for (size_t i = 0; i < s.length(); ) {
            size_t chunk_size = rand() % 18;
            if (i + chunk_size > s.length()) {
                chunk_size = s.length() - i;
            }
            bw.write(string_to_bits(s.substr(i, i + chunk_size)).data(), chunk_size);
            i += chunk_size;
        }
        auto res = bw.reset();
        std::string result;
        bitreader br(res);
        while(!br.eob()) {
            result += br.get() ? '1' : '0';
        }
        EXPECT_EQ(result, s);

    }

    void test_bitwriter_pushback() {
        bitwriter bw;
        std::string s = gen_random_string(rand() % 100);
        for (size_t i = 0; i < s.length(); ++i) {
            bw.push_back(s[i] != '0');
        }
        auto res = bw.reset();
        EXPECT_EQ(res.length(), s.size() / 8);
        EXPECT_EQ(res.tail(), s.size() % 8);
        EXPECT_TRUE(cmp_results(s, bits_to_string(res.data(), res.bit_length())));

    }

}

TEST(test_correctness, writeBits) {
//    std::string test = "10010101010101110101001101010101010100101";
//    EXPECT_TRUE(bits_generator::cmp_results(test, bits_to_string(bits_generator::string_to_bits(test))));
//    test = "10010101010101110101001101010101010100101";
//    EXPECT_TRUE(bits_generator::cmp_results(test, bits_to_string(bits_generator::string_to_bits(test))));
//    test = "1001";
//    EXPECT_TRUE(bits_generator::cmp_results(test, bits_to_string(bits_generator::string_to_bits(test))));
//    test = "1001010101010";
//    EXPECT_TRUE(bits_generator::cmp_results(test, bits_to_string(bits_generator::string_to_bits(test))));
//    test = "1001010101000010";
//    EXPECT_TRUE(bits_generator::cmp_results(test, bits_to_string(bits_generator::string_to_bits(test))));
//    test = "100101010101011101010011";
//    EXPECT_TRUE(bits_generator::cmp_results(test, bits_to_string(bits_generator::string_to_bits(test))));
}


TEST(bitIO, writeBits) {
    bitwriter bw;
    std::vector<unsigned char> test_data = {0b10101010, 0b11111111, 0b01010101, 0b11111111};

    bw.write(test_data.data(), 4*8);
    auto result = bw.reset();
    EXPECT_EQ(result.tail(), 0);


    bw.write(&test_data[0], 4);
    bw.write(&test_data[1], 3);
    bw.write(&test_data[0], 2);
    bw.write(&test_data[1], 1);
    result = bw.reset();
    std::vector<unsigned char> expected = {0b10101111, 0b01000000};
    EXPECT_EQ(result.tail(), 2);
    EXPECT_EQ(1, result.length());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(expected[i], result.data()[i]);
    }


    bits_generator::test_bitwriter();
    bits_generator::test_bitwriter();
    bits_generator::test_bitwriter();
    bits_generator::test_bitwriter();
    bits_generator::test_bitwriter();
    bits_generator::test_bitwriter();
    bits_generator::test_bitwriter();
}


TEST(bit_operations, set_clear) {
    using bits::set_bit;
    using bits::clear_bit;
    size_t n = 0;
    set_bit(n, 4);
    EXPECT_EQ(0b10000, n);
    set_bit(n, 1);
    EXPECT_EQ(0b10010, n);
    set_bit(n, 0);
    EXPECT_EQ(0b10011, n);
    clear_bit(n, 0);
    EXPECT_EQ(0b10010, n);
    clear_bit(n, 2);
    EXPECT_EQ(0b10010, n);
    clear_bit(n, 1);
    EXPECT_EQ(0b10000, n);
}

TEST(bit_operations, bits_to_bytes) {
    using bits::bit_size_to_byte;
    EXPECT_EQ(bit_size_to_byte(16), 2);
    EXPECT_EQ(bit_size_to_byte(0), 0);
    EXPECT_EQ(bit_size_to_byte(1), 1);
    EXPECT_EQ(bit_size_to_byte(6), 1);
    EXPECT_EQ(bit_size_to_byte(7), 1);
    EXPECT_EQ(bit_size_to_byte(15), 2);
    EXPECT_EQ(bit_size_to_byte(17), 3);
}

TEST(bitIO, push_back) {
    bits_generator::test_bitwriter_pushback();
    bits_generator::test_bitwriter_pushback();
    bits_generator::test_bitwriter_pushback();
    bits_generator::test_bitwriter_pushback();
    bits_generator::test_bitwriter_pushback();
}

TEST(bitIO, read) {
    bits_generator::test_bitreader();
    bits_generator::test_bitreader();
    bits_generator::test_bitreader();
    bits_generator::test_bitreader();
    bits_generator::test_bitreader();
}