#define CATCH_CONFIG_MAIN
#include "tests/catch.hpp"
#include "src/libs/index/Post/Post.h"

using namespace BBG;

TEST_CASE("Simple Post test", "[Post]") {
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

TEST_CASE("Simple Byte encoding", "[Post]") {
  char * buf = new char[1000];
  
  Post actual(127, 63);
  EndDocPost endActual(1025, 513);
  // encode returns ptr to location just passed data, so where endDoc will start
  char * endStart = actual.encode(buf);
  
  endActual.encode(endStart);

  Post post;
  // decode will fill post member var with data from buf
  post.decode(buf);

  EndDocPost endPost;
  endPost.decode(endStart);

  REQUIRE(post == actual);
  REQUIRE(endPost == endActual);
  delete[] buf;
}


TEST_CASE("Big byte encoding", "[Post]") {
  char * buf = new char[10000];
  char * curBufPosition = buf;
  for (size_t i = 0; i < 30; ++i ) {
    Post post(120 + i, 60 + i);
    EndDocPost endPost(1000 + i, 500 + i);
    curBufPosition = post.encode(curBufPosition);
    curBufPosition = endPost.encode(curBufPosition);
  }
  // reset current buffer postition to start of buf
  const char * decodePosition = buf;
  for (size_t i = 0; i < 30; ++i ) {
    Post post;
    EndDocPost endPost;
    decodePosition = post.decode(decodePosition);
    decodePosition = endPost.decode(decodePosition);
    REQUIRE(post == Post(120 + i, 60 + i) );
    REQUIRE(endPost == EndDocPost(1000 + i, 500 + i));
  }
  delete[] buf;
}