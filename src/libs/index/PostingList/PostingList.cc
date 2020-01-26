#include "src/libs/index/PostingList/PostingList.h"
#include <iostream>

using namespace BBG;

// this is invalidated by variable lenth byte encoding
size_t PostingList::postBytesRequired(size_t numPosts, size_t numDocs ) 
    {
    return sizeof( PostingList ) 
        + sizeof( SeekTableEntry ) * SeekTableEntry::TableSize 
        + sizeof( Post ) * numPosts 
        + sizeof( WordDocFeatures ) * numDocs;
    }

size_t PostingList::endDocBytesRequired( vector< PreprocessedDocument >& docs ) 
    {
    size_t bytes = sizeof( PostingList ) 
                + sizeof( SeekTableEntry ) * SeekTableEntry::TableSize 
                + sizeof( EndDocPost ) * docs.size( );
    for ( size_t i = 0; i < docs.size( ); ++i ) 
        {
        // add 1 more for null terminator
        bytes += docs[ i ].url.size( ) + 1;
        for( size_t t = 0; t < docs[ i ].titleText.size( ); ++t )
            bytes += docs[ i ].titleText[ t ].size( ) + 1;
        // for null terminator
        bytes += 1;
        }
    return bytes;
    }


 uint32_t PostingList::addPosts( PostingList * pList, uint32_t postOffset, vector<Post> & posts ) 
    {
    // Write posts
    Post* post = ( Post * ) ( ( char * ) pList + postOffset ); 
    for ( size_t i = 0; i < posts.size( ); ++i ) 
        { 
        *post = posts[ i ];
        ++post;
        }
    return postOffset + sizeof( Post ) * posts.size( );
    }

void PostingList::addFeature( PostingList *pList, uint32_t docId, WordDocFeatures& feature ) {
    WordDocFeatures *pf = ( ( WordDocFeatures* ) ( ( char* ) pList + pList->bytesToPostsEnd ) + docId );
    *pf = feature;
}

// returns one byte past end of doc. Don't need but can assert or use for debugging
uint32_t PostingList::addEndPostsAndUrls( PostingList *pList, uint32_t postOffset, vector<EndDocPost>& endPosts,
        vector< PreprocessedDocument > & docs ) 
    {
    
    uint32_t urlOffest = pList->bytesToPostsEnd;
    EndDocPost * endPost = ( EndDocPost* ) ( ( char* ) pList + postOffset );
    for( size_t i = 0; i < docs.size( ); ++i ) 
        {
        // store end post in postLis
        *endPost = endPosts[ i ];
        // save offest to corresponding url in endPost
        endPost->bytesToUrl = urlOffest - postOffset;
        // write url at urlOffset
        strcpy( ( ( char* ) pList) + urlOffest, docs[ i ].url.c_str( ) );
        // move past url
        urlOffest += ( uint32_t ) docs[ i ].url.size( ) + 1;
        // save offset to corresponding title after endPosts
        // endPost->bytesToTitle = urlOffest - postOffset;
        // write title
        string title = to_string( docs[ i ].titleText );
        strcpy( ( ( char* ) pList ) + urlOffest, title.c_str( ) );
        urlOffest += title.size( ) + 1;
        postOffset += sizeof( EndDocPost );
        ++endPost;
        }
    return urlOffest;
    }    

    // Called write after inserting heading info
uint32_t PostingList::WriteSeekTable(  PostingList * pList, vector<EndDocPost>& locations ) 
    {
    // Note: header size is 4 uint32_t
    const uint32_t headerSize = sizeof( PostingList );
    // Note: seekTable entry and post are 2 uint32_t 
    const uint32_t entrySize = sizeof( SeekTableEntry );
    // pList->numberOfPosts = ( uint32_t ) locations.size( );
    
    // ensure we don't write off end of table
    uint32_t tableIndex = 0;

    SeekTableEntry * entry = ( SeekTableEntry * ) ( ( char * ) pList + headerSize );
    uint32_t postOffset = entrySize * SeekTableEntry::TableSize + headerSize;
    
    for ( size_t p = 0; p < locations.size( ) && tableIndex < SeekTableEntry::TableSize; ++p ) 
        {
        // Write seekTable
        uint32_t absLoc = locations[ p ].getAbsoluteLocation( );
        uint32_t shiftLoc = absLoc >> SeekTableEntry::numLowBits;
        
        while( tableIndex <= shiftLoc ) 
            {
            entry->PostingListPosition = postOffset;
            entry->AbsolutePosition = absLoc;
            ++tableIndex;
            ++entry;  
            }
        postOffset += sizeof( EndDocPost ); 
        }
    // post will need this to write features
    pList->bytesToPostsEnd = postOffset;
    // return byteOffset to post start
    return entrySize * SeekTableEntry::TableSize + headerSize;
    }

    // Called write after inserting heading info
uint32_t PostingList::WriteSeekTable(  PostingList * pList, vector< Post >& locations ) 
    {
    
    const uint32_t headerSize = sizeof( PostingList );
    
    const uint32_t entrySize = sizeof( SeekTableEntry );
    // pList->numberOfPosts = ( uint32_t ) locations.size( );
    
    // ensure we don't write off end of table
    uint32_t tableIndex = 0;

    SeekTableEntry * entry = ( SeekTableEntry * ) ( ( char * ) pList + headerSize );
    uint32_t postOffset = entrySize * SeekTableEntry::TableSize + headerSize;
    
    for ( size_t p = 0; p < locations.size( ) && tableIndex < SeekTableEntry::TableSize; ++p ) 
        {
        // Write seekTable
        uint32_t absLoc = locations[ p ].getAbsoluteLocation( );
        uint32_t shiftLoc = absLoc >> SeekTableEntry::numLowBits;
        
        while( tableIndex <= shiftLoc ) 
            {
            entry->PostingListPosition = postOffset;
            entry->AbsolutePosition = absLoc;
            ++tableIndex;
            ++entry;  
            }
        postOffset += sizeof( Post );// addPost(pList, postOffset, locations[ p ] );
        }
    // post will need this to write features
    pList->bytesToPostsEnd = postOffset;
    // return byteOffset to post start
    return entrySize * SeekTableEntry::TableSize + headerSize;
    }

char * PostingList::getSentinel( ) 
    {
   return ( ( char * ) this + bytesToPostsEnd );
    }

uint32_t PostingList::getPostLoc( size_t target ) 
    {

    const uint32_t headerSize = sizeof( PostingList );
    const uint32_t entrySize = sizeof( SeekTableEntry );

    uint32_t seekIndex = target >> SeekTableEntry::numLowBits;
    SeekTableEntry* entry = ( SeekTableEntry* ) ( ( char * ) this + headerSize + entrySize * seekIndex );
    return entry->PostingListPosition;
    }

EndDocPost * PostingList::SeekEndDoc( size_t target ) 
    {
   
    uint32_t postPos = getPostLoc( target );

    if ( !postPos )
        return nullptr;

    // Update: changed sentinel to be just past last Post
    EndDocPost * p = ( EndDocPost * ) ( ( char * ) this + postPos );
    EndDocPost * sentinel = (EndDocPost *) getSentinel( );

    while ( p < sentinel && p->getAbsoluteLocation( ) < target )
        ++p; 
    return ( p >= sentinel ) ? nullptr : p;
}

Post * PostingList::Seek( size_t target ) 
    {
   
    uint32_t postPos = getPostLoc( target );

    if ( !postPos )
        return nullptr;

    // Update: changed sentinel to be just past last Post
    Post *  p = ( Post * ) ( ( char * ) this + postPos );
    Post * sentinel = ( Post* ) getSentinel( );

    while ( p < sentinel && p->getAbsoluteLocation( ) < target )
        ++p; 
    return ( p >= sentinel ) ? nullptr : p;
    }

Post* PostingList::getLastPost( ) 
    {
    return ( Post * ) ( ( char* ) this + bytesToPostsEnd - sizeof( Post) );
    }

EndDocPost* PostingList::getLastEndDocPost( ) 
    {
    return ( EndDocPost * ) ( ( char* ) this + bytesToPostsEnd - sizeof( EndDocPost ) );     
    }


WordDocFeatures * PostingList::getWordDocFeature( const EndDocPost* endDoc ) 
    {
    return  ( WordDocFeatures* ) ( ( char* ) this + bytesToPostsEnd ) + endDoc->docId;
    }
