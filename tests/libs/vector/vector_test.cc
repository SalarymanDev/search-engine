#include "src/libs/vector/vector.h"
#include "tests/catch.hpp"

using BBG::vector;
using BBG::Buffer;
using BBG::serialize;
using BBG::deserialize;

TEST_CASE("Default constructor", "[vector]") {
    vector<int> v;
    vector<vector<double>> v2;
    REQUIRE( true );
}

TEST_CASE("Size constructor", "[vector]") {
    vector<int> v(10);
    REQUIRE(v.size() == 10);

    vector<vector<double>> v2(5);
    REQUIRE(v2.size() == 5);
}

TEST_CASE("Value fille constructor", "[vector]") {
    vector<int> v(10, 2);
    REQUIRE(v.size() == 10);
    REQUIRE(v[0] == 2);

    vector<vector<int>> v2(5, v);
    REQUIRE(v2.size() == 5);
    REQUIRE(v2[0][0] == 2);
}

TEST_CASE("Assignment operator", "[vector]") {
    vector<int> v(10, 5);
    vector<int> v2(5, 3);

    v = v2;
    for (size_t i = 0; i < v.size(); ++i) {
        REQUIRE(v[i] == v2[i]);
    }

    vector<vector<int>> v3(5, v);
    vector<vector<int>> v4(8, v2);

    v3 = v4;
    for (size_t i = 0; i < v3.size(); ++i) {
        for (size_t j = 0; j < v3[i].size(); ++j) {
            REQUIRE(v3[i][j] == v4[i][j]);
        }
    }
}

TEST_CASE("Push back new element", "[vector]") {
    vector<int> vec;
    vec.push_back(0);
    vec.push_back(1);

    for (size_t i = 0; i < vec.size(); ++i) {
        REQUIRE(i == vec[i]);
    }
}

TEST_CASE("Size and capacity", "[vector]") {
    vector<int> v;
    REQUIRE(v.size() == 0);
    REQUIRE(v.capacity() == 1);
    v.push_back(3);
    REQUIRE(v.size() == 1);
    REQUIRE(v.capacity() == 1);
    REQUIRE(*v.begin() == 3);
    v.push_back(5);
    REQUIRE(v.size() == 2);
    REQUIRE(v.capacity() == 2);
    v.push_back(5);
    REQUIRE(v.size() == 3);
    REQUIRE(v.capacity() == 4);
    v.pop_back();
    REQUIRE(v.size() == 2);
    REQUIRE(v.capacity() == 4);
    REQUIRE(v[0] == 3);
    REQUIRE(v[1] == 5);
    REQUIRE(v.size() == 2);
    REQUIRE(v.capacity() == 4);
    v[0] = 8;
    REQUIRE(*v.begin() == 8);
    v.reserve(6);
    REQUIRE(v.capacity() == 6);
    REQUIRE(v.size() == 2);

    vector<int> v2(4);
    REQUIRE(v2.size() == 4);
    REQUIRE(v2.capacity() == 4);
}

TEST_CASE("Move and copy constructor test", "[vector]") {
    vector<int> v;
    v.push_back(3);
    v.push_back(5);
    v[0] = 8;
    v.reserve(6);

    vector<int> v2(5, 7);
    REQUIRE(v2.size() == 5);
    REQUIRE(v2.capacity() == 5);
    REQUIRE(*v2.begin() == 7);
    REQUIRE(*(v2.end() - 1) == 7);
    for (auto ptr = v2.begin(); ptr != v2.end(); ++ptr)
        REQUIRE(*ptr == 7);
    
    vector<int> v3(v2);
    REQUIRE(v3.size() == 5);
    REQUIRE(v3.capacity() == 5);
    REQUIRE(*v3.begin() == 7);
    REQUIRE(*(v3.end() - 1) == 7);
    for (auto ptr = v3.begin(); ptr != v3.end(); ++ptr) 
        REQUIRE(*ptr == 7);
    
    vector<int> v4 = std::move(v3);
    REQUIRE(v4.size() == 5);
    REQUIRE(v4.capacity() == 5);
    REQUIRE(*v4.begin() == 7);
    REQUIRE(*(v4.end() - 1) == 7);
    for (auto ptr = v4.begin(); ptr != v4.end(); ++ptr) 
        REQUIRE(*ptr == 7);
    // ERROR, v3 invalid after move
    // REQUIRE(v3.capacity() == 1);
    // REQUIRE(v3.size() == 0);
    v4 = std::move(v);
    REQUIRE(v4.capacity() == 6);
    REQUIRE(v4.size() == 2);
    // ERROR, v invalid after move
    // REQUIRE(v.capacity() == 1);
    // REQUIRE(v.size() == 0);
}

TEST_CASE("clear", "[vector]") {
    vector<int> vec(5, 1);
    size_t cap = vec.capacity();
    auto begin_ptr = vec.begin();
    auto end_ptr = vec.end() - 1;
    REQUIRE(vec.size() == 5);
    vec.clear();
    REQUIRE(vec.size() == 0);
    REQUIRE(vec.capacity() == cap);
    REQUIRE(vec.begin() == begin_ptr);
    REQUIRE((vec.begin() + 4) == end_ptr);
    REQUIRE(*(vec.begin() + 4) == *end_ptr);
}

TEST_CASE("equal", "[vector]") {

    vector<int> vec1(5);
    vector<int> vec2(5);

    REQUIRE(vec1 == vec2);

    vector<vector<int>> vec3(6);
    vector<vector<int>> vec4(6);

    REQUIRE(vec3.size() == 6);

    for (size_t i = 0; i < vec3.size(); ++i) {
        for (size_t j = 0; j < 13; ++j) {
            vec3[i].push_back((int)(i*j));
            vec4[i].push_back((int)(i*j));
        }
    }

    REQUIRE(vec3 == vec4);
}

TEST_CASE("swap", "[vector]") {
    vector<int> vec1(5, 1);
    vector<int> vec2(10, 3);

    BBG::swap(vec1, vec2);

    REQUIRE(vec1.size() == 10);
    REQUIRE(vec2.size() == 5);
    for (size_t i = 0; i < vec1.size(); ++i)
        REQUIRE(vec1[i] == 3);
    for (size_t i = 0; i < vec2.size(); ++i)
        REQUIRE(vec2[i] == 1);


    vector<vector<int>> vec3(6);
    vector<vector<int>> vec4(6);

    for (size_t i = 0; i < vec3.size(); ++i) {
        for (size_t j = 0; j < 13; ++j) {
            vec3[i].push_back((int)(i*j + i));
            vec4[i].push_back((int)(i*j + j));
        }
    }

    BBG::swap(vec3, vec4);

    for (size_t i = 0; i < vec3.size(); ++i) {
        for (size_t j = 0; j < 13; ++j) {
            REQUIRE(vec3[i][j] == (int)(i*j + j));
            REQUIRE(vec4[i][j] == (int)(i*j + i));
        }
    }
}

TEST_CASE("Buffer constructor", "[vector]") {
    int buff[3] = {1, 2, 3};
    vector<int> vec(buff, 3);
    REQUIRE(vec[0] == 1);
    REQUIRE(vec[1] == 2);
    REQUIRE(vec[2] == 3);
}

TEST_CASE("+= Operator to append vector to vector", "[vector]") {
    vector<int> toAdd;
    toAdd.push_back(3);
    toAdd.push_back(4);
    toAdd.push_back(5);

    vector<int> actual;
    actual.push_back(1);
    actual.push_back(2);

    vector<int> expected;
    expected.push_back(1);
    expected.push_back(2);
    expected.push_back(3);
    expected.push_back(4);
    expected.push_back(5);

    actual += toAdd;
    REQUIRE(actual == expected);
}

TEST_CASE("Serialize/Deserialize Vector", "[vector]") {
    vector<int> expected;
    expected.push_back(1);
    expected.push_back(2);
    expected.push_back(3);

    Buffer bytes;
    serialize(expected, bytes);
    vector<int> actual;
    deserialize(bytes, actual);
    REQUIRE(actual == expected);
}

TEST_CASE("emplace_back simple", "[vector]") {
    vector<int> v;

    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    v.emplace_back(4);
    v.emplace_back(5);

    REQUIRE(v.size() == 5);
}

TEST_CASE("emplace_back complex", "[vector]") {

    vector<vector<int>> v;

    v.emplace_back(1,2);
    v.emplace_back(2,3);

    REQUIRE(v.size() == 2);
    REQUIRE(v[0].size() == 1);
    REQUIRE(v[1].size() == 2);

    REQUIRE(v[0].back() == 2);
    REQUIRE(v[1].back() == 3);
}
