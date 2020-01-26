#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef> // size_t
#include <cassert>
#include "src/libs/utils/utils.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/serialize/Buffer.h"
#include "iostream"

namespace BBG {

   template <typename T>
   class vector {
   private:
      size_t _size, _capacity;
      T *_data;

      void _resize(size_t newCapacity);

   public:
      // Default ctor
      vector();

      // ctor that allocates size and initializes starting values to val
      vector(size_t size, const T& val = T());
      // destructor
      ~vector();
      // copy constructor
      vector(const vector<T>& other);
      // move constructor
      vector(vector<T>&& other);
      // Buffer constructor
      vector(const T*, size_t size);
      // move assignment operator
      vector<T> operator=(vector<T>&& other);

      // assignment operator
      vector<T>& operator=(const vector<T>& other);

      // Append other vector operator
      vector<T>& operator+= ( const vector<T>& other );

      // Capacity
      size_t size() const;
      void resize(size_t newSize, const T& val = T());
      size_t capacity() const;

      void reserve(size_t newCapacity);

      bool empty() const;

      // Element access
      T& operator[](size_t index);
      const T& operator[](size_t index) const;
      T& front();
      const T& front() const;
      T& back();
      const T& back() const;
      T* begin();
      const T* begin() const;
      T* end();
      const T* end() const;

      // Modifiers
      void push_back(const T& val);
      template <typename... Types>
      void emplace_back(Types... types);
      void pop_back();

      // Removes all elements from the vector (which are destroyed), 
      // leaving the container with a size of 0.
      void clear();

      bool operator==(const vector<T>& other) const;
      bool operator!=(const vector<T>& other) const;

      template <typename U>
      friend void swap(vector<U>& a, vector<U>& b);
      template <typename U>
      friend void serialize(const vector<U>& vec, Buffer& bytes);
      template <typename U>
      friend void deserialize(Buffer& bytes, vector<U>& vec);
   };

   template <typename T>
   vector<T>::vector()
      : _size(0), _capacity(1), _data(new T[1]) {}


   template <typename T>
   vector<T>::vector(size_t size, const T& val)
      : _size(size), _capacity(size ? size : 1), _data(new T[size ? size : 1]) {
         for (size_t i = 0; i < size; ++i)
         _data[i] = val;
   }

   template <typename T>
   vector<T>::~vector() {
      if (_data)
         delete[] _data;
      _data = nullptr;
   }

   template <typename T>
   vector<T>::vector(const vector<T>& other)
      : _size(other._size), _capacity(other._capacity),
        _data(new T[other._capacity]) {
      assert(other._capacity > 0);
      for (size_t i = 0; i < _size; ++i)
         _data[i] = other[i];
   }

   template <typename T>
   vector<T>::vector(vector<T>&& other)
      : _size(other._size), 
         _capacity(other._capacity), 
         _data(other._data) {
      assert(other._capacity > 0);
      other._size = other._capacity = 0;
      other._data = nullptr;
   }

   template <typename T>
   vector<T>::vector(const T* buff, size_t size)
      : _size(size), _capacity(size), _data(new T[size]) {
         for (size_t i = 0; i < size; ++i)
            _data[i] = buff[i];
   }

   template <typename T>
   vector<T> vector<T>::operator=(vector<T>&& other) { 
      assert(other._capacity > 0);
      _size = other._size;
      _capacity = other._capacity;
      if (_data)
         delete[] _data;
      _data = other._data;
      other._size = other._capacity = 0;
      other._data = nullptr;
      return *this;
   }

   template <typename T>
   vector<T>& vector<T>::operator=(const vector<T>& other) {
      if (this != &other) {
         if (_capacity < other._capacity) {
            if (_data)
               delete[] _data;
            _capacity = other._capacity;
            _data = new T[_capacity];
         }
         _size = other._size;

         for (size_t i = 0; i < _size; ++i)
            _data[i] = other[i];
      }
      return *this;
   }

   template <typename T>
   size_t vector<T>::size() const {
      return _size;
   }

   template <typename T>
   void vector<T>::_resize(size_t newCapacity) {
      if (_capacity < newCapacity) {
         _capacity = newCapacity;
         T *temp = new T[_capacity];

         size_t copyUpToIndex = _capacity < _size ? _capacity : _size;

         for (size_t i = 0; i < copyUpToIndex; ++i)
            temp[i] = _data[i];

         if (_data)
            delete[] _data;
         _data = temp;
      }
   }

   template <typename T>
   void vector<T>::resize(size_t newSize, const T& val) {
      if (_size < newSize) {
         _resize(newSize);

         // Copy val to the new elements
         for (size_t i = _size; i < newSize; ++i)
            _data[i] = val;

         _size = newSize;
      }
   }

   template <typename T>
   vector<T>& vector<T>::operator+= ( const vector<T>& other ) {
      reserve(_capacity + other._capacity);
      for (size_t i = 0; i < other.size(); ++i)
         _data[_size++] = other._data[i];
      return *this;
   }

   template <typename T>
   size_t vector<T>::capacity() const {
      return _capacity;
   }

   template <typename T>
   void vector<T>::reserve(size_t newCapacity) {
      if (newCapacity > _capacity)
         _resize(newCapacity);

   }

   template <typename T>
   bool vector<T>::empty() const {
      return _size == 0;
   }

   template <typename T>
   T& vector<T>::operator[](size_t i) {
      assert(i < _size);
      return _data[i];
   }

   template <typename T>
   const T& vector<T>::operator[](size_t i) const {
      assert(i < _size);
      return _data[i];
   }

   template <typename T>
   T& vector<T>::front() {
      assert(_size > 0);
      return _data[0];
   }

   template <typename T>
   const T& vector<T>::front() const {
      assert(_size > 0);
      return _data[0];
   }

   template <typename T>
   T& vector<T>::back() {
      assert(_size > 0);
      return _data[_size - 1];
   }

   template <typename T>
   const T& vector<T>::back() const{
      assert(_size > 0);
      return _data[_size - 1];
   }

   template <typename T>
   T* vector<T>::begin() {
      return _data;
   }

   template <typename T>
   const T* vector<T>::begin() const {
      return _data;
   }

   template <typename T>
   T* vector<T>::end() {
      return _data + _size;
   }

   template <typename T>
   const T* vector<T>::end() const{
      return _data + _size;
   }

   template <typename T>
   void vector<T>::push_back(const T& val) {
      if (_size == _capacity)
         _resize(_capacity ? _capacity * 2 : 1);
      _data[_size++] = val;
   }

   template <typename T>
   template <typename... Types>
   void vector<T>::emplace_back(Types... types) {
      if (_size == _capacity)
         _resize(_capacity ? _capacity * 2 : 1);
      _data[_size++] = T(types...);
   }

   template <typename T>
   void vector<T>::pop_back() {
      if (_size > 0)
         --_size;
   }

   template <typename T>
   void vector<T>::clear() {
      _size = 0;
   }

   template <typename T>
   bool vector<T>::operator==(const vector<T>& other) const {
      if (other._size != _size)
         return false;
      for (size_t i = 0; i < _size; ++i)
         if (_data[i] != other[i])
            return false;
      return true;
   }

   template <typename T>
   bool vector<T>::operator!=(const vector<T>& other) const {
      return !(*this == other);  
   }

   template <typename T>
   void swap(vector<T>& a, vector<T>& b) {
      BBG::swap(a._size, b._size);
      BBG::swap(a._capacity, b._capacity);
      BBG::swap(a._data, b._data);
   }

   template <typename T>
   void serialize(const vector<T>& vec, Buffer& bytes) {
      serialize(vec.size(), bytes);
      for (size_t i = 0; i < vec.size(); ++i)
         serialize(vec[i], bytes);
   }

   template <typename T>
   void deserialize(Buffer& bytes, vector<T>& vec) {
      size_t size;
      deserialize(bytes, size);
      vec = vector<T>(size);
      for (size_t i = 0; i < size; ++i)
         deserialize(bytes, vec[i]);
   }

} // BBG

#endif
