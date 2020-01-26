#include "src/libs/rank/PriorityQueue/PriorityQueue.h"
#include <iostream>
#include "tests/catch.hpp"

using BBG::PriorityQueue;
using BBG::QueryResult;
using BBG::QueuePair;
using BBG::vector;
using std::cout;
using std::endl;

TEST_CASE("PriorityQueue Single Element Pop Bottom", "[PriorityQueue]") {
    PriorityQueue test(5);
    QueuePair one(QueryResult(), 1.0);
    test.push(one);

    REQUIRE(test.bottom().score == Approx(1.0));
}

TEST_CASE("PriorityQueue Test", "[PriorityQueue]") {
    PriorityQueue  test(6);
    QueuePair one(QueryResult(), 1.0);
    QueuePair two(QueryResult(), 2.0);
    QueuePair three(QueryResult(), 3.0);
    QueuePair four(QueryResult(), 4.0);
    QueuePair five(QueryResult(), 5.0);
    QueuePair six(QueryResult(), 6.0);
    test.push(four);
    test.push(two);
    test.push(five);
    test.push(one);
    test.push(three);
    test.push(six);

    REQUIRE(test.bottom().score == Approx(6.0));
    test.popBottom();

    cout << "PriorityQueue" << endl;
    vector<QueuePair> results(5);
    for (int i = 0; i < results.size(); ++i)
        results[i] = test.pop();

    bool isSameOrder = true;
    for (int i = 0; i < results.size(); ++i) {
        cout << "Expected: " << (float)(i + 1) << " Actual: " << results[i].score << endl;
        if (results[i].score != Approx((float)(i + 1))) isSameOrder = false;
    }

    REQUIRE(isSameOrder);
}
