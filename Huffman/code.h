//
// Created by nikita on 07.06.19.
//


#ifndef HUFFMAN_CODE_H
#define HUFFMAN_CODE_H

#include <cstddef>
#include <memory>

struct Header {
    size_t data_length;
};


/**
 * Base class for representation of all encoded results
 */
struct PlainData {
    size_t data_length;
    char* data;
    virtual ~PlainData() = 0;
};


struct RaiiData {
 public:
    RaiiData(PlainData* data) : data_(data) {}

    ~RaiiData() {
        delete data_;
    }

    PlainData* release() {
        auto tmp = data_;
        data_ = nullptr;
        return tmp;
    }

 private:
    PlainData* data_;
};


struct Coder {
    virtual PlainData* encode(char* buffer, size_t length) = 0;
    virtual void decode(PlainData* data, char* buffer, size_t* length) = 0;
};

#endif //HUFFMAN_CODE_H
