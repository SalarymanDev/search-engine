#define CATCH_CONFIG_MAIN
#include "tests/catch.hpp"
#include "src/libs/index/Blob/Blob.h"
#include "src/libs/index/HashTable/HashTable.h"
#include "src/libs/logger/logger.h"

Logger logger;

TEST_CASE("Default constructor", "[Blob]") {
  // BytesRequired
  HashTable< uint32_t, size_t > ht;

  ht.Find( 1, 2 );
  ht.Find( 2, 3 );
  ht.Find( 3, 4 );

  ht.Optimize( );

  const uint32_t numDefaultBuckets = 50000;
  const uint32_t numValidBuckets = 3;

  size_t totalBytes = ( numValidBuckets + numDefaultBuckets ) * sizeof( SerialTuple ) 
                      + sizeof( Blob ) + sizeof( uint32_t ) * numDefaultBuckets; 
  
  size_t bytes = Blob::BytesRequired( &ht );
  
  REQUIRE(bytes == totalBytes);
  REQUIRE( bytes % 4 == 0 );

  // Write
  char * data = new char[ bytes ];
  Blob * blob = (Blob * ) data; 
  blob = Blob::Write( blob, bytes, &ht );

  // Find
  const SerialTuple * val = blob->Find( 1 );
  REQUIRE( val->getValue( ) == 2 );
  REQUIRE( val->Value != 2 );
  REQUIRE( val->isValid( ) );

  size_t value = val->getValue( );
  REQUIRE( ( ( value << 1 ) >> 1 ) == 2 );

  val = blob->Find( 2 );
  REQUIRE(val->getValue( ) == 3 );
  REQUIRE( val->Value != 3 );
  REQUIRE( val->isValid( ) );

  value = val->getValue( );
  REQUIRE( ( ( value << 1 ) >> 1 ) == 3 );

  val = blob->Find( 3 );
  REQUIRE( val->getValue( ) == 4 );
  REQUIRE( val->Value != 4 );
  REQUIRE( val->isValid( ) );

  value = val->getValue( );
  REQUIRE( ( ( value << 1 ) >> 1 ) == 4 );
  
// This might crash / segfault. If so just remove delete[] ( doesn't really matter )
  delete[] data;

	REQUIRE( true );
}


TEST_CASE("Large Blob", "[Blob]") {
  // BytesRequired
  HashTable< uint32_t, size_t > ht;

  const size_t loopNum = 100000;

  for( size_t i = 1; i < loopNum; ++i )
    ht.Find( i, i + 1 );

  for( size_t i = 1; i < loopNum; ++i )
    REQUIRE( ht.Find( i )->value == i + 1 );

  ht.Optimize( );
  size_t bytes = Blob::BytesRequired( &ht );

    // Write
  char * data = new char[ bytes ];
  Blob * blob = (Blob * ) data; 
  blob = Blob::Write( blob, bytes, &ht );

  for( size_t i = 1; i < loopNum; ++i ) 
    {
    SerialTuple * res = blob->Find( i );
    if ( !res ) 
      {
      Pair *result = ht.Find( i );
      if ( !result )
          logger.Fatal( "[Blob_test] index hashTable does not contain: " + to_string( i ) );
      else
          logger.Info( to_string( result->key ) + " " + to_string( result->value ) );
      logger.Fatal( "Unable to find " + to_string( i ) + " in hash blob" );
      }
    REQUIRE( res->getValue( ) == i + 1 );
    }

  delete [] data;
}


// TEST_CASE("Very Large Blob", "[Blob]") {
//   // BytesRequired
//   HashTable< uint32_t, size_t > ht;

//   const size_t loopNum = 15000000;

//   for( size_t i = 1; i < loopNum; ++i )
//     ht.Find( i, i + 1 );

//   for( size_t i = 1; i < loopNum; ++i )
//     REQUIRE( ht.Find( i )->value == i + 1 );

//   ht.Optimize( );
//   size_t bytes = Blob::BytesRequired( &ht );

//     // Write
//   char * data = new char[ bytes ];
//   Blob * blob = (Blob * ) data; 
//   blob = Blob::Write( blob, bytes, &ht );

//   for( size_t i = 1; i < loopNum; ++i ) {
//     SerialTuple * res = blob->Find( i );
//     if ( !res ) 
//       {
//       Pair *result = ht.Find( i );
//       if ( !result )
//           logger.Fatal( "[Blob_test] index hashTable does not contain: " + to_string( i ) );
//       else
//           logger.Info( to_string( result->key ) + " " + to_string( result->value ) );
//       logger.Fatal( "Unable to find " + to_string( i ) + " in hash blob" );
//       }
//     REQUIRE( res->getValue( ) == i + 1 );
//   }

//   delete [] data;
// }