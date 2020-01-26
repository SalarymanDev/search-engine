#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/logger/logger.h"

namespace BBG {

   struct QueryResult {
      string url;
      string title;

      QueryResult();
      QueryResult(const QueryResult& other);	
      QueryResult& operator=(const QueryResult& other);
   };

   struct QueuePair {
      float score;
      QueryResult result;

      QueuePair();
      QueuePair(const QueryResult &result, const float score);
      QueuePair(const QueuePair &other);
      QueuePair& operator=(const QueuePair &other);
   };

   void swap(QueryResult &a, QueryResult &b);
   void swap(QueuePair &a, QueuePair &b);

   // Data structure for Min Heap
   class PriorityQueue {
   private:
      // store heap elements
      vector<QueuePair> data;
      Logger log;

      // return parent of ith array element
      size_t get_parent( size_t i );
      // return left child of ith array element
      size_t get_left( size_t i );
      // return right child ith array element
      size_t get_right( size_t i );

      size_t get_sibling(size_t i);

      // fixes node at index i and its two direct children
      // if they violates the heap property
      void fix_down( size_t i );

      void fix_up( size_t i );

      size_t findMax();
      
   public:
      PriorityQueue();

      PriorityQueue( size_t _capacity );

      void push(QueryResult& result, float score);

      void push( QueuePair& p );

      // removes and returns root element
      QueuePair pop( );

      QueryResult popBottom();

      QueuePair& top();

      float& topScore();

      QueuePair& bottom();

      bool empty();
   };
}

#endif
