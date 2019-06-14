//
// Created by nikita on 11.06.19.
//
#include <iostream>
#include "bitwriter.h"
#include "bit_operations.h"


void bitwriter::write(unsigned char const *bytes, size_t bit_length) {
    if (bit_length == 0) return;
    size_t tmp_pos = pos_;
    size_t tmp_tail = tail_pos_;
    size_t length = bit_length / 8;
    size_t tail = bit_length % 8;

    bits::next_bit(pos_, tail_pos_, bit_length);
    buffer_.resize(pos_ + tailShift(tail_pos_));

    for (size_t i = 0; i < length; ++i, ++tmp_pos) {
       buffer_[tmp_pos] |= bits::lowPart(bytes[i], tmp_tail);
       if (tmp_pos + 1 < buffer_.size())
           buffer_[tmp_pos + 1] = bits::highPart(bytes[i], tmp_tail);
    }

    if (tail > 0) {
        buffer_[tmp_pos++] |= bits::lowPart(bytes[length], tmp_tail);
        if (tmp_tail + tail > 8)
            buffer_[tmp_pos] = bits::highPart(bytes[length], tmp_tail);
    }
    clear_end();
}

bitarray bitwriter::reset() {
    if (pos_ == 0 && tail_pos_ == 0)
        return {{}, 0, 0};
    clear_end();
    bitarray barr(buffer_, pos_, tail_pos_);
    pos_ = tail_pos_ = 0;
    buffer_.clear();
    return barr;
}

#ifdef DEBUG
bitarray bitwriter::get() {
    clear_end();
    return {buffer_, pos_, tail_pos_};
}
#endif


void bitwriter::push_back(unsigned char bit) {
    assert(bit == 0 || bit == 1);
    size_t tmp_pos = pos_;
    size_t tmp_tail = tail_pos_;
    bits::next_bit(pos_, tail_pos_);
    buffer_.resize(pos_ + tailShift(tail_pos_));
    bits::assign_bit(buffer_[tmp_pos], bit, 7 - tmp_tail);
}
