#include "src/libs/frontier/shuffleQueue/shuffleQueue.h"
// TODO: remove this include once ShuffleDatum is put in own file
#include "src/libs/frontier/urlQueue/urlQueue.h"
#include "src/libs/string/string.h"
#include "tests/catch.hpp"

using BBG::ShuffleQueue;
// using BBG::ShuffleDatum;
using BBG::string;


// // For debugging
// #include <iostream>
// using std::cout;
// using std::endl;
// void print_queue(ShuffleQueue q) {
//     cout << "QUEUE PRINT:" << endl;
//     while (!q.empty()) {
//         cout << q.front() << endl;
//         q.pop();
//     }
//     cout << "QUEUE PRINT DONE" << endl;
// }


// TEST_CASE("Default constructor", "[shuffleQueue]") {
//     ShuffleQueue<string> q(10, 2, 2);
//     REQUIRE(q.size() == 0);
//     REQUIRE(q.capacity() == 10);
//     REQUIRE(q.empty());
// }

// TEST_CASE("front and push", "[shuffleQueue]") {
//     ShuffleQueue<string> q(10, 2, 2);
//     q.push("cat");
//     REQUIRE(q.size() == 1);
//     REQUIRE(q.front() == "cat");
//     q.pop();
//     REQUIRE(q.size() == 0);

// }

// TEST_CASE("shuffle_push single", "[shuffleQueue]") {
//     ShuffleQueue<string> q(5, 2, 2);
//     q.push("cat");
//     q.push("dog");
//     q.push("hat");
//     q.push("mat");
//     REQUIRE(q.size() == 4);
//     q.shuffle_push("jam");
//     // mat should be located between dog and hat
//     REQUIRE(q.size() == 5);
//     REQUIRE(q.capacity() == 5);
//     REQUIRE(q.front() == "cat");
//     q.pop();
//     REQUIRE(q.front() == "dog");
//     q.pop();
//     REQUIRE(q.front() == "mat");
//     q.pop();
//     REQUIRE(q.front() == "hat");
//     q.pop();
//     REQUIRE(q.front() == "jam");
//     q.pop();
//     REQUIRE(q.size() == 0);
// }

// TEST_CASE("shuffle_push double", "[shuffleQueue]") {
//     ShuffleQueue<string> q(5, 2, 2);
//     q.push("dog");
//     q.push("hat");
//     q.push("mat");
//     // order: dog, hat, mat
//     REQUIRE(q.size() == 3);
//     q.shuffle_push("jam");
//     // order: dog, mat, hat, jam
//     REQUIRE(q.back() == "jam");
//     REQUIRE(q.size() == 4);

//     q.shuffle_push("fog");
//     // order: dog, jam, mat, hat, fog

//     REQUIRE(q.capacity() == 5);
//     REQUIRE(q.size() == 5);
//     REQUIRE(q.front() == "dog");
//     REQUIRE(q.back() == "fog");
//     q.pop();
//     REQUIRE(q.front() == "jam");
//     q.pop();
//     REQUIRE(q.front() == "mat");
//     q.pop();
//     REQUIRE(q.front() == "hat");
//     q.pop();
//     REQUIRE(q.front() == "fog");
//     q.pop();
//     REQUIRE(q.size() == 0);
// }

// TEST_CASE("shuffle_push resize", "[shuffleQueue]") {
//     ShuffleQueue<string> q(5, 2, 2);
//     q.push("cat");
//     q.push("dog");
//     q.push("hat");
//     q.push("mat");
//     // order: cat, dog, hat, mat
//     REQUIRE(q.size() == 4);
//     q.shuffle_push("jam");
//     // order: cat, dog, mat, hat, jam
//     REQUIRE(q.back() == "jam");
//     REQUIRE(q.front() == "cat");
//     REQUIRE(q.size() == 5);
//     REQUIRE(q.capacity() == 5);

//     q.shuffle_push("fog");
//     // order: jam, dog, mat, hat, cat, fog
//     REQUIRE(q.size() == 6);
//     REQUIRE(q.capacity() == 10);
//     REQUIRE(q.back() == "fog");
//     REQUIRE(q.front() == "jam");
//     q.pop();
//     REQUIRE(q.front() == "dog");
//     q.pop();
//     REQUIRE(q.front() == "mat");
//     q.pop();
//     REQUIRE(q.front() == "hat");
//     q.pop();
//     REQUIRE(q.front() == "cat");
//     q.pop();
//     REQUIRE(q.front() == "fog");
//     q.pop();
//     REQUIRE(q.size() == 0);
// }

// TEST_CASE("shuffle_push resize extended", "[shuffleQueue]") {

//     ShuffleQueue<string> q(5, 2, 2);
//     q.push("cat");
//     q.push("dog");
//     q.push("hat");
//     q.push("mat");
//     // order: cat, dog, hat, mat
//     REQUIRE(q.size() == 4);
//     q.shuffle_push("jam");
//     // ind before: 2
//     // order: cat, dog, mat, hat, jam
//     // ind after: 0
//     REQUIRE(q.back() == "jam");
//     REQUIRE(q.front() == "cat");
//     REQUIRE(q.size() == 5);
//     REQUIRE(q.capacity() == 5);

//     q.shuffle_push("fog");
//     // ind before: 0
//     // order: jam, dog, mat, hat, cat, fog
//     // ind after: 2 
//     // Already tested to be correct

//     REQUIRE(q.front() == "jam");
//     REQUIRE(q.back() == "fog");

//     q.shuffle_push("bat");
//     // ind before: 2
//     // order: jam, dog, fog, hat, cat, mat, bat
//     // ind after: 4

//     REQUIRE(q.back() == "bat");
//     REQUIRE(q.front() == "jam");

//     q.shuffle_push("sat");
//     // ind before: 4
//     // order: jam, dog, fog, hat, bat, mat, cat, sat
//     // ind after: 6

//     REQUIRE(q.size() == 8);
//     REQUIRE(q.capacity() == 10);
//     REQUIRE(q.back() == "sat");
//     REQUIRE(q.front() == "jam");
//     q.pop();
//     REQUIRE(q.front() == "dog");
//     q.pop();
//     REQUIRE(q.front() == "fog");
//     q.pop();
//     REQUIRE(q.front() == "hat");
//     q.pop();
//     REQUIRE(q.front() == "bat");
//     q.pop();
//     REQUIRE(q.front() == "mat");
//     q.pop();
//     REQUIRE(q.front() == "cat");
//     q.pop();
//     REQUIRE(q.front() == "sat");
//     q.pop();
//     REQUIRE(q.size() == 0);
// }

// TEST_CASE("shuffle_push popping", "[shuffleQueue]") {
//     ShuffleQueue<string> q(10, 2, 2);
//     // ind = 2
//     q.shuffle_push("cat");
//     // order: cat
//     // ind = 2
//     q.shuffle_push("dog");
//     // order: cat, dog
//     // ind = 0
//     q.shuffle_push("penny");
//     // order: dog, cat, penny
//     // ind = 2
//     REQUIRE(q.front() == "dog");
//     q.shuffle_push("rad");
//     // order: penny, cat, dog, rad
//     REQUIRE(q.front() == "penny");
//     REQUIRE(q.size() == 4);
//     REQUIRE(q.back() == "rad");
//     q.pop();
//     REQUIRE(q.front() == "cat");
//     q.pop();
//     REQUIRE(q.front() == "dog");
//     q.pop();
//     REQUIRE(q.front() == "rad");
//     q.pop();
//     REQUIRE(q.size() == 0);
// }

// TEST_CASE("Shuffle Queue of ShuffleDatum", "[shuffleQueue]") {
//     ShuffleQueue<ShuffleDatum> q;
//     q.push(ShuffleDatum());
//     REQUIRE( q.size() == 1 );

//     ShuffleQueue<ShuffleDatum> q1 = q;

//     REQUIRE(&q.front() != &q1.front());

//     ShuffleQueue<ShuffleDatum> q2;
//     ShuffleQueue<ShuffleDatum> q3 = q2;
// }
