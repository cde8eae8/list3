//
// Created by nikita on 12.06.19.
//

#ifndef HUFFMAN_BITWRITER_H
#define HUFFMAN_BITWRITER_H

#include <vector>
#include <assert.h>

#include "bit_operations.h"

struct bitarray {
    bitarray(std::vector<unsigned char> data_, size_t length_, size_t tail_) :
                                    length(length_), tail(tail_) {
        size_t s = length + (tail != 0);
        data_ = new unsigned char[s];
        std::copy(data_.begin(), data_.begin() + s, data_);
    }

    bitarray(bitarray const& rhs) : length(rhs.length), tail(rhs.tail) {
        size_t s = length + (tail != 0);
        data_ = new unsigned char[s];
//        std::copy(rhs.data_, rhs.data_ + s, data_);
    }

    bitarray& operator=(bitarray const& rhs) {
        bitarray tmp(rhs);
        std::swap(data_, tmp.data_);
        std::swap(length, tmp.length);
        std::swap(tail, tmp.tail);
        return *this;
    }

    ~bitarray() {
        delete [] data_;
    }

    friend std::ostream& operator<<(std::ostream& out, bitarray const& a) {
        out << "[" << a.length * 8 + a.tail << "]" << bits::bits_to_string(a.data_, a.length * 8 + a.tail);
        return out;
    }

    unsigned char* data() {
       return data_.data_;
    }

    size_t bit_length() {
        return data_.bit_size;
    }


 private:
    class Data {
        size_t bit_size;
        size_t nRefs;
        unsigned char data_[0];
    };

    Data data_;

};


struct bitreader {
    bitreader(bitarray const& bits) : data_(bits), pos_(0), tail_pos_(0) {}

    unsigned char get() {
        unsigned char tmp = bits::get_bit(data_.data_[pos_], 7 - tail_pos_);
        bits::next_bit(pos_, tail_pos_);
        return tmp;
    }

    bool eob() {
        return pos_ > data_.length || (pos_ == data_.length && tail_pos_ >= data_.tail);
    }

 private:
    bitarray const& data_;
    size_t pos_;
    size_t tail_pos_;
};

struct bitwriter {
    bitwriter() : pos_(0), tail_pos_(0), buffer_(1) {}
    /**
     * write length*8 + tail bytes to internal buffer
     * @param bytes bitlen(bytes) // 8
     * @param length bitlen(bytes) % 8
     */
    void write(unsigned char const *bytes, size_t length);

    /**
     * write one bit to internal buffer
     * @param bit should be 1 or 0, else result is undefined
     */
    void push_back(unsigned char bit);
    /**
     * reset state to initial, return current state
     * @return internal state for all writes since previous reset or construction of the object
     */
    bitarray reset();


    bitarray get();

 private:
    /**
     * get change of the length dependent on tail size
     */
    size_t tailShift(size_t tail) {
        return tail != 0;
    }

    size_t pos_;
    size_t tail_pos_;
    std::vector<unsigned char> buffer_;
};

#endif //HUFFMAN_BITWRITER_H
