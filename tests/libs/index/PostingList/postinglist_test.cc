#define CATCH_CONFIG_MAIN
#include "tests/catch.hpp"
#include "src/libs/index/PostingList/PostingList.h"
#include "src/libs/index/Post/Post.h"
#include "src/libs/preprocessedDocument/PreprocessedDocument.h"

using namespace BBG;

TEST_CASE("Simple Post test", "[PostingList]") {
  Post actual(2, 0);
  
  Post post(3, 1);
  post.setLocation(2);
  post.setTag(0);
  REQUIRE(post == actual);

  EndDocPost endActual(2, 0);
  EndDocPost endPost(3, 0);
  endPost.setLocation(2);
  REQUIRE(endPost == endActual);
}

TEST_CASE("Simple post at start, With No Feautures", "[PostingList]") {
	vector<Post> posts;
	Post expected(1000, 3); // seek table index 0
	posts.push_back(expected);

	size_t bytes = PostingList::postBytesRequired(posts.size(), 0);
	PostingList* plist = (PostingList*) new char[bytes];

	uint32_t offset = PostingList::WriteSeekTable(plist, posts);
  offset = PostingList::addPosts(plist, offset, posts);

  const Post* actual_start = plist->Seek(0);
  const Post* actual_close = plist->Seek(999);
  const Post* actual = plist->Seek(1000);
  const Post* actual_after = plist->Seek(1001);
  const Post* actual_nextPost = plist->Seek(0x3FFFF);
  const Post* actual_wayPast = plist->Seek(0xFFFFFF);

  REQUIRE(bytes == offset);

  REQUIRE(expected == *actual_start);
  REQUIRE(expected == *actual_close);
  REQUIRE(expected == *actual);


  REQUIRE(actual == actual_start);
  REQUIRE(actual_start->getHtmlTag() == 3);
  REQUIRE(actual_start->getAbsoluteLocation() == 1000);

  REQUIRE(actual == actual_close);
  REQUIRE(actual_close->getHtmlTag() == 3);
  REQUIRE(actual_close->getAbsoluteLocation() == 1000);

  REQUIRE(nullptr == actual_after);
  REQUIRE(nullptr == actual_nextPost);
  REQUIRE(nullptr == actual_wayPast);
}

TEST_CASE("Multiple post test", "[PostingList]") {
  vector<Post> posts;
  vector<uint32_t> seekPos;

  seekPos.push_back(0);
  seekPos.push_back(2);
  seekPos.push_back((1 << SeekTableEntry::numLowBits) + 0xFF);
  seekPos.push_back((3 << SeekTableEntry::numLowBits) + 0xFF);
  seekPos.push_back((128 << SeekTableEntry::numLowBits) + 0xFF);
	
	Post a(seekPos[0], 0);
	Post b(seekPos[1], 0);
	Post c(seekPos[2], 1);
	Post d(seekPos[3], 2);
	Post e(seekPos[4], 3);

	posts.push_back(a);
	posts.push_back(b);
	posts.push_back(c);
	posts.push_back(d);
	posts.push_back(e);

  size_t bytes = PostingList::postBytesRequired(posts.size(), 0);

	PostingList* plist = (PostingList*) new char[bytes];
	uint32_t offset = PostingList::WriteSeekTable(plist, posts);
  offset = PostingList::addPosts(plist, offset, posts);

  // 0
  REQUIRE(posts[0].getAbsoluteLocation() == plist->Seek(0)->getAbsoluteLocation());

  // 2
  /* REQUIRE(posts[1] == *(plist->Seek(1))); */
  REQUIRE(posts[1].getAbsoluteLocation() == plist->Seek(1)->getAbsoluteLocation());
  REQUIRE(posts[1].getAbsoluteLocation() == plist->Seek(2)->getAbsoluteLocation());
  
  REQUIRE( 0 == plist->Seek(2)->getHtmlTag() );
  /* REQUIRE(posts[1] == *(plist->Seek(2))); */

  // 0x0400FF
  REQUIRE(posts[2] == *(plist->Seek(3)));
  REQUIRE(posts[2] == *(plist->Seek(seekPos[2] - 1)));
  REQUIRE(posts[2] == *(plist->Seek(seekPos[2])));

  REQUIRE( 1 == (plist->Seek(3))->getHtmlTag() );

  // 0x0F00FF
  REQUIRE(posts[3] == *(plist->Seek(seekPos[2] + 1))); // index 1
  REQUIRE(posts[3] == *(plist->Seek(2 << SeekTableEntry::numLowBits))); // index 2
  REQUIRE(posts[3] == *(plist->Seek(seekPos[3] - 1))); // index 3
  REQUIRE(posts[3] == *(plist->Seek(seekPos[3]))); // index 3

  REQUIRE( 2 == (plist->Seek(seekPos[2] + 1))->getHtmlTag() );

  // 0xFF00FF
  REQUIRE(posts[4] == *(plist->Seek(seekPos[3] + 1))); // index 3
  REQUIRE(posts[4] == *(plist->Seek(64 << SeekTableEntry::numLowBits))); // index 64
  REQUIRE(posts[4] == *(plist->Seek(seekPos[4] - 1))); // index 128
  REQUIRE(posts[4] == *(plist->Seek(seekPos[4]))); // index 128
  /* REQUIRE(nullptr == plist->Seek(seekPos[4] + 1));  */

  REQUIRE( 3 == (plist->Seek(seekPos[4] - 1))->getHtmlTag() ); // index 128
}

TEST_CASE("Multiple end post test, No features", "[PostingList]") {
  vector<EndDocPost> posts;
  vector<uint32_t> seekPos;

  seekPos.push_back(0);
  seekPos.push_back(2);
  seekPos.push_back((1 << SeekTableEntry::numLowBits) + 0xFF);
  seekPos.push_back((3 << SeekTableEntry::numLowBits) + 0xFF);
  seekPos.push_back((128 << SeekTableEntry::numLowBits) + 0xFF);
	
	EndDocPost a(seekPos[0],0);
	EndDocPost b(seekPos[1],0);
	EndDocPost c(seekPos[2],0);
	EndDocPost d(seekPos[3],0);
	EndDocPost e(seekPos[4],0);

	posts.push_back(a);
	posts.push_back(b);
	posts.push_back(c);
	posts.push_back(d);
	posts.push_back(e);

  vector<PreprocessedDocument> docs;

  string s0("test 0");
  string s1("test 1");
  string s2("test 2");
  string s3("test 3");
  string s4("test 4");

  PreprocessedDocument d0;
  PreprocessedDocument d1;
  PreprocessedDocument d2;
  PreprocessedDocument d3;
  PreprocessedDocument d4;

  d0.url = s0;
  d1.url = s1;
  d2.url = s2;
  d3.url = s3;
  d4.url = s4;

  d0.titleText.push_back(s0);
  d1.titleText.push_back(s1);
  d2.titleText.push_back(s2);
  d3.titleText.push_back(s3);
  d4.titleText.push_back(s4);

  docs.push_back(d0);
  docs.push_back(d1);
  docs.push_back(d2);
  docs.push_back(d3);
  docs.push_back(d4);

	size_t bytes = PostingList::endDocBytesRequired( docs ); 
  // Account for urls
  // bytes += docs.size( ) * (7+7);
  //SeekTableEntry::TableSize * sizeof(SeekTableEntry) + sizeof(PostingList) + sizeof(EndDocPost) * posts.size();
	
  PostingList* plist = (PostingList*) new char[bytes];
	// plist->word = 0;

	uint32_t offset = PostingList::WriteSeekTable(plist, posts);
  PostingList::addEndPostsAndUrls(plist, offset, posts,  docs);

  // 0
  REQUIRE(posts[0].getAbsoluteLocation() == plist->SeekEndDoc(0)->getAbsoluteLocation());
  REQUIRE(plist->SeekEndDoc(seekPos[0])->getUrl() == docs[0].url);
  REQUIRE(plist->SeekEndDoc(seekPos[0])->getTitle() == docs[0].url);

  // 2
  REQUIRE(posts[1].getAbsoluteLocation() == plist->SeekEndDoc(1)->getAbsoluteLocation());
  REQUIRE(posts[1].getAbsoluteLocation() == plist->SeekEndDoc(2)->getAbsoluteLocation());
  REQUIRE(plist->SeekEndDoc(seekPos[1])->getUrl() == docs[1].url);
  REQUIRE(plist->SeekEndDoc(seekPos[1])->getTitle() == docs[1].url);

  // 0x0400FF
  REQUIRE(posts[2].getAbsoluteLocation() == plist->SeekEndDoc(3)->getAbsoluteLocation());
  REQUIRE(posts[2].getAbsoluteLocation() == plist->SeekEndDoc(seekPos[2] - 1)->getAbsoluteLocation());
  REQUIRE(posts[2].getAbsoluteLocation() == plist->SeekEndDoc(seekPos[2])->getAbsoluteLocation());
  REQUIRE(plist->SeekEndDoc(seekPos[2])->getUrl() == docs[2].url);
  REQUIRE(plist->SeekEndDoc(seekPos[2])->getTitle() == docs[2].url);

  // 0x0F00FF
  REQUIRE(posts[3].getAbsoluteLocation() == plist->SeekEndDoc(seekPos[2] + 1)->getAbsoluteLocation()); // index 1
  REQUIRE(posts[3].getAbsoluteLocation() == plist->SeekEndDoc(2 << SeekTableEntry::numLowBits)->getAbsoluteLocation()); // index 2
  REQUIRE(posts[3].getAbsoluteLocation() == plist->SeekEndDoc(seekPos[3] - 1)->getAbsoluteLocation()); // index 3
  REQUIRE(posts[3].getAbsoluteLocation() == plist->SeekEndDoc(seekPos[3])->getAbsoluteLocation()); // index 3
  REQUIRE(plist->SeekEndDoc(seekPos[3])->getUrl() == "test 3");
  REQUIRE(plist->SeekEndDoc(seekPos[3])->getTitle() == "test 3");

  // 0xFF00FF
  REQUIRE(posts[4].getAbsoluteLocation() == plist->SeekEndDoc(seekPos[3] + 1)->getAbsoluteLocation()); // index 3
  REQUIRE(posts[4].getAbsoluteLocation() == plist->SeekEndDoc(64 << SeekTableEntry::numLowBits)->getAbsoluteLocation()); // index 64
  REQUIRE(posts[4].getAbsoluteLocation() == plist->SeekEndDoc(seekPos[4] - 1)->getAbsoluteLocation()); // index 128
  REQUIRE(posts[4].getAbsoluteLocation() == plist->SeekEndDoc(seekPos[4])->getAbsoluteLocation()); // index 128
  REQUIRE(plist->SeekEndDoc(seekPos[4])->getUrl() == "test 4");
  REQUIRE(plist->SeekEndDoc(seekPos[4])->getTitle() == "test 4");
}

