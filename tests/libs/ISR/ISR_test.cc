#define CATCH_CONFIG_MAIN
#include "tests/catch.hpp"
#include "src/libs/index/PostingList/PostingList.h"
#include "src/libs/ISR/ISR.h"

using namespace BBG;


///////// PostingList helpers /////////

PostingList* getQuickPostingList(size_t NUM_DOCS) {
   vector<Post> quickPosts;
   quickPosts.push_back(Post(10, 3));
   quickPosts.push_back(Post(27, 3));
   quickPosts.push_back(Post(105, 3));
   quickPosts.push_back(Post(513, 3));
   quickPosts.push_back(Post(518, 3));
   quickPosts.push_back(Post(520, 3));
   quickPosts.push_back(Post(2000, 3));
   quickPosts.push_back(Post(2200, 3));
   quickPosts.push_back(Post(2203, 3));
   quickPosts.push_back(Post(2400, 3));


   size_t bytes = PostingList::postBytesRequired(quickPosts.size(), NUM_DOCS);

   PostingList* quickPList = (PostingList*) new char[bytes];

   uint32_t offset = PostingList::WriteSeekTable(quickPList, quickPosts);
   offset = PostingList::addPosts(quickPList, offset, quickPosts);

   return quickPList;
}

PostingList* getBrownPostingList(size_t NUM_DOCS) {
   vector<Post> brownPosts;
   brownPosts.push_back(Post(28, 3));
   brownPosts.push_back(Post(50, 3));
   brownPosts.push_back(Post(62, 3));
   brownPosts.push_back(Post(70, 3));
   brownPosts.push_back(Post(514, 3));
   brownPosts.push_back(Post(790, 3));
   brownPosts.push_back(Post(2001, 3));
   brownPosts.push_back(Post(2100, 3));
   brownPosts.push_back(Post(2201, 3));
   brownPosts.push_back(Post(2300, 3));

   size_t bytes = PostingList::postBytesRequired(brownPosts.size(), NUM_DOCS);
   PostingList* brownPList = (PostingList*) new char[bytes];

   uint32_t offset = PostingList::WriteSeekTable(brownPList, brownPosts);
   offset = PostingList::addPosts(brownPList, offset, brownPosts);

   return brownPList;
}

PostingList* getFoxPostingList(size_t NUM_DOCS) {
   vector<Post> foxPosts;
   foxPosts.push_back(Post(87, 3));
   foxPosts.push_back(Post(106, 3));
   foxPosts.push_back(Post(515, 3));
   foxPosts.push_back(Post(550, 3));
   foxPosts.push_back(Post(1200, 3));
   foxPosts.push_back(Post(2101, 3));
   foxPosts.push_back(Post(2202, 3));
   foxPosts.push_back(Post(2401, 3));

   size_t bytes = PostingList::postBytesRequired(foxPosts.size(), NUM_DOCS);
   PostingList* foxPList = (PostingList*) new char[bytes];

   uint32_t offset = PostingList::WriteSeekTable(foxPList, foxPosts);
   offset = PostingList::addPosts(foxPList, offset, foxPosts);

   return foxPList;
}

PostingList* getEndDocPostingList() {
   vector<EndDocPost> endDocPosts;
	endDocPosts.push_back(EndDocPost(112, 0));
	endDocPosts.push_back(EndDocPost(570, 0));
	endDocPosts.push_back(EndDocPost(1006, 0));
	endDocPosts.push_back(EndDocPost(1704, 0));
	endDocPosts.push_back(EndDocPost(2004, 0));
	endDocPosts.push_back(EndDocPost(2104, 0));
	endDocPosts.push_back(EndDocPost(2204, 0));
	endDocPosts.push_back(EndDocPost(2304, 0));
	endDocPosts.push_back(EndDocPost(2404, 0));

   endDocPosts[0].docFeatures.numWords = 111;
   for (int i = 1; i < endDocPosts.size(); ++i) {
      endDocPosts[i].docFeatures.numWords = endDocPosts[i].getAbsoluteLocation() -
         endDocPosts[i - 1].getAbsoluteLocation() - 1;
   }


   vector<PreprocessedDocument> docs;

   string s0("test 0");
   string s1("test 1");
   string s2("test 2");
   string s3("test 3");
   string s4("test 4");
   string s5("test 5");
   string s6("test 6");
   string s7("test 7");
   string s8("test 8");

   PreprocessedDocument d0;
   PreprocessedDocument d1;
   PreprocessedDocument d2;
   PreprocessedDocument d3;
   PreprocessedDocument d4;
   PreprocessedDocument d5;
   PreprocessedDocument d6;
   PreprocessedDocument d7;
   PreprocessedDocument d8;

   d0.url = s0;
   d1.url = s1;
   d2.url = s2;
   d3.url = s3;
   d4.url = s4;
   d5.url = s5;
   d6.url = s6;
   d7.url = s7;
   d8.url = s8;

   d0.titleText.push_back(s0);
   d1.titleText.push_back(s1);
   d2.titleText.push_back(s2);
   d3.titleText.push_back(s3);
   d4.titleText.push_back(s4);
   d5.titleText.push_back(s5);
   d6.titleText.push_back(s6);
   d7.titleText.push_back(s7);
   d8.titleText.push_back(s8);

   docs.push_back(d0);
   docs.push_back(d1);
   docs.push_back(d2);
   docs.push_back(d3);
   docs.push_back(d4);
   docs.push_back(d5);
   docs.push_back(d6);
   docs.push_back(d7);
   docs.push_back(d8);

   size_t bytes = PostingList::endDocBytesRequired(docs);

   // Account for urls
   // bytes += endDocPosts.size() * ( 7 + 7 ); // each "url" is of size 7

   PostingList* endDocPlist = (PostingList*) new char[bytes];
   // endDocPlist->word = 0;

   uint32_t offset = PostingList::WriteSeekTable(endDocPlist, endDocPosts);
   PostingList::addEndPostsAndUrls(endDocPlist, offset, endDocPosts, docs);

   return endDocPlist;
}

///////// Test cases /////////

TEST_CASE("Simple quick word ISR query", "[ISR]") {
   const size_t NUM_DOCS = 9;
   PostingList* endDocPList = getEndDocPostingList();
   PostingList* quickPList = getQuickPostingList(NUM_DOCS);

   ISR* quickISR = new ISRWord(quickPList, endDocPList);
   
   DocumentMatch res = quickISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 112);

   res = quickISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 570);

   res = quickISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2004);

   res = quickISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2204);

   res = quickISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2404);

   // No more document matches
   res = quickISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);

   // Result should not change unless we call seek
   res = quickISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);
}

TEST_CASE("Simple '( quick OR brown OR fox )' query", "[ISR]") {
   const size_t NUM_DOCS = 9;
   PostingList* endDocPList = getEndDocPostingList();
   PostingList* quickPList = getQuickPostingList(NUM_DOCS);
   PostingList* brownPList = getBrownPostingList(NUM_DOCS);
   PostingList* foxPList = getFoxPostingList(NUM_DOCS);


   ISR* quickISR = new ISRWord(quickPList, endDocPList);
   ISR* brownISR = new ISRWord(brownPList, endDocPList);
   ISR* foxISR = new ISRWord(foxPList, endDocPList);
   ISROr* orISR = new ISROr();
   
   orISR->addTerm(quickISR);
   orISR->addTerm(brownISR);
   orISR->addTerm(foxISR);
   
   DocumentMatch res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 112);

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 570);

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 1006);

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 1704);

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2004);

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2104);

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2204);

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2304);

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2404);

   // No more document matches
   res = orISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);

   // Result should not change unless we call seek
   res = orISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);
   
}

TEST_CASE("Simple '( quick AND fox )' query", "[ISR]") {
   const size_t NUM_DOCS = 9;

   PostingList* endDocPList = getEndDocPostingList();
   PostingList* quickPList = getQuickPostingList(NUM_DOCS);
   PostingList* foxPList = getFoxPostingList(NUM_DOCS);

   ISR* quickISR = new ISRWord(quickPList, endDocPList);
   ISR* foxISR = new ISRWord(foxPList, endDocPList);
   ISRAnd* andISR = new ISRAnd();
   andISR->addTerm(quickISR);
   andISR->addTerm(foxISR);
   
   DocumentMatch res = andISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 112);

   res = andISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 570);

   res = andISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2204);

   res = andISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2404);

   // No more document matches
   res = andISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);

   // Result should not change unless we call seek
   res = andISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);
}

TEST_CASE("'( quick AND fox ) OR brown' query", "[ISR]") {
   const size_t NUM_DOCS = 9;
   PostingList* endDocPList = getEndDocPostingList();
   PostingList* quickPList = getQuickPostingList(NUM_DOCS);
   PostingList* brownPList = getBrownPostingList(NUM_DOCS);
   PostingList* foxPList = getFoxPostingList(NUM_DOCS);

   ISR* quickISR = new ISRWord(quickPList, endDocPList);
   ISR* brownISR = new ISRWord(brownPList, endDocPList);
   ISR* foxISR = new ISRWord(foxPList, endDocPList);

   ISRAnd* andISR = new ISRAnd();
   andISR->addTerm(quickISR);
   andISR->addTerm(foxISR);

   ISROr* orISR = new ISROr();
   orISR->addTerm(andISR);
   orISR->addTerm(brownISR);
   
   DocumentMatch res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 112); // both match

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 570); // both match

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 1006); // only brown matches

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2004); // brown matches

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2104); // brown matches

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2204); // both match

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2304); // brown matches

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2404); // quick and fox match

   // No more document matches
   res = orISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);

   // Result should not change unless we call seek
   res = orISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);
}

TEST_CASE("'( quick AND brown ) OR ( brown AND fox )' query", "[ISR]") {
   const size_t NUM_DOCS = 9;
   PostingList* endDocPList = getEndDocPostingList();
   PostingList* quickPList = getQuickPostingList(NUM_DOCS);
   PostingList* brownPList = getBrownPostingList(NUM_DOCS);
   PostingList* foxPList = getFoxPostingList(NUM_DOCS);

   ISR* quickISR = new ISRWord(quickPList, endDocPList);
   ISR* brownISR_1 = new ISRWord(brownPList, endDocPList);
   ISR* brownISR_2 = new ISRWord(brownPList, endDocPList);
   ISR* foxISR = new ISRWord(foxPList, endDocPList);

   ISRAnd* andISR_1 = new ISRAnd();
   andISR_1->addTerm(quickISR);
   andISR_1->addTerm(brownISR_1);

   ISRAnd* andISR_2 = new ISRAnd();
   andISR_2->addTerm(brownISR_2);
   andISR_2->addTerm(foxISR);

   ISROr* orISR = new ISROr();
   orISR->addTerm(andISR_1);
   orISR->addTerm(andISR_2);
   
   DocumentMatch res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 112); // all three match

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 570); // all three match

   // Document ends 1006 and 1704 do not match

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2004); // quick and brown

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2104); // brown and fox

   res = orISR->NextDocument();
   REQUIRE(res.docEnd != nullptr);
   REQUIRE(res.docEnd->getAbsoluteLocation() == 2204); // all three match

   // Document ends 2304 and 2404 do not match

   // No more document matches
   res = orISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);
   printf("at line 394");

   // Result should not change unless we call seek
   res = orISR->NextDocument();
   REQUIRE(res.docEnd == nullptr);
   printf("end of last test");
}