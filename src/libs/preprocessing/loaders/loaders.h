#ifndef LOADERS_H
#define LOADERS_H

#include "src/libs/map/map.h"
#include "src/libs/string/string.h"

namespace BBG {

    bool loadBow(const char *filename, Map<string, uint32_t>& bow_out);
    bool loadTF(const char *filename, Map<uint32_t, Map<string, uint32_t>> &tf_out);
    bool loadIDF(const char *filename, Map<uint32_t, float>& IDF);
    bool loadNorm(const char *filename, Map<string, float>& NORM);

}

#endif