//
// Created by nikita on 12.06.19.
//

#ifndef HUFFMAN_BIT_OPERATIONS_H
#define HUFFMAN_BIT_OPERATIONS_H

#include<string>


namespace bits {
    inline unsigned char highPart(unsigned char byte, size_t shift);
    inline unsigned char lowPart(unsigned char byte, size_t shift);

    template<typename T>
    inline void set_bit(T &number, size_t bit) {
        assert(sizeof(T) * 8 > bit);
        number |= (1U << bit);
    }

    template<typename T>
    inline void clear_bit(T &number, size_t bit) {
        assert(sizeof(T) * 8 > bit);
        number &= ~(1U << bit);
    }

    template<typename T>
    inline void assign_bit(T &number, size_t value, size_t bit) {
        assert(sizeof(T) * 8 > bit);
        assert(value == 1 || value == 0);
        if (value == 1)
            set_bit(number, bit);
        else
            clear_bit(number, bit);
    }

    inline size_t byte_size_to_bit(size_t length, size_t tail) {
        assert(tail < 8);
        return length * 8 + tail;
    }

    inline void next_bit(size_t &pos, size_t &tail, size_t shift = 1) {
        assert(tail < 8);
        tail += shift;
        pos += tail / 8;
        tail = tail % 8;
    }

    template <typename T>
    inline void clear_low_bits(T &number, size_t tail) {
        assert(tail < sizeof(T) * 8);
        T zero{0};
        number &= tail ? highPart(~zero, tail) : ~zero;
    }

    inline size_t byte_to_length(size_t length, size_t tail) {
        assert(tail < 8);
        return length + (tail != 0);
    }

    inline size_t bit_size_to_byte(size_t bit_size) {
        return bit_size / 8 + (bit_size % 8 != 0);
    }

    template<typename T>
    inline T get_bit(T number, size_t pos) {
        assert(sizeof(number) * 8 > pos);
        return (number & (1U << pos)) >> pos;
    }

    inline std::string bits_to_string(unsigned char const *bytes, size_t length_in_bits) {
        std::string result;
        for (size_t i = 0; i < length_in_bits / 8; ++i) {
            for (size_t j = 0; j < 8; ++j) {
                unsigned char bit = get_bit(bytes[i], 7 - j); // bytes[i] & (1 << (7 - j))) >> (7 - j);
                result.push_back(static_cast<char>(bit + '0'));
            }
        }
        for (size_t i = 0; i < length_in_bits % 8; ++i) {
            unsigned char bit = get_bit(bytes[length_in_bits / 8], 7 - i);
            result.push_back(static_cast<char>(bit + '0'));
        }
        return result;
    }

    /**
     * if byte will be shifted on shift bytes to the right, it will be a part
     * of two bytes
     * return the part of first byte
     */
    inline unsigned char lowPart(unsigned char byte, size_t shift) {
        assert(shift < 8);
        return static_cast<unsigned char>(byte >> shift);
    }

    /**
     * if byte will be shifted on shift bytes to the right, it will be a part
     * of two bytes
     * return the part of second byte
     */
    inline unsigned char highPart(unsigned char byte, size_t shift) {
        assert(shift < 8);
        return static_cast<unsigned char>(byte << (8U - shift));
    }
}
#endif //HUFFMAN_BIT_OPERATIONS_H
