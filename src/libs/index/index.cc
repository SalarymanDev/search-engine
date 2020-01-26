#include "src/libs/index/index.h"
#include "src/libs/index/PostingList/PostingList.h"

using namespace BBG;

    
PostingList * IndexChunk::getPostingList( const uint32_t wordID ) 
    {
    Blob* hashTable = (Blob * ) ( (char* ) this + sizeof( IndexChunk ));
    SerialTuple * tup = hashTable->Find( wordID );
    if (!tup)
     	return nullptr;
    const size_t bytesToPostingList = tup->getValue( );
    if ( bytesToPostingList == 0 ) 
        return nullptr;
    PostingList * plist = ( PostingList *) ( (char * ) this + bytesToPostingList );
    return plist;
    }

PostingList * IndexChunk::getEndDocPostingList( ) 
    {
    Blob* hashTable = (Blob * ) ( (char* ) this + sizeof( IndexChunk ) );  
    const size_t bytesToPostingList = hashTable->Find( 0 )->getValue( );
    if ( bytesToPostingList == 0 ) 
        return nullptr;
    PostingList * plist = ( PostingList *) ( (char *) this + bytesToPostingList );
    return plist;
    }

uint32_t IndexChunk::GetNumberOfDocuments( ) 
    {
    return DocumentsInChunk;
    }

uint32_t IndexChunk::GetNumberOfWords( ) 
    {
    return WordsInChunk;
    }

IndexFileReader::IndexFileReader( const char* filename ) 
    { 
    // Open the file for reading, map it, check the header,
    fd = open( filename, O_RDWR, S_IRWXU );
    if ( fd == -1 )
        logger.Fatal( "[IndexFileReader::ctor] Unable to open file: " + string(filename));
    fileSize = FileSize( fd );
    chunk = ( IndexChunk * ) mmap( nullptr,  fileSize, PROT_READ, MAP_SHARED, fd, 0 );
    if (chunk == MAP_FAILED)
        logger.Fatal("[IndexFileReader::ctor] Unable to memory map chunk: " + string(filename) + " fd: " + to_string(fd));
    }

IndexFileReader::~IndexFileReader( ) 
    {
    if ( fd != -1) {
        munmap(chunk, fileSize);
        int result = close( fd );
        if (result == -1)
            logger.Error("[IndexFileReader::dtor] Unable to close index chunk.");
    }
    }

IndexChunk * IndexFileReader::Load( ) const
    {
    return chunk;
    }
