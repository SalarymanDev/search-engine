#ifndef RANK_H
#define RANK_H

#include <cstddef> // size_t
#include <cassert>
#include <math.h>
#include "src/libs/vector/vector.h"
#include "src/libs/string/string.h"
#include "src/libs/map/map.h"
#include "src/libs/document/Document.h" 


// TODO:  map: word -> term_id
//        map: term_id -> idf
//        map: term_id -> (map: url -> tf)
//        map: url -> norm

// To get n_k take the size of the 3rd map[term_id].size()


// 1) First get bow ... tolower, isalphanumeric

// 1) calculate N

// 2) Build tf map.
//      Loop through each document
//          get the term_id with bow[term] for each word in titleText and bodyText
//          3rdmap[term_id][doc.url].tf += 1

// 3) calc n_k
//      create tmp map: term_id -> n_k
//      Loop over each term in bow
//          tmpmap[term_id] += 3rdmap[term_id].size()    

// 4) calc idf map
//      Loop over each term in bow
//          2nd map [term_id] = log(N/tmpmap[term_id])


// 5) calc norm
//      Loop over documents in gdfs
//          tmpsum = 0
//          for each term in titleText and bodyText
//              tmpsum += 3rdmap[bow[term]][doc.url]^2 * idfmap[bow[term]]^2
//          normmap[doc.url] = sqrt(tmpsum)


namespace BBG {

    // Math functions
    template <typename T>
    T sum(const vector<T>& vec) {
        T result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i];
        return result;
    }

    template <typename T>
    T mean(const vector<T>& vec) {
        return sum(vec) / (T) vec.size();
    }

    template <typename T>
    T dotprod(const vector<T>& vec1, const vector<T>& vec2) {
        assert(vec1.size() == vec2.size());
        T result = 0;
        for (size_t i = 0; i < vec1.size(); ++i)
            result += vec1[i] * vec2[i];
        return result;
    }

    template <typename T>
    float l2norm(const vector<T>& vec) {
        float result = 0;
        for (size_t i = 0; i < vec.size(); ++i)
            result += vec[i] * vec[i];
        return sqrt(result);
    }

    float absoluteValue(float A);
    bool compareFloat(float A, float B, float epsilon = 0.005f);
}

#endif
