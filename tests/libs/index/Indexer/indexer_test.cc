#include "tests/catch.hpp"
#include "src/libs/index/Indexer/Indexer.h"
#include "src/libs/index/PostingList/PostingList.h"
#include "src/libs/index/rankingData/rankingData.h"
#include "src/libs/index/index.h"
#include "src/libs/preprocessedDocument/PreprocessedDocument.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/gdfs/gdfs.h"

#include "src/libs/logger/logger.h"

#include <iostream>

using namespace BBG;
Logger logger;

// global to prevent duplicats and copy paste errors 
const string body("cities");
const string title( "bear" );
const string anchor( "anchor" );
const string url( "https/test.com" );

Map<string, uint32_t> bow;
Map<uint32_t, Map<string, uint32_t>> tf;
Map<uint32_t, float> idf; 
Map<string, float> NORM;

// anchor.size() + title.size() + body.size() + docs.size() 
const size_t numUniqueDocElements = 4;

void initRankingData( ) {
   bow[anchor] = 1;
   bow[title] = 2;
   bow[body] = 3;

   uint32_t bodyID = bow[body];
   uint32_t titleID = bow[title];

   tf[titleID][url] = 10.0f;
   tf[bodyID][url] = 20.0f;

   idf[titleID] = 1.0f;
   idf[bodyID] = 2.0f;

   NORM[url] = 5.0f;
}

void makeDummyDocument(PreprocessedDocument & doc) {

   string anchorUrl( "https/anch-test.com" );

   doc.url = url;
   doc.titleText.push_back( title );
   doc.bodyText.push_back( body );
   doc.anchorText.push_back(anchor);
   doc.incomingLinkCount = 0;
   doc.outgoingLinkCount = 0;
   doc.imgCount = 0;
}

TEST_CASE("Indexer extractUrlWords", "[Indexer]") {
   string url1 = "https://test/this/word.com";
   vector<string> expected1;
   expected1.push_back("test");
   expected1.push_back("this");
   expected1.push_back("word");
   expected1.push_back("com");

   vector<string> actual1;
   Indexer::extractUrlWords( url1, actual1 );
   
   REQUIRE( actual1 == expected1 );

   string url2 = "https://test/this/word-your_mom.is.com";
   vector<string> expected2;
   expected2.push_back("test");
   expected2.push_back("this");
   expected2.push_back("word");
   expected2.push_back("your");
   expected2.push_back("mom");
   expected2.push_back("is");
   expected2.push_back("com");

   vector<string> actual2;
   Indexer::extractUrlWords( url2, actual2 );

   REQUIRE( actual2 == expected2 );
}


TEST_CASE("Indexer process doc", "[Indexer]") {
   
   PreprocessedDocument doc;
   makeDummyDocument(doc);

   vector< PreprocessedDocument > docs;
   docs.push_back( doc );

   initRankingData();
   Indexer indexer(bow, tf, idf, NORM);

   indexer.collectFeatures( docs ); 

   uint32_t wordID_1 = indexer.wordToID( title );
   uint32_t wordID_2 = indexer.wordToID( body );

   REQUIRE(indexer.posts[wordID_2][0].getHtmlTag() == 0);
   REQUIRE(indexer.posts[wordID_1][0].getHtmlTag() == 1);
}



TEST_CASE("Indexering dynamic memory: Everything at once", "[Indexer]") {

   WordFeatures wordFeats_2(1.0f);
   WordFeatures wordFeats_3(2.0f);
   DocFeatures docFeats(3, 1, 0, 0, 0, 5.0f); 

   WordDocFeatures wordDocFeats_2(0, 10); 
   WordDocFeatures wordDocFeats_3(0, 20);

//******  Prepare Documents *******//
   PreprocessedDocument doc;
   makeDummyDocument(doc);

   vector< PreprocessedDocument > docs;
   docs.push_back( doc );

//*****  Prepare Index *********//
   Indexer indexer(bow, tf, idf, NORM);
   initRankingData();
   // fillIndexWithDummyRankData(indexer,0);

   size_t bytes = indexer.collectFeatures( docs ); 

   char * ptr = new char[bytes]; 
   IndexChunk * chunk = ( IndexChunk* ) ptr; 

   indexer.writeIndexChunk( chunk, docs ); 

//******* Begin tests ******//

   // test header
   REQUIRE(chunk->GetNumberOfWords() == numUniqueDocElements - 1);
   REQUIRE(chunk->GetNumberOfDocuments() == 1);

   //test posting list for "anchor"   
   uint32_t wordID_1 = indexer.wordToID( anchor );
   PostingList* plist_1 = chunk->getPostingList( wordID_1 );
   REQUIRE( plist_1 != nullptr );
   Post * post1 = plist_1->Seek( 0 ); 
   REQUIRE (post1); 
   REQUIRE(post1->getAbsoluteLocation() == 1);
   REQUIRE(post1->getHtmlTag() == 3);

   //test posting list for "title"   
   uint32_t wordID_2 = indexer.wordToID( title );
   PostingList* plist_2 = chunk->getPostingList( wordID_2 );
   REQUIRE( plist_2 != nullptr );
   Post * post2 = plist_2->Seek( 0 ); 
   REQUIRE (post2); 
   REQUIRE(post2->getAbsoluteLocation() == 2);
   REQUIRE(post2->getHtmlTag() == 1);

   //test posting list for "body"
   uint32_t wordID_3 = indexer.wordToID( body );
   PostingList* plist_3 = chunk->getPostingList( wordID_3 );
   REQUIRE( plist_3 != nullptr );
   Post * post3 = plist_3->Seek( 0 ); 
   REQUIRE (post3); 
   REQUIRE(post3->getAbsoluteLocation() == 3);
   REQUIRE(post3->getHtmlTag() == 0);

   // test end docs
   PostingList* plistEnd = chunk->getEndDocPostingList();
   REQUIRE( plistEnd != nullptr );
   EndDocPost * firstDocEnd = plistEnd->SeekEndDoc(0); 
   REQUIRE (firstDocEnd); 
   REQUIRE(firstDocEnd->getUrl() == "https/test.com");
   REQUIRE(firstDocEnd->getAbsoluteLocation() == 4);
   REQUIRE(firstDocEnd->docStart() == 1);

   // test ranking data
   REQUIRE(plist_2->wordFeatures.idf == wordFeats_2.idf);
   REQUIRE(plist_3->wordFeatures.idf == wordFeats_3.idf);

   REQUIRE(firstDocEnd->docFeatures == docFeats);

   REQUIRE(*plist_2->getWordDocFeature(firstDocEnd ) == wordDocFeats_2);
   REQUIRE(*plist_3->getWordDocFeature(firstDocEnd ) == wordDocFeats_3);
   
   delete[] ptr;
}

TEST_CASE("Indexexing Multiple into dynamic array: RankingData", "[Indexer]") {
//******  Prepare Documents *******//
   vector< PreprocessedDocument > docs;
   for(size_t i = 0; i < 10; ++i) {
      PreprocessedDocument doc;
      makeDummyDocument(doc);
      docs.push_back( doc );
   }

//*****  Prepare Index *********//
   initRankingData();
   Indexer indexer(bow, tf, idf, NORM); 
   
   // for(size_t i = 0; i < 10; ++i)
   //    fillIndexWithDummyRankData(indexer, i);

   size_t bytes = indexer.collectFeatures( docs ); 

   char * ptr = new char[bytes]; 
   IndexChunk * chunk = ( IndexChunk* ) ptr; 

   indexer.writeIndexChunk( chunk, docs ); 

//******* Begin tests ******//
   WordFeatures wordFeats_1(1.0f);
   WordFeatures wordFeats_2(2.0f);
   DocFeatures docFeats(3, 1, 0, 0, 0, 5.0f); 
   WordDocFeatures wordDocFeats_1(0, 10); 
   WordDocFeatures wordDocFeats_2(0, 20);

   uint32_t wordID_1 = indexer.wordToID( title );
   PostingList* plist_1 = chunk->getPostingList( wordID_1 );
   uint32_t wordID_2 = indexer.wordToID( body );
   PostingList* plist_2 = chunk->getPostingList( wordID_2 );
   
   // test ranking data
   REQUIRE(plist_1->wordFeatures == wordFeats_1);
   REQUIRE(plist_2->wordFeatures == wordFeats_2);
   for( size_t i = 0; i < 10; ++i ) {

      EndDocPost * ithDocEnd = chunk->getEndDocPostingList()->SeekEndDoc(i * numUniqueDocElements + 3);
      REQUIRE( ithDocEnd->docStart() == i * numUniqueDocElements + 1); // extra 1 for chunk offset, 0 reserved
      REQUIRE(ithDocEnd->docFeatures == docFeats);

      REQUIRE(*plist_1->getWordDocFeature(ithDocEnd ) == wordDocFeats_1);
      REQUIRE(*plist_2->getWordDocFeature(ithDocEnd ) == wordDocFeats_2);
   }
   delete[] ptr;
}


TEST_CASE("Indexexing into file: Header", "[Indexer]") {
 //******  Prepare Documents *******//
   PreprocessedDocument doc;
   makeDummyDocument(doc);

   vector< PreprocessedDocument > docs;
   docs.push_back( doc );

//*****  Prepare Index *********//
   initRankingData();
   Indexer indexer(bow, tf, idf, NORM); 

   indexer.createIndexChunk("pls.bin", docs);

   IndexFileReader ifile("pls.bin") ;
   IndexChunk* chunk = ifile.Load();
   // test header
   REQUIRE(chunk->GetNumberOfWords() == numUniqueDocElements - 1);
   REQUIRE(chunk->GetNumberOfDocuments() == 1);
}

TEST_CASE("Indexexing into file: Posts", "[Indexer]") {
 //******  Prepare Documents *******//
   
   PreprocessedDocument doc;
   makeDummyDocument(doc);
   vector< PreprocessedDocument > docs;
   docs.push_back( doc );

   //*****  Prepare Index *********//
   initRankingData();
   Indexer indexer(bow, tf, idf, NORM); 
   indexer.createIndexChunk("pls.bin", docs);
   IndexFileReader ifile("pls.bin") ;
   IndexChunk* chunk = ifile.Load();

//******* Begin tests ******//

   //test posting list for "anchor"   
   uint32_t wordID_1 = indexer.wordToID( anchor );
   PostingList* plist_1 = chunk->getPostingList( wordID_1 );
   REQUIRE( plist_1 != nullptr );
   Post * post1 = plist_1->Seek( 0 ); 
   REQUIRE (post1); 
   REQUIRE(post1->getAbsoluteLocation() == 1);
   REQUIRE(post1->getHtmlTag() == 3);

   //test posting list for "title"   
   uint32_t wordID_2 = indexer.wordToID( title );
   PostingList* plist_2 = chunk->getPostingList( wordID_2 );
   REQUIRE( plist_2 != nullptr );
   Post * post2 = plist_2->Seek( 0 ); 
   REQUIRE (post2); 
   REQUIRE(post2->getAbsoluteLocation() == 2);
   REQUIRE(post2->getHtmlTag() == 1);

   //test posting list for "body"
   uint32_t wordID_3 = indexer.wordToID( body );
   PostingList* plist_3 = chunk->getPostingList( wordID_3 );
   REQUIRE( plist_3 != nullptr );
   Post * post3 = plist_3->Seek( 0 ); 
   REQUIRE (post3); 
   REQUIRE(post3->getAbsoluteLocation() == 3);
   REQUIRE(post3->getHtmlTag() == 0);

   // test end docs
   PostingList* plistEnd = chunk->getEndDocPostingList();
   REQUIRE( plistEnd != nullptr );
   EndDocPost * firstDocEnd = plistEnd->SeekEndDoc(0); 
   REQUIRE (firstDocEnd); 
   REQUIRE(firstDocEnd->getUrl() == "https/test.com");
   REQUIRE(firstDocEnd->getAbsoluteLocation() == 4);
   REQUIRE(firstDocEnd->docStart() == 1);
}

TEST_CASE("Indexexing into file: EndDocPost", "[Indexer]") {
 //******  Prepare Documents *******//

   PreprocessedDocument doc;
   makeDummyDocument(doc);
   vector< PreprocessedDocument > docs;
   docs.push_back( doc );

   //*****  Prepare Index *********//
   initRankingData();
   Indexer indexer(bow, tf, idf, NORM); 
   indexer.createIndexChunk("pls.bin", docs);
   IndexFileReader ifile("pls.bin") ;
   IndexChunk* chunk = ifile.Load();

//******* Begin tests ******//
   // test end docs
   PostingList* plistEnd = chunk->getEndDocPostingList();
   REQUIRE( plistEnd != nullptr );
   EndDocPost * firstDocEnd = plistEnd->SeekEndDoc(0); 
   REQUIRE (firstDocEnd); 
   REQUIRE(firstDocEnd->getUrl() == "https/test.com");
   REQUIRE(firstDocEnd->getAbsoluteLocation() == 4);
   REQUIRE(firstDocEnd->docStart() == 1);
}



TEST_CASE("Indexexing into file: RankingData", "[Indexer]") {
//******  Prepare Documents *******//
   PreprocessedDocument doc;
   makeDummyDocument(doc);

   vector< PreprocessedDocument > docs;
   docs.push_back( doc );

//*****  Prepare Index *********//
   initRankingData();
   Indexer indexer(bow, tf, idf, NORM); 

   indexer.createIndexChunk("pls.bin", docs);

   IndexFileReader ifile("pls.bin") ;
   IndexChunk* chunk = ifile.Load();

//******* Begin tests ******//
   WordFeatures wordFeats_1(1.0f);
   WordFeatures wordFeats_2(2.0f);
   DocFeatures docFeats(3, 1, 0, 0, 0, 5.0f); 
   WordDocFeatures wordDocFeats_1(0, 10); 
   WordDocFeatures wordDocFeats_2(0, 20);

   uint32_t wordID_1 = indexer.wordToID( title );
   PostingList* plist_1 = chunk->getPostingList( wordID_1 );
   uint32_t wordID_2 = indexer.wordToID( body );
   PostingList* plist_2 = chunk->getPostingList( wordID_2 );
   
   EndDocPost * firstDocEnd = chunk->getEndDocPostingList()->SeekEndDoc(0);

   // test ranking data
   REQUIRE(plist_1->wordFeatures == wordFeats_1);
   REQUIRE(plist_2->wordFeatures == wordFeats_2);
//   docFeats.length = firstDocEnd->docFeatures.length;

   REQUIRE(firstDocEnd->docFeatures == docFeats);

   REQUIRE(*plist_1->getWordDocFeature(firstDocEnd ) == wordDocFeats_1);
   REQUIRE(*plist_2->getWordDocFeature(firstDocEnd ) == wordDocFeats_2);
}


TEST_CASE("Indexexing mulitple into file: Header", "[Indexer]") {
 //******  Prepare Documents *******//
   const size_t loopNumber = 10; 

   vector< PreprocessedDocument > docs;
   for(size_t i = 0; i < loopNumber; ++i) {
      PreprocessedDocument doc;
      makeDummyDocument(doc);
      docs.push_back( doc );
   }

//*****  Prepare Index *********//
   initRankingData();
   Indexer indexer(bow, tf, idf, NORM); 

   indexer.createIndexChunk("pls.bin", docs);

   IndexFileReader ifile("pls.bin") ;
   IndexChunk* chunk = ifile.Load();

   // test header
   REQUIRE(chunk->GetNumberOfWords() == ( numUniqueDocElements - 1 ) * loopNumber);
   REQUIRE(chunk->GetNumberOfDocuments() == loopNumber);
}

TEST_CASE("Indexexing Multiple into file: Posts", "[Indexer]") {
 //******  Prepare Documents *******//
   const size_t loopNumber = 10;
 
   PreprocessedDocument doc;
   vector< PreprocessedDocument > docs;
   for(size_t i = 0; i < loopNumber; ++i) {
      PreprocessedDocument doc;
      makeDummyDocument(doc);
      docs.push_back( doc );
   }

   //*****  Prepare Index *********//
   initRankingData();
   Indexer indexer(bow, tf, idf, NORM); 
   // for( size_t i = 0; i < loopNumber; ++i)
   //    fillIndexWithDummyRankData(indexer, i);
   indexer.createIndexChunk("pls.bin", docs);
   IndexFileReader ifile("pls.bin") ;
   IndexChunk* chunk = ifile.Load();

//******* Begin tests ******//

   for( size_t i = 0; i < loopNumber; ++i) {
      
      uint32_t wordID_0 = indexer.wordToID( anchor );
      PostingList* plist_0 = chunk->getPostingList( wordID_0 );
      REQUIRE( plist_0 != nullptr );
      Post * post0 = plist_0->Seek( i*numUniqueDocElements ); 
      REQUIRE (post0); 
      REQUIRE(post0->getAbsoluteLocation() ==  i*numUniqueDocElements + 1) ;
      REQUIRE(post0->getHtmlTag() == 3);
      
      //test posting list for "title"   
      uint32_t wordID_1 = indexer.wordToID( title );
      PostingList* plist_1 = chunk->getPostingList( wordID_1 );
      REQUIRE( plist_1 != nullptr );
      Post * post1 = plist_1->Seek( i*numUniqueDocElements ); 
      REQUIRE (post1); 
      REQUIRE(post1->getAbsoluteLocation() ==  i*numUniqueDocElements + 2) ;
      REQUIRE(post1->getHtmlTag() == 1);

      //test posting list for "body"
      uint32_t wordID_2 = indexer.wordToID( body );
      PostingList* plist_2 = chunk->getPostingList( wordID_2 );
      REQUIRE( plist_2 != nullptr );
      Post * post2 = plist_2->Seek( i*numUniqueDocElements ); 
      REQUIRE (post2); 
      REQUIRE(post2->getAbsoluteLocation() == i*numUniqueDocElements + 3);
      REQUIRE(post2->getHtmlTag() == 0);
   }
}

TEST_CASE("Indexexing Multiple into file: EndDocPost", "[Indexer]") {
 //******  Prepare Documents *******//

   vector< PreprocessedDocument > docs;
   for(size_t i = 0; i < 10; ++i) {
      PreprocessedDocument doc;
      makeDummyDocument(doc);
      docs.push_back( doc );
   }

   //*****  Prepare Index *********//
   initRankingData();
   Indexer indexer(bow, tf, idf, NORM); 
   // for( size_t i = 0; i < 10; ++i)
   //    fillIndexWithDummyRankData(indexer,i);
   indexer.createIndexChunk("pls.bin", docs);
   IndexFileReader ifile("pls.bin") ;
   IndexChunk* chunk = ifile.Load();

//******* Begin tests ******//
   for(size_t i = 0; i < 10; ++i) {
      // test end docs
      PostingList* plistEnd = chunk->getEndDocPostingList();
      REQUIRE( plistEnd != nullptr );
      EndDocPost * firstDocEnd = plistEnd->SeekEndDoc(i*numUniqueDocElements + 4); 
      REQUIRE (firstDocEnd); 
      REQUIRE(firstDocEnd->getUrl() == "https/test.com");
      REQUIRE(firstDocEnd->getAbsoluteLocation() == i*numUniqueDocElements + 4 );
      REQUIRE(firstDocEnd->docStart() == i * numUniqueDocElements + 1); // extra 1 for chunk offset, 0 reserved
   }
}


// TEST_CASE("Indexexing Multiple into file: RankingData", "[Indexer]") {
// //******  Prepare Documents *******//

//    vector< PreprocessedDocument > docs;
//    for(size_t i = 0; i < 10; ++i) {
//       PreprocessedDocument doc;
//       makeDummyDocument(doc);
//       docs.push_back( doc );
//    }

// //*****  Prepare Index *********//
//    initRankingData();
//    Indexer indexer(bow, tf, idf, NORM); 
//    // for(size_t i = 0; i < 10; ++i)
//    //    fillIndexWithDummyRankData(indexer,i);

//    indexer.createIndexChunk("pls.bin", docs);

//    IndexFileReader ifile("pls.bin") ;
//    IndexChunk* chunk = ifile.Load();

// //******* Begin tests ******//
//    WordFeatures wordFeats_1(1.0f);
//    WordFeatures wordFeats_2(2.0f);
//    DocFeatures docFeats;
//    docFeats.docNorm = 5.0f;
//    docFeats.numAnchorWords = 1;
//    docFeats.titleLength = 1;
//    WordDocFeatures wordDocFeats_1(0, 10); 
//    WordDocFeatures wordDocFeats_2(0, 20);

//    // title.size() + body.size() + docs.size() 
//    size_t numUniqueDocElements = 3;

//    uint32_t wordID_1 = indexer.wordToID( title );
//    PostingList* plist_1 = chunk->getPostingList( wordID_1 );
//    uint32_t wordID_2 = indexer.wordToID( body );
//    PostingList* plist_2 = chunk->getPostingList( wordID_2 );
   
//    // test ranking data
//    REQUIRE(plist_1->wordFeatures == wordFeats_1);
//    REQUIRE(plist_2->wordFeatures == wordFeats_2);
//    for(size_t i = 0; i < 10; ++i) {

//       EndDocPost * ithDocEnd = chunk->getEndDocPostingList()->SeekEndDoc(i * numUniqueDocElements + 3);
//       REQUIRE(ithDocEnd->docStart() == i * numUniqueDocElements + 1); // extra 1 for chunk initial location, 0 reserved
//       docFeats.length = ithDocEnd->docFeatures.length;
//       REQUIRE(ithDocEnd->docFeatures == docFeats);

//       REQUIRE(*plist_1->getWordDocFeature(ithDocEnd ) == wordDocFeats_1);
//       REQUIRE(*plist_2->getWordDocFeature(ithDocEnd ) == wordDocFeats_2);
//    }
// }
