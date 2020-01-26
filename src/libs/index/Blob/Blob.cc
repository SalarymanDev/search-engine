#include "src/libs/index/Blob/Blob.h"

using namespace BBG;
Logger loggmister;

// Write the HashBucket out as a SerialTuple in the buffer,
// returning a pointer to one past the last character written.

 char *SerialTuple::Write( char *buffer, char *bufferEnd,
        const HashBucket *b)
    {
    size_t bytes = sizeof(SerialTuple);
    while ( b )
        {
        struct SerialTuple *tmpTup = (struct SerialTuple *) buffer;
        if ( buffer + bytes >= bufferEnd )
            loggmister.Fatal( "[SerialTuple::Write] You walked of the buffer." );
        tmpTup->setValue( b->tuple.value );
        tmpTup->HashValue = b->hashValue;
        tmpTup->Key = b->tuple.key; 
        
        buffer += bytes;
        b = b->next;
        bytes = sizeof(SerialTuple); 
        }
    struct SerialTuple *tmpTup = (struct SerialTuple *) buffer;
    // added sentinal tuple
    tmpTup->Value = 0;
    tmpTup->HashValue = 0;
    tmpTup->Key = 0;

    return buffer + bytes;
    }

// Helper Function that reads an entire tuple chain
 size_t SerialTuple::BytesRequired( const  HashBucket * b )
    {
    size_t bytes = sizeof( SerialTuple );
    for ( ; b ; b = b->next )
        bytes += sizeof( SerialTuple );
    return bytes;
    }

// checks valid bit to if at the end of a bucket
bool SerialTuple::isValid( ) const
    {
    return ( Value & validationMask ) != 0;
    }
// returns a value with valid bit set
void SerialTuple::setValue( size_t value )
    {
    if ( value > validationMask ) 
        {
        loggmister.Fatal( "[SerialTuple::makeValid] Value is too large for Blob." );
        }
    Value = ( value & valueMask ) + validationMask;
    }
// remove the valid bit to retrieve the actual value
size_t SerialTuple::getValue( ) const
    {
    return ( Value & valueMask );
    }


SerialTuple* Blob::Find( uint32_t key ) const
    {
    // Search for the key k and return a pointer to the
    // ( key, value ) entry.  If the key is not found,
    // return nullptr.
    uint32_t hashValue = hash( key ) % NumberOfBuckets;
    SerialTuple * tup = ( struct SerialTuple * ) ( ( ( char* ) this ) + offsetToBuckets[ hashValue ] );
    // loggmister.Info("[Blob::Find] hashValue: " + to_string(hashValue));
    if ( tup->HashValue == hashValue )
        {
        while( tup->isValid( ) )
            {
            if ( tup->Key == key )
                return tup;
            ++tup;
            if ( !tup->isValid( ) )
                {
                loggmister.Debug( "[Blob::Find] Found a bucket but did not find the key" );
                return nullptr;
                }
            }
        }
    loggmister.Debug( "[Blob::Find] Did not find the bucket for hashvalue: " + to_string( hashValue ) );
    return nullptr;
    }

 size_t Blob::BytesRequired( const Hash *hashTable )
    {
    // Calculate how much space it will take to
    // represent a HashTable as a Blob.

    // Need space for the header + buckets +
    // all the serialized tuples.
    
    // header
    // BlobSize, NumberOfBuckets, Buckets *
    size_t bytes = sizeof( Blob );
    // need 1 uint32_t per bucket
    uint32_t numBuckets = hashTable->numberOfBuckets;
    bytes += numBuckets * sizeof( uint32_t );

    // tuples
    for ( size_t i = 0; i < numBuckets; ++i )
        bytes += SerialTuple::BytesRequired( hashTable->buckets[ i ] );
    return bytes;
    }

// Write a Blob into a buffer, returning a
// pointer to the blob.

 Blob *Blob::Write( Blob *hb, size_t bytes,
        const Hash *hashTable )
    {
    size_t numBuckets = hashTable->numberOfBuckets;
    hb->BlobSize = bytes;
    hb->NumberOfBuckets = numBuckets;
                                                        // Using uint32_t because offsetToBuckets hold uint32_t
    uint32_t bytesBeforeTups = sizeof( Blob ) + ( numBuckets ) * sizeof( uint32_t ); 
    hb->offsetToBuckets[ 0 ] = bytesBeforeTups;

    for ( size_t i = 1; i < numBuckets; ++i )
        hb->offsetToBuckets[ i ] = ( uint32_t ) SerialTuple::BytesRequired( hashTable->buckets[ i - 1 ] ) 
                                                + hb->offsetToBuckets[ i - 1 ];
    
    char * currentBucket = ( (char * ) hb ) + bytesBeforeTups;
    char * bufEnd = ( ( char * ) ( hb ) ) + bytes;
    for ( size_t i = 0; i < numBuckets; ++i )
        currentBucket = SerialTuple::Write( currentBucket, bufEnd, hashTable->buckets[ i ] );

    return hb;
    }

// Create allocates memory for a Blob of required size
// and then converts the HashTable into a Blob.
// Caller is responsible for discarding when done.

// (No easy way to override the new operator to create a
// variable sized object.)

 Blob *Blob::Create( const Hash *hashTable )
    {
    // Your code here.
    size_t bytes = BytesRequired( hashTable );
    char * data = new char [ bytes ];
    Blob * hb = ( Blob * ) data ;

    Blob::Write( hb, bytes, hashTable );
    return hb;
    }

// Discard
 void Discard( Blob *blob )
    {
    char * data = ( char * ) blob;
    delete[] data;
    } 

HashFile::HashFile( const char * filename )
    {
    // Open the file for reading, map it, check the header,
    // and note the blob address.
    int fd = open( filename, O_RDWR, S_IRWXU );
    if( fd == -1 )
       loggmister.Fatal( "[HashFile::HashFile] error file open");
    size_t fileSize = FileSize( fd );
    blob = ( Blob * ) mmap( nullptr,  fileSize, PROT_READ, MAP_SHARED, fd, 0 );
    close( fd );
    }

HashFile::HashFile( const char *filename, const Hash *hashtable )
    {
    // Open the file for write, map it, write
    // the hashtable out as a Blob, and note
    // the blob address.

    int fd = open(filename, O_RDWR | O_CREAT, S_IRWXU);
    if( fd == -1 )
       loggmister.Fatal( "[HashFile::HashFile] error file open" );
    
    size_t bytes = Blob::BytesRequired( hashtable );

    fallocate(fd, 0, 0, bytes * sizeof( char ) );

    blob = ( Blob * ) mmap( nullptr,  bytes * sizeof( char ), PROT_READ | PROT_WRITE, MAP_PRIVATE , fd, 0 );
    if ( blob == MAP_FAILED )
        loggmister.Fatal( "[HashFile::HashFile] mmap file open" );

    blob->Write( blob, bytes, hashtable );
    close( fd );
    }


const Blob * HashFile::getBlob( )
    {
    return blob;
    }
