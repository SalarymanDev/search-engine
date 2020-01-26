  #include "src/libs/frontier/shuffleQueue/shuffleQueue.h"

  using BBG::ShuffleQueue;
  using BBG::URL;
  using BBG::string;
  
// ShuffleQueue impl

ShuffleQueue::ShuffleQueue() 
    : queue(), ind(0), inc(0) {}

ShuffleQueue::ShuffleQueue(size_t _size, size_t _ind, size_t _inc)
    : queue(_size), ind(_ind), inc(_inc) {}

ShuffleQueue::ShuffleQueue(const ShuffleQueue& other) : 
    queue(other), ind(other.ind), inc(other.inc) {}

ShuffleQueue& ShuffleQueue::operator=(const ShuffleQueue& other) {
    if (this != &other) {
        ind = other.ind;
        inc = other.inc;
        _front = other._front;
        _back = other._back;
        _full = other._full;
        if (_capacity < other._capacity) {
            if (data)
               delete[] data;
            _capacity = other._capacity;
            data = new URL[_capacity];
         }
        for (size_t i = 0; i < other._capacity; ++i)
            data[i] = other.data[i];
    }
    return *this;
}

void ShuffleQueue::shuffle_push(const URL& val) {
    // Can't evaluate n mod 0, so if val is pushed 
    // onto an empty queue, then don't shuffle.

    size_t _size = this->size();
    if (_size) {
        size_t tmp_ind = this->_front + ind;

        // Need to apply mod incase many pops happen in between calls
        // to shuffle_push. Otherwise ind could have been large and
        // then many calls to pop are made leading to the condition
        // _size < tmp_ind resulting in a seg fault.
        swap(this->back(), this->data[tmp_ind % _size]);

        // _front starts at 0 so this equation always holds
        ind = (tmp_ind + inc) % _size;
    }
    this->push(val);
    
}
