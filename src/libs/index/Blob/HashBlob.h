#ifndef HASH_BLOB_H
#define HASH_BLOB_H

// HashBlob, a serialization of a HashTable into one contiguous
// block of memory, possibly memory-mapped to a HashFile.

// Nicole Hamilton  nham@umich.edu

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstdint>
#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>

#include "src/libs/index/HashTable/HashTable.h"
#include "src/libs/utils/utils.h"

using namespace BBG;
using Hash = HashTable< const char *, uint32_t >;
using Pair = Tuple< const char *, uint32_t >;
using HashBucket = Bucket< const char *, uint32_t >;

namespace BBG {

static const size_t Unknown = 0;

template <class T>
extern uint32_t hash( T value );


   // template <>
   // uint32_t inline fnvHash<const char*>( const char* data, size_t length );

   struct SerialTuple
      {
      // This is a serialization of a HashTable< char *, size_t >::Bucket.
      // One is packed up against the next in a HashBlob.

      // Since this struct includes size_t and uint32_t members, we'll
      // require that it be sizeof( size_t ) aligned to avoid unaligned
      // accesses.

      public:

         // SerialTuple.Length = 0 is a sentinel indicating
         // this is the last SerialTuple chained in this list.
         // (Actual length is not given but not needed.)

         size_t Length, Value;
         uint32_t HashValue;

         // The Key will be a C-string of whatever length.
         char Key[ Unknown ];

         // Calculate the bytes required to encode a HashBucket as a
         // SerialTuple.

         static size_t BytesRequired( const HashBucket *b )
            {
            // Your code here.
            // Length, Value and HashValue.
            size_t size = 3 * sizeof( size_t );
            // key, if b isn't a sentinal
            if ( b )
               size += strlen( b->tuple.key ) * sizeof( char );
            // null charater
            size += sizeof( char ); 
            // don't need next pointer since next immediately follows in memory
            // size += sizeof( HashBucket* );
            return RoundUp( size, sizeof( size_t ) );
            }

         // Write the HashBucket out as a SerialTuple in the buffer,
         // returning a pointer to one past the last character written.

         static char *Write( char *buffer, char *bufferEnd,
               const HashBucket *b)
            {
            size_t length = 0;
            HashBucket *tmp;
            size_t bytes = BytesRequired( b );
            while ( b )
               {
   //            cout << " hit live bucket \n\n";
               struct SerialTuple *tmpTup = (struct SerialTuple *) buffer;
               if ( buffer + bytes >= bufferEnd )
                  exit( 1 );
               tmpTup->Length = bytes;
               tmpTup->Value = b->tuple.value;
               tmpTup->HashValue = b->hashValue;
               strcpy( tmpTup->Key, b->tuple.key ); 
               
            //   memcpy(buffer, this, bytes );
               buffer += bytes;
               b = b->next;
               bytes = BytesRequired( b ); 
   
               }
            struct SerialTuple *tmpTup = (struct SerialTuple *) buffer;
            // added sentinal tuple
            tmpTup->Length = 0;
            tmpTup->Value = 0;
            tmpTup->HashValue = 0;
            char empty[1] {'\0'};
            strcpy( tmpTup->Key, empty );
            tmpTup->Key;
         //   memcpy(buffer, this, bytes );
            return buffer + bytes;
            }

            // Helper Function that reads an entire tuple chain
            static size_t ReadBytesRequired( const  HashBucket * b )
               {
               size_t bytes = 0;
               do
                  {
                  bytes += SerialTuple::BytesRequired( b );
                  if( !b )
                     break;
                  b = b->next;
                  } while( true );
               return bytes;
               }
   };


   class HashBlob
      {
      // This will be a hash specifically designed to hold an
      // entire hash table as a single contiguous blob of bytes.
      // Pointers are disallowed so that the blob can be
      // relocated to anywhere in memory

      // The basic structure should consist of some header
      // information including the number of buckets and other
      // details followed by a concatenated list of all the
      // individual lists of tuples within each bucket.

      public:

         // Define a MagicNumber and Version so you can validate
         // a HashBlob really is one of your HashBlobs.

         uint32_t BlobSize,
            NumberOfBuckets,
            Buckets[ Unknown ];

         // The SerialTuples will follow immediately after.


         const SerialTuple *Find( const char *key ) const
            {
            // Search for the key k and return a pointer to the
            // ( key, value ) entry.  If the key is not found,
            // return nullptr.
            uint32_t hash = fnvHash( key, strlen( key ) ) % NumberOfBuckets;

            SerialTuple * tup = (struct SerialTuple *) ( ( ( char* ) this )  + Buckets[ hash - 1 ] );

            if ( tup->HashValue == hash )
               {
               while( !CompareEqual( tup->Key, key ) )
                  {
                  tup = (SerialTuple * ) ( (char* )(tup) + tup->Length );
                  if ( tup->Length == 0 )
                     return nullptr;
                  }
               return tup;
               }
            return nullptr;
            }


         static size_t BytesRequired( const Hash *hashTable )
            {
            // Calculate how much space it will take to
            // represent a HashTable as a HashBlob.

            // Need space for the header + buckets +
            // all the serialized tuples.
            
            // header
            // BlobSize, NumberOfBuckets,
            size_t bytes = 2 * sizeof( uint32_t );
            // need 1 size_t per bucket
            size_t numBuckets = hashTable->numberOfBuckets;
            bytes += numBuckets * sizeof( uint32_t );

            // tuples
            for ( size_t i = 0; i < numBuckets; ++i )
               {
               bytes += SerialTuple::ReadBytesRequired( hashTable->buckets[ i ] );
               }
            return bytes;
            }

         // Write a HashBlob into a buffer, returning a
         // pointer to the blob.

         static HashBlob *Write( HashBlob *hb, size_t bytes,
               const Hash *hashTable )
            {
            size_t numBuckets = hashTable->numberOfBuckets;
            hb->BlobSize = bytes;
            hb->NumberOfBuckets = numBuckets;
            
            size_t bytesBeforeTups = ( 4 + numBuckets ) * sizeof( size_t );
            hb->Buckets[ 0 ] = SerialTuple::ReadBytesRequired( hashTable->buckets[ 0 ] ) + bytesBeforeTups;

            for ( size_t i = 1; i < numBuckets; ++i )
               hb->Buckets[ i ] = SerialTuple::ReadBytesRequired( hashTable->buckets[ i ] ) + hb->Buckets[ i - 1 ];
            
            char * currentBucket = ( (char * ) hb ) + bytesBeforeTups;
            char * bufEnd = ( (char *) ( hb ) ) + bytes;
            for ( size_t i = 0; i < numBuckets; ++i )
               currentBucket = SerialTuple::Write( currentBucket, bufEnd, hashTable->buckets[ i ] );

            return hb;
            }

         // Create allocates memory for a HashBlob of required size
         // and then converts the HashTable into a HashBlob.
         // Caller is responsible for discarding when done.

         // (No easy way to override the new operator to create a
         // variable sized object.)

         static HashBlob *Create( const Hash *hashTable )
            {
            // Your code here.
            size_t bytes = BytesRequired( hashTable );
            char * data = new char [ bytes ];
            HashBlob * hb = ( HashBlob * ) data ;

            HashBlob::Write( hb, bytes, hashTable );
            return hb;
            }

         // Discard

         static void Discard( HashBlob *blob )
            {
            char * data = ( char * ) blob;
            delete[] data;
            }
      };

   class HashFile
      {
      private:

         HashBlob *blob;

         size_t FileSize( int f )
            {
            struct stat fileInfo;
            fstat( f, &fileInfo );
            return fileInfo.st_size;
            }

      public:

         const HashBlob *Blob( )
            {
            return blob;
            }

         HashFile( const char *filename )
            {
            // Open the file for reading, map it, check the header,
            // and note the blob address.
            int fd = open(filename, O_RDWR, S_IRWXU);
   //         if( fd == -1 )
   //            cout << "error file open \n\n\n";
            size_t fileSize = FileSize( fd );
            blob = ( HashBlob * ) mmap( nullptr,  fileSize, PROT_READ, MAP_SHARED, fd, 0 );
            close( fd );
            }

         HashFile( const char *filename, const Hash *hashtable )
            {
            // Open the file for write, map it, write
            // the hashtable out as a HashBlob, and note
            // the blob address.

            int fd = open(filename, O_RDWR | O_CREAT, S_IRWXU);
   //         if( fd == -1 )
   //            cout << "error file open \n\n\n";
            
            size_t bytes = HashBlob::BytesRequired( hashtable );
            fallocate(fd, 0, 0, bytes);

            blob = ( HashBlob * ) mmap( nullptr,  bytes, PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0 );
   //         if ( blob == MAP_FAILED )
   //            cout << " error \n\n";

            blob->Write( blob, bytes, hashtable );
            close( fd );
            }

         ~HashFile( )
            {
            // blob->Discard( blob );
            // Your code here.
            }
      };
}

#endif