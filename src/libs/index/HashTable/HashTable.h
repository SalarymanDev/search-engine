#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstdint>

#include "src/libs/utils/utils.h"

using namespace BBG;

// You may add additional members or helper functions.
#define MIN_BUCKETS 50000

namespace BBG {

uint32_t fnvHash( const char* data, size_t length );

template <class T>
uint32_t hash( T value );

template <>
uint32_t inline hash<const char*>( const char* value )
   {
   return fnvHash(value, strlen( value ) );
   }

template <class T>
uint32_t hash( T value ) 
   {
      return fnvHash( (char* ) &value, sizeof( T ) ); 
   }

// Compare C-strings, return true if they are the same.

bool CompareEqual( const char *L, const char *R );

bool CompareEqual( uint32_t L, uint32_t R );

template< typename Key, typename Value > class Tuple
   {
   public:
      Key key;
      Value value;

      Tuple( const Key &k, const Value v ) : key( k ), value( v )
         { 
         }
   };


template< typename Key, typename Value > class Bucket
   {
   public:
      Tuple< Key, Value > tuple;
      Bucket *next;
      uint32_t hashValue;
      Bucket( const Key &k, uint32_t h, const Value v ) :
         tuple( k, v ), next( nullptr ) , hashValue( h )
         {
         }
   };

template< typename Key, typename Value > class HashTable
   {
   public:
   //private:

      friend class Iterator;
      friend class Blob;
      
      size_t numberOfBuckets;
      size_t size;
      Bucket< Key, Value > **buckets;



      void insertIntoRebuild( Bucket< Key, Value >** newBuckets, 
                              size_t newBucketNumber, 
                              Bucket< Key, Value > * oldBucket );

      void rehashTable( double loading = 1.3 );

   

      HashTable( );

      ~HashTable( );

      Tuple< Key, Value > *Find( const Key k, const Value initialValue );

      Tuple< Key, Value > *Find( const Key k ) const;
      
      void Optimize( double loading = 1.5 );

      class Iterator
         {
         private:

            friend class HashTable;

            HashTable *table;
            size_t index; 
            Bucket<Key, Value> *bucket;

            Iterator( HashTable *_table, size_t _index, Bucket<Key, Value> *_bucket );

         public:
            Iterator( );

            Iterator( const Iterator& rhs );

            ~Iterator( );


            Tuple< Key, Value > & operator*( );

            Tuple< Key, Value > * operator->( ) const;

            // Prefix ++
            Iterator & operator++( );

            // Postfix ++
            Iterator operator++( int );

            bool operator==( const Iterator &rhs ) const;

            bool operator!=( const Iterator &rhs ) const;

         };

      Iterator begin( );

      Iterator end( );

   };


// HashTable public member functions

template< typename Key, typename Value > 
HashTable<Key, Value>::HashTable() 
   : numberOfBuckets(MIN_BUCKETS), size(0)
   {
   buckets = new Bucket< Key, Value >*[ numberOfBuckets + 1 ] { nullptr };
   }


template< typename Key, typename Value > 
HashTable<Key, Value>::~HashTable( )
   {
   for( size_t i = 0; i < numberOfBuckets; ++i ) 
      {
      Bucket< Key, Value > * bucket = buckets[ i ];
      Bucket< Key, Value > * tmp;
      while( bucket ) 
         {
         tmp = bucket;
         bucket = bucket->next;
         delete tmp;
         }
      }
   delete[] buckets;
   }


template< typename Key, typename Value > 
Tuple< Key, Value > *HashTable<Key, Value>::Find( const Key k, const Value initialValue )
   {
   // Search for the key k and return a pointer to the
   // ( key, value ) entry.  If the key is not already
   // in the hash, add it with the initial value.
   if( 1.5 * size > numberOfBuckets )
      rehashTable( );

   uint32_t bucket_loc = hash( k ) % numberOfBuckets;
   Bucket< Key, Value > * bucket = buckets[ bucket_loc ];
            
   if( !bucket )
      {
      bucket = new Bucket<Key, Value>( k, bucket_loc, initialValue );
      buckets[ bucket_loc ] = bucket;
      ++size;
      return &bucket->tuple;
      }

   Bucket< Key, Value > *previous = bucket;
   while ( bucket ) 
      {
      if( CompareEqual( bucket->tuple.key, k ) )
         return &bucket->tuple;
      previous = bucket;
      bucket = bucket->next;
      }

   previous->next = new Bucket<Key, Value>( k, bucket_loc, initialValue );
   ++size;

   return &previous->next->tuple;
   }


template< typename Key, typename Value > 
Tuple< Key, Value > *HashTable<Key, Value>::Find( const Key k ) const
   {
   // Search for the key k and return a pointer to the
   // ( key, value ) enty.  If the key is not already
   // in the hash, return nullptr.
   
   uint32_t bucket_loc = hash( k ) % numberOfBuckets;

   Bucket< Key, Value > * bucket = buckets[ bucket_loc ];
   while ( bucket ) 
      {
      if ( CompareEqual( bucket->tuple.key, k ) )
         return &bucket->tuple;
      bucket = bucket->next;
      }
   return nullptr;
   }


template< typename Key, typename Value > 
void HashTable< Key, Value >::Optimize( double loading )
   {
   // Modify or rebuild the hash table as you see fit
   // to improve its performance now that you know
   // nothing more is to be added.
   if( loading * size > numberOfBuckets )
      rehashTable( loading );
   }


// HashTable private member functions

template< typename Key, typename Value > 
void HashTable< Key, Value >::insertIntoRebuild( Bucket< Key, Value >** newBuckets, 
                                                 size_t newBucketNumber, 
                                                 Bucket< Key, Value > * oldBucket )
   {
   Key & k = oldBucket->tuple.key;

   uint32_t newBucketIndex = hash( k ) % newBucketNumber;
   
   if ( oldBucket->next )
      insertIntoRebuild( newBuckets, newBucketNumber, oldBucket->next );
   oldBucket->next = nullptr; 
   // TODO: delete hashValue?
   oldBucket->hashValue = newBucketIndex;

   Bucket< Key, Value > * newBucket = newBuckets[ newBucketIndex ];
   
   if ( !newBucket ) { 
      newBuckets[ newBucketIndex ] = oldBucket;
      return;
   }
      
   while ( newBucket->next ) 
      newBucket = newBucket->next;
      
   newBucket->next = oldBucket;
   return;
   }


template< typename Key, typename Value > 
void HashTable< Key, Value >::rehashTable( double loading )
   {
   size_t nextNumberOfBuckets = static_cast<size_t> ( double ( numberOfBuckets ) * loading ) + 1;
   // +1 to handle end iterator
   Bucket< Key, Value >** temp = new Bucket< Key, Value >* [ nextNumberOfBuckets + 1 ] { nullptr };

   for ( size_t i = 0; i < numberOfBuckets; ++i ) 
      {
      if ( buckets[ i ] )
         insertIntoRebuild( temp, nextNumberOfBuckets, buckets[ i ] );
      }
   delete[] buckets;
   buckets = temp;
   numberOfBuckets = nextNumberOfBuckets;
   }


// HashTable::Iterator public functions

template< typename Key, typename Value >
HashTable< Key, Value >::Iterator::Iterator( ) 
   : Iterator( nullptr, 0, nullptr ) 
   { 
   }


template< typename Key, typename Value >
HashTable< Key, Value >::Iterator::Iterator( const Iterator& rhs ) 
   {
   table = rhs.table;
   index = rhs.index;
   bucket = rhs.bucket;
   }  


template< typename Key, typename Value >
HashTable< Key, Value >::Iterator::~Iterator( )
   {
   }


template< typename Key, typename Value >
Tuple< Key, Value > & HashTable< Key, Value >::Iterator::operator*( )
   {
   return bucket->tuple;
   }


template< typename Key, typename Value >
Tuple< Key, Value > * HashTable< Key, Value >::Iterator::operator->( ) const
   {
   return &bucket->tuple;
   }


// Prefix ++
template< typename Key, typename Value >
typename HashTable< Key, Value >::Iterator & HashTable< Key, Value >::Iterator::operator++( )
   {
   if( bucket->next )
      bucket = bucket->next;
   else { 
      do {
         if ( index >= table->numberOfBuckets ) {
               bucket = nullptr;
               return *this;
         }
         ++index;
      } while ( !table->buckets[ index ] );
      bucket = table->buckets[ index ];
      } // else
   return *this;
   }


// Postfix ++
template< typename Key, typename Value >
typename HashTable< Key, Value >::Iterator HashTable< Key, Value >::Iterator::operator++( int )
   {
   Iterator itor( *table, index, bucket );
   ++this;
   return itor;
   }


template< typename Key, typename Value >
bool HashTable< Key, Value >::Iterator::operator==( const HashTable< Key, Value >::Iterator &rhs ) const
   {
   if ( this == &rhs )
      return true;
   if ( ( !bucket && rhs.bucket ) || ( bucket && !rhs.bucket ) ) 
      {
      return false; 
      }
   if ( index == rhs.index && table == rhs.table && 
      ( ( !bucket && !rhs.bucket ) || ( CompareEqual( bucket->tuple.key, rhs.bucket->tuple.key ) && 
                        bucket->tuple.value == rhs.bucket->tuple.value ) )
      ) { 
      return true;
      } //if
   return false;
   }


template< typename Key, typename Value >
bool HashTable< Key, Value >::Iterator::operator!=( const Iterator &rhs ) const 
   {
   return !( *this == rhs );
   }


template< typename Key, typename Value >
typename HashTable< Key, Value >::Iterator HashTable< Key, Value >::begin( )
   {
   Iterator itor( this, 0, buckets[ 0 ] );

   if ( buckets[ 0 ] ) 
      return itor;
   
   while( !itor.bucket )
      {
      ++itor.index;
      itor.bucket = buckets[ itor.index ];
      }
   return itor;
   }

template< typename Key, typename Value >
typename HashTable< Key, Value >::Iterator HashTable< Key, Value >::end( ) 
   { 
   return Iterator { this, numberOfBuckets, nullptr };
   }


// HashTable::Iterator private functions

template< typename Key, typename Value >
HashTable< Key, Value >::Iterator::Iterator(  HashTable *_table, size_t _index, 
                                              Bucket<Key, Value> *_bucket )
   : table( _table ), index( _index ), bucket( _bucket ) 
   {
   }

} // BBG

#endif
