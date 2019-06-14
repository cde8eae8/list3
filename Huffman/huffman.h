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
#include "code.h"
#include "bit_operations.h"
#include "bitwriter.h"


// TODO: encode(input_iterator it)?
struct HuffmanData {
    HuffmanData() : tree(), used_chars(), n_used_chars(), code() {}
    HuffmanData(bitarray const& tree_, char* used, size_t n_used, bitarray code_) :
                           tree(tree_), used_chars(used), n_used_chars(n_used), code(code_) {}
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
    HuffmanData encode(char const* data_, size_t length) {
        unsigned char const *data = static_cast<unsigned char const*>(static_cast<void const*>(data_));
        auto tmp = getCodes(data, length);
        auto codes = tmp.first;
        auto huffmanData = tmp.second;
        bitwriter bw{};
        for (size_t i = 0; i < length; ++i) {
            bw.write(codes[data[i]].code.data(), codes[data[i]].code_bit_length);
        }
        huffmanData.code = bw.reset();
        return huffmanData;
    }


    std::vector<unsigned char> decode(HuffmanData const& data, char* dest, size_t length);

    std::vector<unsigned char> decode(std::vector<Code> codes, bitarray const& src) {
       bitreader br(src);
       bitwriter bw;
       size_t pos = 0;
       std::vector<unsigned char> result;
       size_t i = 0;
       while(!br.eob()) {
           bw.push_back(br.get());
           auto cur_code = bw.get();
           i++;
           for (const auto &code : codes) {
               if (code.code_bit_length != cur_code.length * 8 + cur_code.tail)
                   continue;
               bool flag = true;
               for (size_t i = 0; i < bits::byte_to_length(cur_code.length, cur_code.tail); ++i) {
                  if (code.code[i] != cur_code.data_[i]) {
                      flag = false;
                      break;
                  }
               }
               if (!flag) continue;
               result.push_back(code.ch);
               bw.reset();
               break;
           }
       }
       return result;
    }

 private:
    std::pair<std::vector<Code>, HuffmanData> getCodes(unsigned char const* buffer, size_t length);

};


#endif //HUFFMAN_HUFFMAN_H
