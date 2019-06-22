//
// Created by nikita on 17.06.19.
//

#ifndef EXAM1_REFCOUNTER_H
#define EXAM1_REFCOUNTER_H

// ch 19:27
// ch 19:55 e
// ch 12:51
// ch 14:27 e
// ch 14:55


#include <glob.h>
#include <string>

struct ref_data {
    ref_data() : nRefs(1), id(i++), info(std::to_string(id)) {}
    ref_data(ref_data const&) = delete;
    ref_data& operator=(ref_data const&) = delete;
    ~ref_data() {
        nRefs--;
    }

    static size_t i;

    size_t nRefs;
    size_t id;
    std::string info;
};


#endif //EXAM1_REFCOUNTER_H
