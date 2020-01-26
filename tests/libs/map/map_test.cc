#include "src/libs/map/map.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "tests/catch.hpp"

#include <iostream>
#include <unordered_set>

using BBG::Map;
using BBG::MapEntry;
using BBG::string;
using BBG::vector;
using BBG::Buffer;
using BBG::serialize;
using BBG::deserialize;

TEST_CASE("Capacity constructor w/ primitives", "[map]") {
  Map<int, int> intToIntMap(1110);
  Map<char*, long> ptrToLongMap(123);
  Map<long long, char*> vLongToPtrMap(2);

  REQUIRE(intToIntMap.capacity() == 1110);
  REQUIRE(ptrToLongMap.capacity() == 123);
  REQUIRE(vLongToPtrMap.capacity() == 2);

  REQUIRE(true);
}

TEST_CASE("Capacity constructor w/ our data types", "[map]") {
  Map<int, string> intToStringMap(1);
  Map<string, int> stringToIntMap(101);

  REQUIRE(intToStringMap.size() == 0);
  REQUIRE(stringToIntMap.size() == 0);
  REQUIRE(intToStringMap.capacity() == 1);
  REQUIRE(stringToIntMap.capacity() == 101);

  Map<int, vector<int>> intToVectorMap(99999);
  Map<vector<int>, int> vectorToIntMap(1234);
  
  REQUIRE(intToVectorMap.size() == 0);
  REQUIRE(vectorToIntMap.size() == 0);
  REQUIRE(intToVectorMap.capacity() == 99999);
  REQUIRE(vectorToIntMap.capacity() == 1234);

  Map<vector<string>, string> vectorToStringMap(98765);
  Map<string, vector<string>> stringToVectorMap(98765);

  REQUIRE(vectorToStringMap.size() == 0);
  REQUIRE(stringToVectorMap.size() == 0);
  REQUIRE(vectorToStringMap.capacity() == 98765);
  REQUIRE(stringToVectorMap.capacity() == 98765);

  REQUIRE(true);
}

TEST_CASE("Default constructor w/ primitives", "[map]") {
  Map<int, int> intToIntMap;
  Map<char*, long> ptrToLongMap;
  Map<long long, char*> vLongToPtrMap;

  REQUIRE(true);
}

TEST_CASE("Default constructor w/ our data types", "[map]") {
  Map<int, string> intToStringMap;
  Map<string, int> stringToIntMap;

  Map<int, vector<int>> intToVectorMap;
  Map<vector<int>, int> vectorToIntMap;
  
  Map<vector<string>, string> vectorToStringMap;
  Map<string, vector<string>> stringToVectorMap;

  REQUIRE(true);
}

TEST_CASE("Default size and capacity", "[map]") {
  Map<char, char> m;
  REQUIRE(m.size() == 0);
  REQUIRE(m.capacity() == 128);
  REQUIRE(m.empty() == true);
}

TEST_CASE("Access and set value of an element", "[map]") {
  Map<int, int> m;
  m[3] = 3012;
  REQUIRE(m.size() == 1);
  REQUIRE(m.empty() == false);
  REQUIRE(m[3] == 3012);
}

TEST_CASE("Add new elements with insert", "[map]") {
	Map<int, double> m;
	int values[] = {10, 443, 1, 13, 0, 999, 1231, 44};
	int numValues = 8;

	for (int i = 0; i < numValues; ++i)
		m.insert(values[i], values[i] * 12.3456);

	REQUIRE(m.size() == numValues);

	for (int i = 0; i < numValues; ++i)
		REQUIRE(m[values[i]] == values[i] * 12.3456);

	REQUIRE(m.size() == numValues);
}

TEST_CASE("Multiple inserts with the same element", "[map]") {
  Map<int, double> m;
	int values[] = {10, 443, 1, 13, 0, 999, 1231, 44};
	int numValues = 8;

	for (int i = 0; i < numValues; ++i) {
		m.insert(values[i], values[i] * 12.3456);
		m.insert(values[i], values[i]);
		m.insert(values[i], values[i]);
		m.insert(values[i], values[i]);
  }

	for (int i = 0; i < numValues; ++i)
		REQUIRE(m[values[i]] == values[i] * 12.3456);

	REQUIRE(m.size() == numValues);
}

TEST_CASE("Add and modify existing elements with [] operator", "[map]") {
  Map<double, double> m;
  double values[] = {10, 443, 1, 13, 0, 999, 1231, 44};
	int numValues = 8;

	for (int i = 0; i < numValues; ++i)
		m[values[i]] = values[i];

	REQUIRE(m.size() == numValues);

	for (int i = 0; i < numValues; ++i) {
		REQUIRE(m[values[i]] == values[i]);
		m[values[i]] *= 12.34567;
		REQUIRE(m[values[i]] == values[i] * 12.34567);
  }

	REQUIRE(m.size() == numValues);
}

TEST_CASE("Map iteration capabilities", "[map]") {
  Map<double, double> m;
  std::unordered_set<double> values = {10, 443, 1, 13, 0, 999, 1231, 44};
	int numValues = 8;

	for (auto v : values)
		m[v] = v;

	REQUIRE(m.size() == numValues);

	for (auto elem = m.begin(); elem; elem = m.next()) {
    REQUIRE(elem->key == elem->val);
    REQUIRE(values.find(elem->val) != values.end());
  }

	REQUIRE(m.size() == numValues);

  REQUIRE(m.next() == nullptr);
  REQUIRE(m.next() == nullptr);
  REQUIRE(m.next() == nullptr);
  REQUIRE(m.next() == nullptr);

  for (auto elem = m.begin(); elem; elem = m.next()) {
    REQUIRE(elem->key == elem->val);
    REQUIRE(values.find(elem->val) != values.end());
    values.erase(elem->val);
  }

  REQUIRE(values.empty());

  REQUIRE(m.next() == nullptr);
  REQUIRE(m.next() == nullptr);
  REQUIRE(m.next() == nullptr);
  REQUIRE(m.next() == nullptr);

	REQUIRE(m.size() == numValues);
}

TEST_CASE("Check map contains", "[map]") {
	Map<double, double> m(12);
  double values[] = {10, 443, 1, 13, 0, 999, 1231, 44};
	int numValues = 8;

	for (int i = 0; i < numValues; ++i)
		m[values[i]] = values[i] * 12.3456;

	REQUIRE(m.size() == numValues);

	for (int i = 0; i < numValues; ++i) {
	  REQUIRE(m.contains(values[i]));
	  if (values[i] != 0)
      REQUIRE(!m.contains(values[i] * 12.3456));
  }

  REQUIRE(!m.contains(-1));
  REQUIRE(!m.contains(-443));
  REQUIRE(!m.contains(2));
  REQUIRE(!m.contains(9999));
	REQUIRE(m.size() == numValues);
}

TEST_CASE("Clear map then use it", "[map]") {
	int values[] = {10, 443, 1, 13, 0, 999, 1231, 44};
	int numValues = 8;
	int capacity = 10;
  Map<int, double> m(capacity);

  for (int i = 0; i < numValues; ++i)
		m[values[i]] = values[i] * 12.3456;

	REQUIRE(m.size() == numValues);
  REQUIRE(m.capacity() == capacity * 2);

	m.clear();

	REQUIRE(m.size() == 0);
	REQUIRE(m.capacity() == capacity * 2);

  for (int i = 0; i < numValues; ++i) {
		REQUIRE(!m.contains(values[i]));
		m[values[i]] = values[i];
  }

	REQUIRE(m.size() == numValues);

	for (int i = 0; i < numValues; ++i) {
		REQUIRE(m[values[i]] == values[i]);
		m[values[i]] *= 12;
		REQUIRE(m[values[i]] == values[i] * 12);
  }

  REQUIRE(m.size() == numValues);
}

TEST_CASE("Check map get", "[map]") {
	Map<double, double> m;
  double values[] = {10, 443, 1, 13, 0, 999, 1231, 44};
	int numValues = 8;

	for (int i = 0; i < numValues; ++i)
		m[values[i]] = values[i] * 12.3456;

	REQUIRE(m.size() == numValues);

	for (int i = 0; i < numValues; ++i) {
	  REQUIRE(m.get(values[i]));
	  if (values[i] != 0)
      REQUIRE(!m.get(values[i] * 12.3456));
  }

  REQUIRE(!m.get(-1));
  REQUIRE(!m.get(-443));
  REQUIRE(!m.get(2));
  REQUIRE(!m.get(9999));
	REQUIRE(m.size() == numValues);
}

TEST_CASE("Delete elements", "[map]") {
  Map<int, double> m(12);
	int values[] = {10, 443, 1, 13, 0, 999, 1231, 44};
	int finalValues[] = {443, 1, 13, 999, 44};
	int numValues = 8;
	int numFinalValues = 5;

	for (int i = 0; i < numValues; ++i)
		m[values[i]] = values[i] * 12.3456;

	REQUIRE(m.size() == numValues);

	m.erase(10);
	REQUIRE(!m.contains(10));
	REQUIRE(m.size() == numValues - 1);
	
	m.erase(0);
	REQUIRE(!m.contains(0));
	REQUIRE(m.size() == numValues - 2);

	m.erase(1231);
	REQUIRE(!m.contains(1231));
	REQUIRE(m.size() == numValues - 3);

	for (int i = 0; i < numFinalValues; ++i)
    REQUIRE(m[finalValues[i]] == finalValues[i] * 12.3456);

	REQUIRE(m.size() == numValues - 3);
}

TEST_CASE("Delete on empty map", "[map]") {
  Map<int, double> m;
  m.erase(0);
  REQUIRE(m.size() == 0);
  REQUIRE(m.empty() == true);

  m.erase(123);
  REQUIRE(m.size() == 0);
  REQUIRE(m.empty() == true);
}

TEST_CASE("Delete non existent elements", "[map]") {
  Map<int, double> m(10);
  int values[] = {10, 443, 0, 999, 1231, 44};
  int otherValues[] = {1110, 123, -123, 13, 99999, 9};
	int numValues = 6;

	for (int i = 0; i < numValues; ++i)
		m[values[i]] = values[i] * 12.3456;

  REQUIRE(m.size() == numValues);

	for (int i = 0; i < numValues; ++i) {
    m.erase(otherValues[i]);
    REQUIRE(!m.contains(otherValues[i]));
    REQUIRE(m.size() == numValues);
  }

  REQUIRE(m.size() == numValues);
}

TEST_CASE("Delete and re add element (shouldn't save value)", "[map]") {
  Map<int, double> m(10);
  int values[] = {10, 443, 0, 999, 1231, 44};
	int numValues = 6;

	for (int i = 0; i < numValues; ++i)
		m[values[i]] = values[i] * 12.3456;

	for (int i = 0; i < numValues; ++i) {
		m.erase(values[i]);
    REQUIRE(m.size() == numValues - 1);
    REQUIRE(m[values[i]] == 0);
    REQUIRE(m.size() == numValues);
  }
}

TEST_CASE("Assignment operator", "[map]") {
  Map<int, int> map;
	int values[] = {10, 443, 1, 13, 0, 999, 1231, 44};
	int numValues = 8;

	for (int i = 0; i < numValues; ++i) {
		map[values[i]] = values[i];
	}

	Map<int, int> copyMap = map;

	for (int i = 0; i < numValues; ++i) {
		REQUIRE(map[values[i]] == copyMap[values[i]]);
	}
}

TEST_CASE("Force rehash and grow x2", "[map]") {
  Map<int, double> m;

  // CAPACITY: 128
  // MAX_LOAD_FACTOR: 0.7
  // 128 * 0.7 = 89.6
  // Map should grow at 90th insert
  for (int i = 0; i < 89; ++i)
		m[i] = i * 12.3456;

	REQUIRE(m.size() == 89);
	REQUIRE(m.capacity() == 128);

  m[999] = 999 * 12.3456;

  for (int i = 0; i < 89; ++i)
		REQUIRE(m[i] == i * 12.3456);

	REQUIRE(m.size() == 90);
	REQUIRE(m.capacity() == 256);

  // CAPACITY: 256
  // MAX_LOAD_FACTOR: 0.7
  // 256 * 0.7 = 179.2
  // Map should grow at 180th insert
  for (int i = 90; i < 179; ++i)
		m[i] = i * 12.3456;

	REQUIRE(m.size() == 179);
	REQUIRE(m.capacity() == 256);

  m[9999] = 9999 * 12.3456;

  for (int i = 90; i < 179; ++i)
		REQUIRE(m[i] == i * 12.3456);

	REQUIRE(m.size() == 180);
	REQUIRE(m.capacity() == 512);
}

TEST_CASE("General use 1", "[map]") {
  Map<int, double> m;

  for (int i = 0; i < 350; ++i)
    m[i] = i * 32.1098;

  for (int i = 0; i < 350; i += 2)
    m.erase(i);

  for (int i = 0; i < 350; ++i) {
    if (i % 2 == 0)
      REQUIRE(!m.contains(i));
    else
      REQUIRE(m[i] == i * 32.1098);
  }
}

TEST_CASE("Test with custom types", "[map]") {

  int numElems = 9;

  string testStr[] = {
    "",
    "1",
    "test 10 a b",
    "test 100 a b",
    "test 1000 a b",
    "test 20 a b",
    "test 200 a b",
    "test 2000 a b",
    "test 99999 a b"
  };

  int testInt[] = {0, 1, 10, 100, 1000, 20, 200, 2000, 99999};

  SECTION("Test with string keys") {
    Map<string, int> m(10);

    for (int i = 0; i < numElems; ++i)
      m[testStr[i]] = testInt[i];

    for (int i = 0; i < numElems; ++i)
      REQUIRE(m[testStr[i]] == testInt[i]);
  }

  SECTION("Test with string values") {
    Map<int, string> m(10);

    for (int i = 0; i < numElems; ++i)
      m[testInt[i]] = testStr[i];

    for (int i = 0; i < numElems; ++i)
      REQUIRE(m[testInt[i]] == testStr[i]);
  }

  SECTION("Test with string keys and values") {
    Map<string, string> m(10);

    for (int i = 0; i < numElems; ++i)
      m[testStr[i]] = testStr[i];

    for (int i = 0; i < numElems; ++i)
      REQUIRE(m[testStr[i]] == testStr[i]);
  }

  SECTION("Test with vector values") {
    Map<int, vector<string>> m(2);
    m[123].push_back("test 123 0");
    m[123].push_back("test 123 1");
    m[123].push_back("test 123 2");
    m[123].push_back("test 123 3");
    m[1];
    m[22].push_back("test 22 0");
    m[333].push_back("test 333 0");
    m[333].push_back("test 333 1");
    m[333].push_back("test 333 2");

    REQUIRE(m[123][0] == "test 123 0");
    REQUIRE(m[123][1] == "test 123 1");
    REQUIRE(m[123][2] == "test 123 2");
    REQUIRE(m[123][3] == "test 123 3");
    REQUIRE(m[123].size() == 4);
    REQUIRE(m[1].size() == 0);
    REQUIRE(m[22][0] == "test 22 0");
    REQUIRE(m[22].size() == 1);
    REQUIRE(m[333][0] == "test 333 0");
    REQUIRE(m[333][1] == "test 333 1");
    REQUIRE(m[333][2] == "test 333 2");
    REQUIRE(m[333].size() == 3);
  }

  SECTION("Test with string keys and vector values") {
    Map<string, vector<string>> m(2);
    m[testStr[0]].push_back("test 123 0");
    m[testStr[0]].push_back("test 123 1");
    m[testStr[0]].push_back("test 123 2");
    m[testStr[0]].push_back("test 123 3");
    m[testStr[1]];
    m[testStr[2]].push_back("test 22 0");
    m[testStr[3]].push_back("test 333 0");
    m[testStr[3]].push_back("test 333 1");
    m[testStr[3]].push_back("test 333 2");

    REQUIRE(m[testStr[0]][0] == "test 123 0");
    REQUIRE(m[testStr[0]][1] == "test 123 1");
    REQUIRE(m[testStr[0]][2] == "test 123 2");
    REQUIRE(m[testStr[0]][3] == "test 123 3");
    REQUIRE(m[testStr[0]].size() == 4);
    REQUIRE(m[testStr[1]].size() == 0);
    REQUIRE(m[testStr[2]][0] == "test 22 0");
    REQUIRE(m[testStr[2]].size() == 1);
    REQUIRE(m[testStr[3]][0] == "test 333 0");
    REQUIRE(m[testStr[3]][1] == "test 333 1");
    REQUIRE(m[testStr[3]][2] == "test 333 2");
    REQUIRE(m[testStr[3]].size() == 3);
  }
}

// This seems to be the biggest map i can build without
// bazel shitting itself (or just timing out)
// TODO: GG lads this times out on Gitlab CI
/* TEST_CASE("Build huge map", "[map]") { */
/*   SECTION("Long long to string") { */
/*     Map<long long, string> llToStringMap(1024); // 2^10 */
/*     for (long long i = 0; i < 11744000; ++i) { */
/*       REQUIRE(!llToStringMap.contains(i)); */
/*       llToStringMap.insert(i, "test"); */
/*       if (i % 1000 == 0) { */
/*         std::cout << i << '\n'; */
/*         REQUIRE(llToStringMap.size() == i+1); */
/*         REQUIRE(llToStringMap[i] == "test"); */
/*         REQUIRE(llToStringMap.contains(i)); */
/*       } */
/*     } */
/*   } */
/*   REQUIRE(true); */
/* } */

TEST_CASE("==/!= Operator MapEntry Test", "[map]") {
  MapEntry<int, int> actual, expected;
  actual.hash = 50;
  expected.hash = 50;
  actual.dist = 30;
  expected.dist = 30;
  actual.key = 10;
  expected.key = 10;
  actual.val = 90;
  expected.val = 90;
  REQUIRE(actual == expected);

  actual.val = 55;
  REQUIRE(actual != expected);
}

TEST_CASE("==/!= Operator Map Test", "[map]") {
  Map<int, int> actual, expected;
  actual[0] = 9;
  actual[5] = 97;
  actual[300] = 9023;
  expected[300] = 9023;
  expected[5] = 97;
  expected[0] = 9;
  REQUIRE(actual == expected);

  actual[5] = 0;
  REQUIRE(actual != expected);
  actual[5] = 97;

  actual[40] = 30;
  REQUIRE(actual != expected);
}

TEST_CASE("Serialize/Deserialize MapEntry", "[map]") {
  MapEntry<string, int> expected;
  expected.hash = 50;
  expected.dist = 20;
  expected.key = "test";
  expected.val = 7;

  Buffer bytes;
  serialize(expected, bytes);
  MapEntry<string, int> actual;
  deserialize(bytes, actual);
  REQUIRE(actual == expected);
}

TEST_CASE("Serialize/Deserialize Map", "[map]") {
  int numElems = 9;

  string testStr[] = {
    "",
    "1",
    "test 10 a b",
    "test 100 a b",
    "test 1000 a b",
    "test 20 a b",
    "test 200 a b",
    "test 2000 a b",
    "test 99999 a b"
  };

  int testInt[] = {0, 1, 10, 100, 1000, 20, 200, 2000, 99999};

  Map<string, int> expected(10);
  for (int i = 0; i < numElems; ++i)
    expected[testStr[i]] = testInt[i];

  Buffer bytes;
  serialize(expected, bytes);
  Map<string, int> actual;
  deserialize(bytes, actual);

  REQUIRE(expected == actual);
}
