#include "src/libs/index/Indexer/Indexer.h"
#include <iostream>

using namespace BBG;


Indexer::Indexer( Map< string, uint32_t >& _bow, Map< uint32_t, Map< string, uint32_t > >& _tf, Map< uint32_t, float >& _idf, 
        Map< string, float >& _NORM )
    :   numUniqueWords( 0 ), numTotalWords( 0 ), 
        numDocuments( 0 ), postingsHash( ), currentPostLocation( 1 ), 
        BagOfWords( _bow ), tf( _tf ), idf( _idf ), NORM( _NORM ) 
    { 
    }

size_t Indexer::collectFeatureFromVector( vector< string > & text, size_t docNumber, HtmlTag HtmlTag ) 
    {
    size_t bytes = 0;
    // features data from all words
    for ( size_t i = 0; i < text.size( ); ++i )
        {
        const uint32_t * wordID = BagOfWords.get( text[ i ] );
        if ( !wordID ) 
            {
            // logger.Error( "[Indexer::collectFeatureFromVector] Indexer given word not in BOW: " + text[ i ] );
            continue;
            }
        ++numTotalWords;
        posts[ *wordID ].push_back( Post( currentPostLocation++, HtmlTag ) );
        docIds[ *wordID ][ docNumber ] = true;
        ++wordCountMap[ *wordID ];
        // reserve space for single post
        bytes += sizeof( Post );
        if ( wordCountMap[ *wordID ] == 1 ) 
            { 
            // add each unique to hashTable. Current offset to posting unknown so leave blank.
            postingsHash.Find( *wordID, 0 );
            wordIDs.push_back( *wordID );
            ++numUniqueWords;
            // reserve size for PostingList
            bytes += sizeof( PostingList ) + sizeof( SeekTableEntry ) * SeekTableEntry::TableSize 
                + sizeof( WordDocFeatures ) * numDocuments;
            }
        }   
    return bytes;
    }

size_t Indexer::collectFeatures( vector< PreprocessedDocument > & docs )
    {
    size_t bytes = 0;
    numDocuments = docs.size( );
    vector<string> urlWords;
    for ( size_t i = 0; i < docs.size( ); ++i )
        {
        // anchor text
        bytes += collectFeatureFromVector( docs[ i ].anchorText, i, HtmlTag::Anchor );
        // url text
        urlWords.clear( );
        extractUrlWords( docs[ i ].url, urlWords );
        bytes += collectFeatureFromVector( urlWords, i, HtmlTag::Url );
        // title text
        bytes += collectFeatureFromVector( docs[ i ].titleText, i, HtmlTag::Title );
        // body Text
        bytes += collectFeatureFromVector( docs[ i ].bodyText, i, HtmlTag::Body );
        // docEND and url
        bytes += docs[ i ].url.size( ) + 1; 

        // reserve space for title
        for (size_t t = 0; t < docs[ i ].titleText.size( ); ++t)
            bytes += docs[ i ].titleText[ t ].size( ) + 1;
        ++bytes;
        
        size_t numWords = docs[ i ].anchorText.size( ) + docs[ i ].titleText.size( ) 
                        + docs[ i ].bodyText.size( );
        DocFeatures docFeats( 
            numWords, docs[ i ].titleText.size( ),
            docs[ i ].imgCount, docs[ i ].incomingLinkCount,
            docs[ i ].outgoingLinkCount, NORM[ docs[ i ].url ] );
        endDocPosts.push_back( EndDocPost( currentPostLocation++, i, docFeats ) );
        bytes += sizeof( EndDocPost );
        }
    // reserve space for endDoc PostingList
    bytes += sizeof( PostingList ) + sizeof( SeekTableEntry ) * SeekTableEntry::TableSize;
    // reserve space for header and hashBlob
    bytes += sizeof( IndexChunk );
    // reserve spot for endDoc offset at the end of hashBlob
    postingsHash.Find( 0, 0 );
    bytes += Blob::BytesRequired( &postingsHash );
    return bytes;
    }

void Indexer::writeIndexChunk( IndexChunk* index, vector< PreprocessedDocument > & docs ) 
    {
    //uint32_t WordsInIndex, DocumentsInIndex, LocationsInIndex, MaximumLocation;
    uint32_t indexHeaderSize = sizeof( IndexChunk );
    index->WordsInChunk = numTotalWords;
    index->DocumentsInChunk = numDocuments;
    index->LocationsInChunk = numTotalWords + numDocuments;
    index->MaximumLocation = currentPostLocation;
    
    Blob * indexBlob = ( Blob * ) ( ( char* ) index + indexHeaderSize );
    postingsHash.Optimize( );
    size_t blobBytes = Blob::BytesRequired( &postingsHash );
    indexBlob->Write( indexBlob, blobBytes, &postingsHash );

    size_t currentFilePosition = indexHeaderSize + indexBlob->BlobSize;
    
    // flag: This is just incase the index overflows. Remove once we know it works
    size_t prevPosition = currentFilePosition;
    
    for ( size_t i = 0; i < numUniqueWords; ++i ) 
        {
        // flag
        if( currentFilePosition < prevPosition )
            logger.Fatal( "[Indexer::writeIndexChunk] byte overflow" );

        // Fill EndDocPostings
        uint32_t wordID = wordIDs[ i ];
        SerialTuple *wordResult = indexBlob->Find( wordID );
        if ( !wordResult ) {
            Pair *result = postingsHash.Find( wordID );
            if ( !result )
                logger.Fatal( "[Indexer::writeIndexChunk] index hashTable does not contain: " + to_string( wordID ) );
            else
                logger.Info( to_string( result->key ) + " " + to_string( result->value ) );
            logger.Fatal( "Unable to find " + to_string( wordID ) + " in hash blob" );
        }
        wordResult->setValue( currentFilePosition );
        PostingList * pList =  ( PostingList* ) ( ( char* ) index + currentFilePosition );
    
        // fill out header pList
        pList->wordFeatures = WordFeatures( idf[ wordID ] );
        // pList->word = wordID; 
        // fill seek table, then posts
        uint32_t postingListOffset = PostingList::WriteSeekTable( pList, posts[ wordID ] );
        currentFilePosition += PostingList::addPosts( pList, postingListOffset, posts[ wordID ] );

        // error checking
        prevPosition = currentFilePosition;
    
        // document data unique to word and is placed after all posts.
        for( auto docID = docIds[ wordID ].begin( ); docID; docID = docIds[ wordID ].next( ) ) 
            {
            uint32_t logAnchorCount = (uint32_t) log10( docs[ docID->key ].anchorTermCounts[ wordID ] + 1);
            WordDocFeatures wordDocFeats( logAnchorCount, tf[wordID][ docs[ docID->key ].url ] + logAnchorCount );
            PostingList::addFeature( pList, docID->key, wordDocFeats );
            }
        // incement  currentFilePosition passed feature section
        currentFilePosition += numDocuments * sizeof( WordDocFeatures );
        }
    if ( currentFilePosition < prevPosition )
        logger.Fatal( "[Indexer::writeIndexChunk] byte overflow" );

    // Fill EndDocPostings
    SerialTuple *endDocBucket = indexBlob->Find( 0 );
    if ( !endDocBucket ) {
        Pair *result = postingsHash.Find( 0 );
        if ( !result )
            logger.Fatal( "[Indexer::writeIndexChunk] index hashTable does not contain: " + to_string( 0 ) );
        else
            logger.Info( to_string( result->key ) + " " + to_string( result->value ) );
        logger.Fatal( "Unable to find " + to_string( 0 ) + " in hash blob" );
    }
    endDocBucket->setValue( currentFilePosition );
    PostingList * pList =  ( PostingList* ) ( ( char* ) index + currentFilePosition );
    
    // fill out header pList
    // pList->word = 0;
    uint32_t postingListOffset = PostingList::WriteSeekTable( pList, endDocPosts );
    currentFilePosition += PostingList::addEndPostsAndUrls( pList, postingListOffset, endDocPosts, docs ); 
    }


// make index the write object to filename
void Indexer::createIndexChunk( const char* indexFile, vector< PreprocessedDocument > & docs ) {

    size_t bytes = collectFeatures( docs );
    logger.Info( "[Indexer::createIndexChunk] The total bytes requires will be: " + to_string( bytes ) );

    int fd = open( indexFile, O_RDWR | O_CREAT );
    if( fd == -1 )
        { 
        logger.Fatal( "[Indexer::createIndexChunk] couldn't open or create indexFile" );
        }

    fallocate( fd, 0, 0, bytes );

    IndexChunk * index = (IndexChunk *) mmap( nullptr,  bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );
    
    if ( index == MAP_FAILED ) 
        {
        close( fd );
        logger.Fatal( "[Indexer::createIndexChunk] gg mmap failed to open index file" );
        }

    writeIndexChunk( index, docs );
    close( fd );
    }


 uint32_t Indexer::wordToID( const string& word ) 
    {
    const uint32_t * id = BagOfWords.get( word );
    if( id )
        return *id;
    // logger.Error( "[Indexer::wordToID] Bag of Words does not include: " + word );
    return 0;
    }

void Indexer::extractUrlWords( string& url, vector< string >& vec )
    {
    vector< char > delimiters;
    delimiters.push_back('.');
    delimiters.push_back('/');
    delimiters.push_back('-');
    delimiters.push_back('_');
    // moving past https://
    size_t start = url.find( "://" ), end;
    if (start == string::npos)
        return;
    start += 3;

    while ((end = url.findFirstOf(delimiters, start)) != string::npos) {
        vec.push_back(url.substr(start, end - start));
        start = end + 1;
    }

    vec.push_back(url.substr(start));
    }