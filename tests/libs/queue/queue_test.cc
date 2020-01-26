#include "src/libs/queue/queue.h"
#include "src/libs/vector/vector.h"
#include "tests/catch.hpp"

using BBG::queue;
using BBG::vector;

TEST_CASE("Default constructor", "[queue]") {
	queue<int> q;
	REQUIRE(q.size() == 0);
	REQUIRE(q.empty());

	queue<queue<int>> qq;
	REQUIRE(qq.size() == 0);
	REQUIRE(qq.empty());
}

TEST_CASE("Size constructor", "[queue]") {
	queue<int> q(10);
	REQUIRE(q.size() == 0);
	REQUIRE(q.empty());

	queue<queue<int>> qq(10);
	REQUIRE(qq.size() == 0);
	REQUIRE(qq.empty());
}

TEST_CASE("size1", "[queue]") {
	queue<int> q(5);

	for (int i = 1; i < 6; ++i) {
		q.push(i);
		REQUIRE(i == q.size());
		REQUIRE(!q.empty());
	}
	for (int i = 4; i > 0; --i) {
		q.pop();
		REQUIRE(i == q.size());
		REQUIRE(!q.empty());
	}

	q.pop();
	REQUIRE(q.size() == 0);
	REQUIRE(q.empty());
}

TEST_CASE("size2", "[queue]") {
	queue<int> q(5);

	for (int i = 1; i < 6; ++i) {
		q.push(i);
		REQUIRE(i == q.size());
		REQUIRE(!q.empty());
	}
	q.pop();
	REQUIRE(q.size() == 4);
	REQUIRE(q.capacity() == 5);
	REQUIRE(!q.empty());
	q.pop();
	REQUIRE(q.size() == 3);
	REQUIRE(q.capacity() == 5);
	REQUIRE(!q.empty());
	q.pop();
	REQUIRE(q.size() == 2);
	REQUIRE(q.capacity() == 5);
	REQUIRE(!q.empty());
	q.push(2);
	REQUIRE(q.capacity() == 5);
	REQUIRE(!q.empty());
	REQUIRE(q.size() == 3);
	q.push(4);
	REQUIRE(q.size() == 4);
	REQUIRE(q.capacity() == 5);
	REQUIRE(!q.empty());
	q.push(5);
	REQUIRE(q.size() == 5);
	REQUIRE(q.capacity() == 5);
	REQUIRE(!q.empty());

	size_t q_size = q.size();
	REQUIRE(q_size == 5);
	for (int i = 1; i < 40; ++i) {
		q.push(i);
		REQUIRE(q_size + i == q.size());
		REQUIRE(!q.empty());
	}
}


TEST_CASE("back", "[queue]") {
	queue<int> q(5);

	for (int i = 1; i < 6; ++i) {
		q.push(i);
		REQUIRE(i == q.size());
		REQUIRE(!q.empty());
	}
	REQUIRE(q.full());
	q.pop();
	REQUIRE(q.back() == 5);
	q.pop();
	REQUIRE(q.back() == 5);
	q.pop();
	REQUIRE(q.back() == 5);
	q.push(2);
	REQUIRE(q.back() == 2);
	q.push(4);
	REQUIRE(q.back() == 4);
	q.push(7);
	REQUIRE(q.back() == 7);

	q.push(8);
	REQUIRE(q.capacity() == 10);
	REQUIRE(q.back() == 8);
}

TEST_CASE("front", "[queue]") {
	queue<int> q(5);

	for (int i = 1; i < 6; ++i) {
		q.push(i);
		REQUIRE(i == q.size());
		REQUIRE(!q.empty());
	}
	REQUIRE(q.front() == 1);
	q.pop();
	REQUIRE(q.front() == 2);
	q.pop();
	REQUIRE(q.front() == 3);
	q.pop();
	REQUIRE(q.front() == 4);
	q.push(2);
	REQUIRE(q.front() == 4);
	q.push(4);
	REQUIRE(q.front() == 4);
	q.push(7);
	REQUIRE(q.front() == 4);
	// Triggers resize
	q.push(8);
	REQUIRE(q.front() == 4);
	REQUIRE(q.back() == 8);
	REQUIRE(q.capacity() == 10);
	REQUIRE(q.size() == 6);

	q.pop();
	REQUIRE(q.front() == 5);
	REQUIRE(q.back() == 8);
	REQUIRE(q.capacity() == 10);
	REQUIRE(q.size() == 5);

	q.pop();
	REQUIRE(q.front() == 2);
	REQUIRE(q.back() == 8);
	REQUIRE(q.capacity() == 10);
	REQUIRE(q.size() == 4);

	q.pop();
	REQUIRE(q.front() == 4);
	REQUIRE(q.back() == 8);
	REQUIRE(q.capacity() == 10);
	REQUIRE(q.size() == 3);

	q.pop();
	REQUIRE(q.front() == 7);
	REQUIRE(q.back() == 8);
	REQUIRE(q.capacity() == 10);
	REQUIRE(q.size() == 2);

	q.pop();
	REQUIRE(q.front() == 8);
	REQUIRE(q.back() == 8);
	REQUIRE(q.capacity() == 10);
	REQUIRE(q.size() == 1);

	q.pop();
	REQUIRE(q.size() == 0);
	REQUIRE(q.empty());

	for (int i = 1; i < 10; ++i) {
		q.push(i);
		REQUIRE(i == q.size());
		REQUIRE(q.back() == q.size());
		REQUIRE(!q.empty());
		REQUIRE(q.capacity() == 10);
	}

	q.push(10);
	REQUIRE(q.front() == 1);
	REQUIRE(q.capacity() == 10);
	REQUIRE(q.size() == 10);
	q.push(11);
	REQUIRE(q.front() == 1);
	REQUIRE(q.capacity() == 20);
	REQUIRE(q.size() == 11);
	REQUIRE(q.back() == 11);
}


TEST_CASE("vector of queues", "[queue]") { 
	vector<queue<int>> vec_qs;
	vec_qs.emplace_back(2);

	REQUIRE(vec_qs.size() == 1);

	vec_qs[0].push(3);

	REQUIRE(vec_qs[0].size() == 1);
	REQUIRE(vec_qs[0].front() == 3);
}


TEST_CASE("vector of queues inner scope", "[queue]") { 
	vector<queue<int>> vec_qs;

	{
		queue<int> q;
		vec_qs.push_back(q);
	}
	vec_qs[0].push(3);

	REQUIRE(vec_qs[0].size() == 1);
	REQUIRE(vec_qs[0].front() == 3);
}


TEST_CASE("copy ctor queue", "[queue]") { 
	vector<queue<int>> vec_qs;

	{
		queue<int> q;
		vec_qs.push_back(q);
	}
	vec_qs[0].push(3);

	queue<int> copy_q(vec_qs[0]);
	REQUIRE(copy_q.size() == 1);
	REQUIRE(copy_q.front() == 3);
}


TEST_CASE("quese =op", "[queue]") { 
	queue<int> q1;
	q1.push(1);
	q1.push(5);

	queue<int> q2;
	q2.push(3);

	q2 = q1;

	REQUIRE(q2.front() == 1);
	REQUIRE(q2.back() == 5);
	REQUIRE(q2.size() == 2);
}
