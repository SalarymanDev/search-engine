#ifndef QUEUE_H
#define QUEUE_H

#include <cstddef> // size_t
#include <cassert>


namespace BBG { 

   template <typename T>
   class queue {
   protected:
      // Child classes have access
      size_t _front, _back, _capacity;
      bool _full;
      T *data;

   private:
      void _resize();

   public:
      // ctor
      queue();
      queue(size_t in_capacity);
      queue(const queue<T> &other);
      queue<T>& operator=(const queue<T>& other);
      ~queue();

      // Capacity
      size_t capacity() const;
      size_t size() const;
      bool empty() const;
      bool full() const;

      // Element access
      T& front();
      const T& front() const;
      T& back();
      const T& back() const;

      // Modifiers
      void push(const T& val);
      void pop();
   };

   template <typename T>
   queue<T>::queue() 
      : _front(0), _back(0), _capacity(1),
        _full(false), data(new T[1]) {}

   template <typename T>
   queue<T>::queue(size_t in_capacity) 
      : _front(0), _back(0), _capacity(in_capacity),
        _full(false), data(new T[in_capacity]) {
           assert(in_capacity);
        }

   template <typename T>
   queue<T>::queue(const queue<T> &other)
      : _front(other._front), _back(other._back), 
        _capacity(other._capacity), _full(other._full),
        data(new T[other._capacity]) {
      assert(other._capacity);
      for (size_t i = 0; i < _capacity; ++i)
         data[i] = other.data[i];
   }

   template <typename T>
   queue<T>& queue<T>::operator=(const queue<T>& other) {
      if (this != &other) {
         if (_capacity < other._capacity) {
            if (data)
               delete[] data;
            _capacity = other._capacity;
            data = new T[_capacity];
         }
         _front = other._front;
         _back = other._back;
         _full = other._full;

         for (size_t i = 0; i < _capacity; ++i)
            data[i] = other.data[i];
      }
      return *this;
   }

   template <typename T>
   queue<T>::~queue() {
      if (data)
         delete[] data;
      data = nullptr;
   }

   template <typename T>
   size_t queue<T>::capacity() const {
      return _capacity;
   }

   template <typename T>
   size_t queue<T>::size() const {
      if (_full)
         return _capacity;
      if (_back >= _front)
         return _back - _front;
      // capacity - (_front - _back)
      return _capacity - _front + _back;
   }

   template <typename T>
   bool queue<T>::empty() const {
      return size() == 0;
   }

   template <typename T>
   bool queue<T>::full() const {
      return _full;
   }

   template <typename T>
   T& queue<T>::front() {
      assert(size());
      return data[_front];
   }

   template <typename T>
   const T& queue<T>::front() const {
      assert(size());
      return data[_front];
   }

   template <typename T>
   T& queue<T>::back() {
      assert(size());
      if (_back)
         return data[_back - 1];
      return data[_capacity - 1];
   }

   template <typename T>
   const T& queue<T>::back() const{
      assert(size());
      if (_back)
         return data[_back - 1];
      return data[_capacity - 1];
   }

   template <typename T>
   void queue<T>::push(const T& val) {

      // _full is set to true in the ctor for _capacity == 0
      if (_full)      
         _resize();

      // If _back is at end of memory, then reset to 0
      size_t new_back = (_back + 1) % _capacity;
      
      // Queue is full after this push

      if (new_back == _front)
         _full = true;

      data[_back] = val;
      
      _back = new_back;
   }

   template <typename T>
   void queue<T>::pop() {
      assert(!empty());
      if (_full)
         _full = false;

      // If _front is at end of memory, then reset to 0
      size_t new_front = (_front + 1) % _capacity;
      if (new_front == _back)
         _front = _back = 0;
      else
         _front = new_front;
   }

   template <typename T>
   void queue<T>::_resize() {
      // Handles _capacity == 0, doubles memory
      size_t new_capacity = _capacity ? 2 * _capacity : 1; 
      T *temp = new T[new_capacity];

      // Copy from _front to end of queue
      for (size_t i = _front; i < _capacity; ++i)
         temp[i - _front] = data[i];

      // Copy from begining of queue up to _front
      for (size_t i = 0; i < _front; ++i)
         temp[i + _front - 1] = data[i];

      // Set _back to start of new memory
      _front = 0;
      _back = _capacity;
      _full = false;
      _capacity = new_capacity;

      if (data)
         delete[] data;
      data = temp;
   }

} // BBG

#endif
