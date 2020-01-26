#define CATCH_CONFIG_MAIN
#include "tests/catch.hpp"
#include "src/libs/index/HashTable/HashTable.h"

using BBG::HashTable;

TEST_CASE("Default constructor", "[HashTable]") {
	HashTable<uint32_t, uint32_t> ht;

	ht.Find(3,2);

	REQUIRE(ht.Find(3)->value == 2);

}

TEST_CASE("large addition and Optimize", "[HashTable]") {
	HashTable<uint32_t, uint32_t> ht;

	const size_t loopNum = 100000;

	for(size_t i = 0; i < loopNum; ++i)
		ht.Find(i, 2);

	for(size_t i = 0; i < loopNum; ++i)
		REQUIRE( ht.Find(i)->value == 2 );

	for(size_t i = 0; i < loopNum; ++i)
		ht.Find(i, i)->value = i;

	for(size_t i = 0; i < loopNum; ++i)
		REQUIRE( ht.Find(i)->value == i );

	ht.Optimize( );

	for(size_t i = 0; i < loopNum; ++i)
		REQUIRE( ht.Find(i)->value == i );

}
