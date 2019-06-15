#include <iostream>
#include <cstring>
#include <fstream>
#include <iterator>
#include "huffman.h"

const size_t BLOCK_SIZE = 4096;


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
       HuffmanData hd = hc.encode(buffer, in.gcount());

       ///
       write_size_t(out, hd.tree.bit_length());

       std::cout << "block\n" <<
                        "tree\t" << hd.tree << std::endl;
       for (size_t i = 0; i < hd.n_used_chars; ++i) {
           std::cout << hd.used_chars[i] << " ";
       }
       std::cout << std::endl;
       std::cout << "code\t" << hd.code << std::endl;


       std::cout << "written " << sizeof(hd.tree.bit_length()) << std::endl;

       ///
       out.write(hd.tree.data(), hd.tree.byte_length());

       std::cout << "written " << hd.tree.byte_length() << std::endl;

       std::cout << "pos" << out.tellp() << std::endl;
       ///
       write_size_t(out, hd.n_used_chars);

       std::cout << hd.n_used_chars << std::endl;
       std::cout << "written " << sizeof(hd.n_used_chars) << std::endl;

       ///
       out.write(hd.used_chars, hd.n_used_chars);

       std::cout << "written " << hd.n_used_chars << std::endl;

       ///
       write_size_t(out, hd.code.bit_length());

       std::cout << "written " << sizeof(hd.tree.bit_length()) << std::endl;
      ///
       out.write(hd.code.data(), hd.code.byte_length());

       std::cout << "written " << hd.code.byte_length() << std::endl;
   }
   in.close();
   out.close();
}

size_t read_size_t(std::istream& in) {
    size_t value;
    char* ptr = static_cast<char*>(static_cast<void*>(&value));
    in.read(ptr, sizeof(value));
    return value;
}


void decode(std::string input_file, std::string output_file) {
    std::ifstream in(input_file, std::ios::binary | std::ios::in);
    std::ofstream out(output_file, std::ios::binary | std::ios::out);
    if (!in.is_open()) std::cout << "error" << std::endl;

    HuffmanCoder hc;
    while (in) {
        size_t tree_length;
        tree_length = read_size_t(in);
        ///
        bitarray tree(std::istreambuf_iterator<char>(in), tree_length / 8, tree_length % 8);

        std::cout << tree << std::endl;

        size_t n_used_chars;
        std::cout << "pos" <<in.tellg() << std::endl;
        ///
        n_used_chars = read_size_t(in);
        std::cout << n_used_chars << std::endl;
        char *chars = static_cast<char *>(operator new(n_used_chars));
        ///
        in.read(chars, n_used_chars);

        HuffmanData hd(tree, chars, n_used_chars);

        std::cout << "block\n" <<
                  "tree len \t" << tree_length << "\n"
                  "tree\t" << hd.tree << std::endl;
        for (size_t i = 0; i < hd.n_used_chars; ++i) {
            std::cout << hd.used_chars[i] << " ";
        }
        std::cout << std::endl;
        size_t code_length;
        ///
//        in >> code_length;
        code_length = read_size_t(in);

        std::cout << code_length << std::endl;
        bitarray code(std::istreambuf_iterator<char>(in), code_length);
        hd.code = code;

        std::cout << "code\t" << hd.code << std::endl;


        char* buffer = static_cast<char *>(operator new(BLOCK_SIZE));
        size_t length = BLOCK_SIZE;
        if (hc.decode(hd, buffer, length) != DecodeState::SUCCESS) {
           std::cout << "Some errors" << std::endl;
           return;
        }
        out.write(buffer, length);
        operator delete(buffer);
        break;
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
