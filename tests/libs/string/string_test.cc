#define CATCH_CONFIG_MAIN
#include "tests/catch.hpp"
#include "src/libs/string/string.h"

using BBG::string;
using BBG::Buffer;
using BBG::getLine;
using BBG::serialize;
using BBG::deserialize;

const size_t npos = -1;
// unable to reach BBG::npos for some reason

TEST_CASE("Default constructor", "[string]") {
	string str;
	REQUIRE(true);
}

TEST_CASE("Copy constructor", "[string]") {
	string str;
	string str2(str);
	REQUIRE( true );

	string str3;
	str3.push_back('a');
	str3.push_back('b');
	str3.push_back('c');
	string str4(str3);
	REQUIRE( true );
}

TEST_CASE("Assignment operator", "[string]") {
	const char* s = "ABC";
	string str = s;

	REQUIRE(!strcmp(str.c_str(), s));

	REQUIRE(str.c_str() != s);
	// not using == operator to avoid potential bugs with it

	str += 'D';

	REQUIRE(strcmp(str.c_str(), s));
	str = s;
	string str2 = str;
	REQUIRE(!strcmp(str.c_str(), str2.c_str()));
	REQUIRE(str.c_str() != str2.c_str()); // pointers should be different

	str = "DEF"; // str2 is not modified
	REQUIRE(strcmp(str.c_str(), str2.c_str()));

	string str5;
	string str6 = str5;

	REQUIRE(str5 == str6);
}

TEST_CASE("Char constructor", "[string]") {
	char arr[] = { 'a', 'b', 'c', '\0' };
	string str(arr);
	REQUIRE(str[0] == 'a');
	REQUIRE(str[1] == 'b');
	REQUIRE(str[2] == 'c');

	char *a = new char[4];
	a[0] = 'd';
	a[1] = 'e';
	a[2] = 'f';
	string str2(a);
	REQUIRE(a[0] == 'd');
	REQUIRE(a[1] == 'e');
	REQUIRE(a[2] == 'f');
	delete[] a;
}

TEST_CASE("Buffer constructor", "[string]") {
	char buff[] = {'0', '1', '2'};
	string actual(buff, 3);
	string expected("012");
	REQUIRE(actual == expected);
}

TEST_CASE("Default destructor", "[string]") {
	string *str = new string();
	str->push_back('a');
	str->push_back('b');
	str->push_back('c');
	REQUIRE((*str)[0] == 'a');
	REQUIRE((*str)[1] == 'b');
	REQUIRE((*str)[2] == 'c');

	delete str;
}


TEST_CASE("size", "[string]") {
	string str;
	REQUIRE(str.size() == 0);
	str.push_back('a');
	str.push_back('b');
	REQUIRE(str.size() == 2);
	str.push_back('c');
	REQUIRE(str.size() == 3);
	str.pop_back();
	REQUIRE(str.size() == 2);
	str.pop_back();
	REQUIRE(str.size() == 1);
	str.pop_back();
	REQUIRE(str.size() == 0);	
}

TEST_CASE("resize", "[string]") {
	string str;
	str.resize(10);
	REQUIRE(str.size() == 10);	
	REQUIRE(str.capacity() == 10);
	str.resize(15);
	REQUIRE(str.size() == 15);	
	REQUIRE(str.capacity() == 15);

	str.resize(17, 'b');
	REQUIRE(str.size() == 17);	
	REQUIRE(str.capacity() == 17);
	REQUIRE(str[9] == '\0');
	REQUIRE(str[10] == '\0');
	REQUIRE(str.back() == 'b');
}

TEST_CASE("capacity", "[string]") {
	string str;
	REQUIRE(str.capacity() == 0);
	str.push_back('a');
	REQUIRE(str.capacity() == 1);
	str.push_back('b');
	REQUIRE(str.capacity() == 3); // _data capacity goes 2 -> 4 due to sentinel
	str.push_back('c');
	REQUIRE(str.capacity() == 3);
	str.pop_back();
	REQUIRE(str.capacity() == 3);
	str.pop_back();
	REQUIRE(str.capacity() == 3);
	str.pop_back();
	REQUIRE(str.capacity() == 3);
	str += "abcde";
	REQUIRE(str.capacity() == 7); // doubles since push_back call
	str += "f";
	REQUIRE(str.capacity() == 7);
	str += "gh";
	REQUIRE(str.size() == 8);
	REQUIRE(str.capacity() == 15);
	str += "sdfghyu";
	REQUIRE(str.size() == 15);
	REQUIRE(str.capacity() == 15);
	str += "k";
	REQUIRE(str.size() == 16);
	REQUIRE(str.capacity() == 31);
}

TEST_CASE("empty", "[string]") {
	string str;
	REQUIRE(str.empty() == true);
	str.push_back('a');
	REQUIRE(str.empty() == false);
	str.pop_back();
	REQUIRE(str.empty() == true);
}

TEST_CASE("operator[]", "[string]") {
	char arr[] = { 'a', 'b', 'c', '\0' };
	string str;
	str.resize(3);
	for (int i = 0; i < 3; ++i) {
		str[i] = arr[i];
	}
	string str2;
	str2.resize(3);
	for (size_t i = 0; i < str.size(); ++i) {
		str2[i] = str[i];
	}
	for (size_t i = 0; i < str.size(); ++i) {
		REQUIRE(str[i] == str2[i]);
	}
	REQUIRE(str[0] == 'a');
	REQUIRE(str[1] == 'b');
	REQUIRE(str[2] == 'c');
}

TEST_CASE("== and != operators", "[string]") {
	string strA = "String A";
	REQUIRE(strA == strA);
	REQUIRE(strA == "String A");

	string strB = "String B";
	REQUIRE(strA != strB);

	string strC = "String ABC";
	REQUIRE(strA != strC);
	REQUIRE(strA != "String ABC");
}

TEST_CASE("substr", "[string]") {
	string strA = "String A";
	REQUIRE(strA == strA.substr(0, strA.size()));
	REQUIRE(strA.substr(2, strA.size()) == "ring A");
	REQUIRE(strA.substr(0, strA.size() - 2) == "String");
	REQUIRE(strA.substr(1, 3) == "tri");
	REQUIRE(strA.substr(0, 0).empty());
	REQUIRE(strA.substr(0, 100) == strA);
	REQUIRE(strA.substr(0, BBG::string::npos) == strA);
	REQUIRE(strA.substr(2, BBG::string::npos) == "ring A");



	string strB = strA.substr(0, strA.size()); // full copy
	strB += "BC";
	REQUIRE(strA != strB);
}

TEST_CASE("find on string, chars, characters, and c-strings", "[string]") {
	const char* a = "strA";
	const char* b = "strB";
	const char* c = "strC";
	string strA = a, strB = b, strC = c;
	string str = "strA strB strC";

	REQUIRE(str.find(a, 0) == 0);
	REQUIRE(str.find(b, 0) == 5);
	REQUIRE(str.find(c, 0) == 10);

	REQUIRE(str.find(strA, 0) == 0);
	REQUIRE(str.find(strB, 0) == 5);
	REQUIRE(str.find(strC, 0) == 10);

	REQUIRE(str.find(a, 1) == npos);
	REQUIRE(str.find(strA, 1) == npos);

	REQUIRE(str.find("str", 1) == 5);

	const char* buff = "strCgarbagegarbagegarbage";
	REQUIRE(str.find(buff, 0) == npos);
	REQUIRE(str.find(buff, 0, 4) == 10);
	REQUIRE(str.find(buff, 0, 5) == npos);

	REQUIRE(str.find('B', 0) == 8);
	REQUIRE(str.find('B', 8) == 8);
	REQUIRE(str.find('B', 9) == npos);
	REQUIRE(str.find('D', 0) == npos);
}

TEST_CASE("getline", "[string]") {
   string line;
   const char* robots = 
		"# advertising-related bots:\n"
		"User-agent: Mediapartners-Google*\r\n"
		"Disallow: /\r"
		"\r\n\r\n"
		"# Wikipedia work bots:\r\n"
		"User-agent: IsraBot\n\r"
		"Disallow:\n"
		"\n\r\n\r"
		"User-agent: Orthogaffe\n"
		"Disallow:";
    
	robots = getLine(robots, line);
   REQUIRE(line == "# advertising-related bots:");

   robots = getLine(robots, line);
   REQUIRE(line == "User-agent: Mediapartners-Google*");

   robots = getLine(robots, line);
	REQUIRE(line == "Disallow: /");

	robots = getLine(robots, line);
	REQUIRE(line == "# Wikipedia work bots:");

	robots = getLine(robots, line);
	REQUIRE(line == "User-agent: IsraBot");

	robots = getLine(robots, line);
	REQUIRE(line == "Disallow:");

	robots = getLine(robots, line);
	REQUIRE(line == "User-agent: Orthogaffe");

	robots = getLine(robots, line);
	REQUIRE(line == "Disallow:");

	robots = getLine(robots, line);
	REQUIRE(line.empty());
}


TEST_CASE("swap", "[string]") {
	string s1("catsalot");
	string s2("dogs");

	string s3("catsalot");
	string s4("dogs");

	REQUIRE(s1 == s3);
	REQUIRE(s2 == s4);

	BBG::swap(s1, s2);

	REQUIRE(s1 != s2);
	REQUIRE(s1 == s4);
	REQUIRE(s2 == s3);
	REQUIRE(s1 != s3);
	REQUIRE(s2 != s4);

	BBG::swap(s1, s2);

	REQUIRE(s1 == s3);
	REQUIRE(s2 == s4);

	BBG::swap(s3, s3);
	REQUIRE(s3 == "catsalot");

}

TEST_CASE("Operator += for char", "[string]") {
	string actual("Test");
	actual += ' ';

	string expected("Test ");
	REQUIRE(actual == expected);
}

TEST_CASE("serialize/deserialize string", "[string]") {
	string expected("test");
	string actual;
	Buffer bytes;
	serialize(expected, bytes);
	deserialize(bytes, actual);
	REQUIRE(actual == expected);
}


TEST_CASE("string contains", "[string]") {
	string url("http://search/engine/bois/bois/go/");
	// Number of slashes
	size_t expected = 7;
	size_t actual = url.contains('/');
	REQUIRE(actual == expected);

	url = "help";
	// Number of slashes
	expected = 0;
	actual = url.contains('/');
	REQUIRE(actual == expected);
}

TEST_CASE("to lower", "[string]") {
	string word("AbcZzLkiI&EZQAa");
	word.lower();
	REQUIRE(word == "abczzlkii&ezqaa");
}

TEST_CASE("is alpha", "[string]") {
	string word("AbcZzLkiI&EZQAa");
	REQUIRE(!word.isAlphaNum());

	string word1("abc");
	REQUIRE(word1.isAlphaNum());

	string word2("a23bc");
	REQUIRE(word2.isAlphaNum());

	string word3("{");
	REQUIRE(!word3.isAlphaNum());

}
