//
// Created by nikita on 07.06.19.
//

#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include <iostream>
#include <vector>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <array>
#include "bit_operations.h"
#include "bitwriter.h"

enum class DecodeState {
    NOT_ENOUGH_MEMORY,
    SUCCESS
};

// TODO: encode(input_iterator it)?
struct HuffmanData {
    HuffmanData() : tree(), used_chars(), n_used_chars(), code() {}
    HuffmanData(bitarray const& tree_, char* used, size_t n_used) :
                           tree(tree_), used_chars(used), n_used_chars(n_used) {}
    bitarray tree;
    char* used_chars;
    size_t n_used_chars;
    bitarray code;
};

struct Code {
    Code(char ch_) : ch(ch_), frequence(0), code_bit_length(0) {
#ifdef DEBUG
        std::fill(code.begin(), code.end(), 0);
#endif
    };

    friend std::ostream& operator<<(std::ostream& out, Code const& c) {
        out << "ch " << c.ch << " [" << c.code_bit_length << "]" << bits::bits_to_string(c.code.data(), c.code_bit_length);
        return out;
    }

    static bool cmp_codes(Code const& a, Code const& b) {
        if (a.code_bit_length != b.code_bit_length)
            return false;
        for (size_t i = 0; i < bits::bit_size_to_byte(b.code_bit_length); ++i) {
           if (a.code[i] != b.code[i])
               return false;
        }
    };

    unsigned char ch;
    size_t frequence;
    size_t code_bit_length;
    std::array<unsigned char, 8> code;
};

struct HuffmanCoder {
    // TODO: write tree
    // TODO: test all
    // TODO: check files 'aaaaaaaa' and 'a' and '' and 'baaaaaaa'
    // TODO: change bitarray to smart-pointer/RAII
    // TODO: new [] to operator new to RAII
    // TODO: change array
    // TODO: HuffmanCode class to namespace
    // TODO: finish function
    // TODO: bitwriter.clear()
    // TODO: unify names
    // TODO: HuffmanTree to class
    // TODO: don't allocate dest in decode
    HuffmanData encode(char const* data_, size_t length);
    DecodeState decode(HuffmanData const &data, char* buffer, size_t& length);

 private:
    std::pair<std::vector<Code>, HuffmanData> getCodes(unsigned char const* buffer, size_t length);
};


#endif //HUFFMAN_HUFFMAN_H
