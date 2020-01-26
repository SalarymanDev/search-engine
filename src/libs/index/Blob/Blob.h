#ifndef BLOB_H
#define BLOB_H

// Blob, a serialization of a HashTable into one contiguous
// block of memory, possibly memory-mapped to a HashFile.

// Nicole Hamilton  nham@umich.edu

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>
#include <cstdint>
#include <malloc.h>
#include <unistd.h>
#include <sys/mman.h>   

#include "src/libs/utils/utils.h"
#include "src/libs/index/HashTable/HashTable.h"
#include "src/libs/logger/logger.h"

using Hash = HashTable< uint32_t, size_t >;
using Pair = Tuple< uint32_t, size_t >;
using HashBucket = Bucket< uint32_t, size_t >;

namespace BBG {
    static const size_t Unknown = 0;

    template <class T>
    extern uint32_t hash( T value, size_t length );

    struct SerialTuple
    {
    // This is a serialization of a HashTable< char *, size_t >::Bucket.
    // One is packed up against the next in a Blob.

    // Since this struct includes size_t and uint32_t members, we'll
    // require that it be sizeof( size_t ) aligned to avoid unaligned
    // accesses.

    public:

        // SerialTuple.Length = 0 is a sentinel indicating
        // this is the last SerialTuple chained in this list.
        // (Actual length is not given but not needed.)

        // Bits          |     63-63       |   62-0   |
        // attributes := | validation bit  |  Value   |
        static const size_t validationMask = 0x8000000000000000;
        static const size_t valueMask      = 0x7FFFFFFFFFFFFFFF;
        size_t Value;
        uint32_t Key, HashValue;

        // Write the HashBucket out as a SerialTuple in the buffer,
        // returning a pointer to one past the last character written.

        static char *Write( char *buffer, char *bufferEnd, const HashBucket *b );

        // Helper Function that reads an entire tuple chain
        static size_t BytesRequired( const  HashBucket * b );

        // checks valid bit to if at the end of a bucket
        bool isValid( ) const;
        // returns a value with valid bit set
        void setValue( size_t value );
        // remove the valid bit to retrieve the actual value
        size_t getValue( ) const;
    };


    class Blob
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
        // a Blob really is one of your Blobs.

        uint32_t 
            BlobSize,
            NumberOfBuckets,
            offsetToBuckets[ Unknown ];

        // The SerialTuples will follow immediately after.

        
    static Blob *Create( const Hash *hashTable );
    static Blob *Write( Blob *hb, size_t bytes, const Hash *hashTable );
    static size_t BytesRequired( const Hash *hashTable );
    
    SerialTuple* Find( uint32_t key ) const;

    };

    class HashFile
    {
    private:
        Blob *blob;

    public:

        const Blob * getBlob( );

        HashFile( const char * filename );
        HashFile( const char *filename, const Hash *hashtable );
    };
}

#endif