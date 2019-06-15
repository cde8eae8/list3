#include <iostream>
#include <cstring>
#include <fstream>
#include <iterator>
#include "huffman.h"

const size_t BLOCK_SIZE = 4096;
const size_t MAX_TREE_SIZE = 256 * 2 / 8;


void printUsage(char* program_name) {
    printf("Usage: %s [-e|-d] input_file output_file\n", program_name);
}


void write_size_t(std::ostream& out, size_t value) {
    char* ptr = static_cast<char*>(static_cast<void*>(&value));
    out.write(ptr, sizeof(value));
}

void encode(std::string input_file, std::string output_file) {
   std::ifstream in(input_file, std::ios::binary | std::ios::in);
   std::ofstream out(output_file, std::ios::binary | std::ios::out);

   char buffer[BLOCK_SIZE];
   while (in) {
       in.read(buffer, BLOCK_SIZE);
       HuffmanCoder hc;
       if (in.gcount() == 0)
           return;
       HuffmanData hd = hc.encode(buffer, in.gcount());
       write_size_t(out, hd.tree.bit_length());
       out.write(hd.tree.data(), hd.tree.byte_length());
       write_size_t(out, hd.used_chars.length());
       out.write(hd.used_chars.array(), hd.used_chars.length());
       write_size_t(out, hd.code.bit_length());
       out.write(hd.code.data(), hd.code.byte_length());
   }
   in.close();
   out.close();
}

size_t read_size_t(std::istream& in) {
    size_t value;
    char* ptr = static_cast<char*>(static_cast<void*>(&value));
    if (!in.read(ptr, sizeof(value))) {
        return 0;
    }
    return value;
}


struct buffer {
    buffer(size_t size, std::istream& in) {
        if (in.eof()) {
            data_ = nullptr;
            return;
        }
        data_ = static_cast<char*>(operator new(size));
        in.read(data_, size);
    }

    buffer(size_t size) {
        data_ = static_cast<char*>(operator new(size));
    }

    ~buffer() {
        operator delete(data_);
    }

    char* data() {
        return data_;
    }

 private:
    char* data_;
};


void decode(std::string input_file, std::string output_file) {
    std::ifstream in(input_file, std::ios::binary | std::ios::in);
    std::ofstream out(output_file, std::ios::binary | std::ios::out);
    if (!in.is_open()) std::cout << "error" << std::endl;

    HuffmanCoder hc;
    while (in) {
        size_t tree_length;
        tree_length = read_size_t(in);
        if (tree_length > MAX_TREE_SIZE) {
            std::cout << "error: file is corrupted" << std::endl;
//            continue;
            return;
        }
        buffer b(tree_length / 8 + (tree_length % 8 != 0), in);
        bitarray tree(b.data(), tree_length / 8, tree_length % 8);
        size_t n_used_chars;
        n_used_chars = read_size_t(in);
        if (n_used_chars > 256) {
            std::cout << "error: file is corrupted" << std::endl;
//            continue;
            return;
        }
        buffer chars(n_used_chars);
        in.read(chars.data(), n_used_chars);

        HuffmanData hd(tree, bytearray(chars.data(), n_used_chars));

        size_t code_length;
        code_length = read_size_t(in);
        if (code_length > BLOCK_SIZE) {
            std::cout << "error: file is corrupted" << std::endl;
//            continue;
            return;
        }

        buffer b2(code_length / 8 + (code_length % 8 != 0), in);
        bitarray code(b2.data(), code_length / 8, code_length % 8);
        hd.code = code;

        char* buffer = static_cast<char *>(operator new(BLOCK_SIZE));
        size_t length = BLOCK_SIZE;
        if (hc.decode(hd, buffer, length) != DecodeState::SUCCESS) {
           std::cout << "error: file is corrupted" << std::endl;
           return;
        }
        out.write(buffer, length);
        operator delete(buffer);
    }

    out.close();
    in.close();
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printUsage(argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "-e") == 0) {
        encode(argv[2], argv[3]);
    } else if (strcmp(argv[1], "-d") == 0) {
        decode(argv[2], argv[3]);
    } else {
        printUsage(argv[0]);
        return -1;
    }
    return 0;
}
