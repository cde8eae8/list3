//
// Created by nikita on 12.06.19.
//

#ifndef HUFFMAN_BITWRITER_H
#define HUFFMAN_BITWRITER_H

#include <vector>
#include <algorithm>
#include <assert.h>

#include "bit_operations.h"


struct bitarray {
    bitarray() : data_(nullptr) {}

    template <typename InputIterator>
    bitarray(InputIterator data, size_t bit_length) : bitarray(data, bit_length / 8, bit_length % 8){}

    template <typename InputIterator>
    bitarray(InputIterator data, size_t length, size_t tail) {
        size_t s = length + (tail != 0);
//        std::cout << "s = " << s << std::endl;
//        data_ = static_cast<Data*>(operator new(sizeof(Data) + s * sizeof(unsigned char)));
        data_ = static_cast<Data*>(operator new(sizeof(Data)));
        data_->data_ = static_cast<char*>(operator new(s * sizeof(unsigned char)));
        data_->bit_size = length * 8 + tail;
        std::copy_n(data, s, data_->data_);
        // skip last char of input
        ++data;
        data_->nRefs = 1;
    }

    bitarray(bitarray const& rhs) {
        data_ = rhs.data_;
        addLink();
    }

    bitarray& operator=(bitarray const& rhs) {
        if (data_ == rhs.data_) return *this;
        deleteLink();
        data_ = rhs.data_;
        addLink();
        return *this;
    }

    ~bitarray() {
        deleteLink();
    }

    friend std::ostream& operator<<(std::ostream& out, bitarray const& a) {
        out << "[" << a.bit_length() << "]" << bits::bits_to_string(a.data(), a.bit_length());
        return out;
    }

    char* data() const {
        if (!data_) return nullptr;
       return data_->data_;
    }

    size_t bit_length() const {
        if (!data_) return 0;
        return data_->bit_size;
    }

    size_t length() const {
        if (!data_) return 0;
        return data_->bit_size / 8;
    }

    size_t tail() const {
        if (!data_) return 0;
        return data_->bit_size % 8;
    }

    size_t byte_length() const {
        if (!data_) return 0;
       return bits::byte_to_length(length(), tail());
    }

 private:
    struct Data {
        size_t bit_size;
        size_t nRefs;
        char* data_;
    };

    void addLink() {
        if (!data_) return;
        ++data_->nRefs;
    }

    void deleteLink() {
        if (!data_) return;
       --data_->nRefs;
       if(data_->nRefs == 0) {
           operator delete(data_);
       }
    }

    Data *data_;
};

struct bytearray : private bitarray {
    bytearray() : bitarray() {}

    template <typename T>
    bytearray(T* data, size_t size) : bitarray(data, size * 8) {}


    char const *array() const {
        return data();
    }

    auto &operator[](size_t pos) {
        return data()[pos];
    }

    auto const &operator[](size_t pos) const {
        return data()[pos];
    }

    size_t length() const {
        return byte_length();
    }
};


struct bitreader {
    explicit bitreader(bitarray const& bits) : data_(bits), pos_(0), tail_pos_(0) {}

    unsigned char get() {
        unsigned char tmp = bits::get_bit(data_.data()[pos_], 7 - tail_pos_);
        bits::next_bit(pos_, tail_pos_);
        return tmp;
    }

    bool eob() {
        return pos_ > data_.length() || (pos_ == data_.length() && tail_pos_ >= data_.tail());
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

    void clear_end() {
        bits::clear_low_bits(buffer_[pos_ - 1 + tailShift(tail_pos_)], tail_pos_);
    }

    size_t pos_;
    size_t tail_pos_;
    std::vector<unsigned char> buffer_;
};

#endif //HUFFMAN_BITWRITER_H
