//
// Created by nikita on 11.06.19.
//
#include <iostream>
#include "bitwriter.h"
#include "bit_operations.h"


void bitwriter::write(unsigned char const *bytes, size_t bit_length) {
//    std::cout << "req " << bit_length << " ";
//    for (size_t j = 0; j < bits::bit_size_to_byte(bit_length); ++j) {
//       std::cout << std::hex << "'" << (int)bytes[j] << "' ";
//    }
//    std::cout << std::dec << std::endl;
    size_t tmp_pos = pos_;
    size_t tmp_tail = tail_pos_;
    size_t length = bit_length / 8;
    size_t tail = bit_length % 8;
    pos_ += length;
    tail_pos_ += tail;
    if (tail_pos_ >= 8) {
        pos_++;
        tail_pos_ -= 8;
    }

    buffer_.resize(pos_ + tailShift(tail_pos_));
    if (length > 0) {
        buffer_[tmp_pos++] |= bits::lowPart(bytes[0], tmp_tail);
        for (size_t i = 1; i < length; ++i) {
            buffer_[tmp_pos++] = bits::highPart(bytes[i - 1], tmp_tail) | bits::lowPart(bytes[i], tmp_tail);
        }
        if (tmp_tail > 0)
            buffer_[tmp_pos] = bits::highPart(bytes[length - 1], tmp_tail);
    }
    if (tail > 0) {
        buffer_[tmp_pos++] |= bits::lowPart(bytes[length], tmp_tail);
        if (tmp_tail + tail > 8)
            buffer_[tmp_pos] = bits::highPart(bytes[length], tmp_tail);
    }
    buffer_[pos_ - 1 + tailShift(tail_pos_)] &= tail_pos_ ? bits::highPart(0xFF, tail_pos_) : 0xFF;
}

bitarray bitwriter::reset() {
    if (pos_ == 0 && tail_pos_ == 0)
        return {{}, 0, 0};
    buffer_[pos_ - 1 + tailShift(tail_pos_)] &= tail_pos_ ? bits::highPart(0xFF, tail_pos_) : 0xFF;
    bitarray barr(buffer_, pos_, tail_pos_);
    pos_ = tail_pos_ = 0;
#ifdef DEBUG
    buffer_.clear();
#endif
    return barr;
}

bitarray bitwriter::get() {
    buffer_[pos_ - 1 + tailShift(tail_pos_)] &= tail_pos_ ? bits::highPart(0xFF, tail_pos_) : 0xFF;
    return {buffer_, pos_, tail_pos_};
}


void bitwriter::push_back(unsigned char bit) {
    assert(bit == 0 || bit == 1);
    size_t tmp_pos = pos_;
    size_t tmp_tail = tail_pos_;
    ++tail_pos_;
    if (tail_pos_ >= 8) {
        pos_++;
        tail_pos_ -= 8;
    }
    buffer_.resize(pos_ + tailShift(tail_pos_));
    if (bit)
        bits::set_bit(buffer_[tmp_pos], 7 - tmp_tail);
    else
        bits::clear_bit(buffer_[tmp_pos], 7 - tmp_tail);
}
