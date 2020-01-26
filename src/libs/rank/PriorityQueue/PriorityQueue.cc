#include "src/libs/rank/PriorityQueue/PriorityQueue.h"
#include "src/libs/utils/utils.h"

using BBG::PriorityQueue;
using BBG::QueryResult;
using BBG::QueuePair;
using BBG::swap;

QueryResult::QueryResult() {}

QueryResult::QueryResult(const QueryResult& other)
    : url(other.url), title(other.title) {}	

QueryResult& QueryResult::operator=(const QueryResult& other) {
    if (this != &other) {
        url = other.url;
        title = other.title;
    }
    return *this;
}

QueuePair::QueuePair() : score(0.0) {}

QueuePair::QueuePair(const QueryResult &result, const float score)
    : score(score), result(result) {}

QueuePair::QueuePair(const QueuePair &other)
    : score(other.score), result(other.result) {}

QueuePair& QueuePair::operator=(const QueuePair &other) {
    if (this != &other) {
        score = other.score;
        result = other.result;
    }
    return *this;
}

void BBG::swap(QueryResult &a, QueryResult &b) {
    swap(a.url, b.url);
    swap(a.title, b.title);
}

void BBG::swap(QueuePair &a, QueuePair &b) {
    swap(a.score, b.score);
    swap(a.result, b.result);
}

// return parent of ith array element
size_t PriorityQueue::get_parent( size_t i ) {
    if (i == 0)
        return i;
    return (i - 1) / 2; 
}
// return left child of ith array element
size_t PriorityQueue::get_left( size_t i ) { 
    return ( 2 * i + 1 ); 
}
// return right child ith array element
size_t PriorityQueue::get_right( size_t i ) {
    return ( 2 * i + 2 ); 
}

size_t PriorityQueue::get_sibling(size_t i) {
    if (i == 0)
        return i;
    size_t parentIndex = get_parent(i);
    size_t leftIndex = get_left(parentIndex);
    size_t rightIndex = get_right(parentIndex);
    return i == leftIndex ? rightIndex : leftIndex;
}

// fixes node at index i and its two direct children
// if they violates the heap property
void PriorityQueue::fix_down( size_t i ) {
    size_t size = data.size();
    // get left and right child of node at index i
    size_t left = get_left( i );
    size_t right = get_right( i );

    if (left >= size)
        return;

    size_t minIndex = i; 

    // If left and right are within heap range, then compare 
    // ith element with both children and take index of largest
    if (data[left].score < data[i].score)
        minIndex = left;

    if ((right < size) && (data[right].score < data[minIndex].score))
        minIndex = right; 

    // if needed, swap larger child then continue fix-down on the child
    if ( minIndex != i ) {
        swap(data[i], data[minIndex]);
        fix_down( minIndex ); 
    }
}

void PriorityQueue::fix_up( size_t i ) {
    if (i == 0)
        return;
    // check if node at index i and its parent violates 
    // the heap property. If so swap the two, then check new parent
    size_t parent = get_parent( i ); 
    if ( data[ parent ].score > data[ i ].score ) {
        swap(data[parent], data[i]);
        fix_up(parent); 
    }
}

size_t PriorityQueue::findMax() {
    size_t index = data.size() / 2;
    float max = data[index].score;

    for (size_t i = 1 + index; i < data.size(); ++i) {
        if (max < data[i].score) {
            max = data[i].score;
            index = i;
        }
    }

    return index;
}

PriorityQueue::PriorityQueue() {
    data.reserve(10);
}

PriorityQueue::PriorityQueue( size_t _capacity ) {
    data.reserve(_capacity);
}

void PriorityQueue::push(QueryResult& result, float score) {
    QueuePair p(result, score);
    push(p);
}

void PriorityQueue::push( QueuePair& p ) {
    // if incoming value is smaller than all others, no point adding it
    if (data.size() < data.capacity()) {
        data.push_back(p);
        fix_up(data.size() - 1);
        return;
    }

    if ( p.score < data[ 0 ].score )
        return;

    swap(data[0], p);
    fix_down(0);
}

// removes and returns root element
QueuePair PriorityQueue::pop( ) {
    // save root element
    QueuePair top = data[ 0 ];
    // replace the root of the heap with the last element
    data[ 0 ] = data[ data.size() - 1 ];
    data.pop_back();
    // fix new root node
    fix_down( 0 );
    return top;
}

QueryResult PriorityQueue::popBottom() {
    // Not calling fix_up because we are removing the last element.
    size_t index = findMax();
    size_t siblingIndex = get_sibling(index);
    QueuePair bottom = data[index];

    // siblingIndex may be equal to size if there is no sibling.
    if (index < siblingIndex && siblingIndex < data.size())
        swap(data[index], data[siblingIndex]);

    data.pop_back();
    return bottom.result;
}

QueuePair& PriorityQueue::top() {
    return data[ 0 ];
}

float& PriorityQueue::topScore() {
    return bottom().score;
}

QueuePair& PriorityQueue::bottom() {
    size_t max = findMax();
    return data[max];
}

bool PriorityQueue::empty() {
    return data.empty();
}
