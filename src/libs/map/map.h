#ifndef MAP_H
#define MAP_H

#include <cstdint>
#include <iostream>
#include "src/libs/fnv/fnv.h"
#include "src/libs/vector/vector.h"
#include "src/libs/utils/utils.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/logger/logger.h"

#define MAX_LOAD_FACTOR 0.7
#define HASH_TOMBSTONE_MASK 0x8000000000000000

namespace BBG {

  template <typename Key, typename Val>
  struct MapEntry {
    uint32_t dist = 0;
    uint64_t hash = 0;
    Key key;
    Val val;

    bool operator==( const MapEntry<Key, Val>& other ) const;
    bool operator!=( const MapEntry<Key, Val>& other ) const;
  };

  template <typename Key, typename Val>
  class Map {
    private:
      uint32_t _maxDist, _size, _iteratorPos;
      BBG::vector<MapEntry<Key, Val>> _data;

      uint32_t insertHelper(const Key& key, const Val& val);
      void rehashAndGrow(uint32_t size);
      uint64_t getHash(const Key& key) const;
      uint32_t getPos(const uint64_t& hash) const;

    public:
      // Constructors
      Map();
      Map(uint32_t capacity);

      // Capacity
      uint32_t size() const;
      uint32_t capacity() const;
      bool empty() const;

      // Element access
      void clear();
      void erase(const Key& key);
      void insert(const Key& key, const Val& val);
      bool contains(const Key& key) const;
      const Val* get(const Key& key) const;
      Val& operator[](const Key& key);

      bool operator==( const Map<Key, Val>& other ) const;
      bool operator!=( const Map<Key, Val>& other ) const;

      // Iterator capability
      MapEntry<Key, Val>* begin();
      MapEntry<Key, Val>* next();

      template <typename K, typename V>
      friend void serialize(const Map<K, V>& map, Buffer& bytes);
      template <typename K, typename V>
      friend void deserialize(Buffer& bytes, Map<K, V>& map);
      template <typename K, typename V>
      friend void serialize(const Map<K, V*>& map, Buffer& bytes);
      template <typename K, typename V>
      friend void deserialize(Buffer& bytes, Map<K, V*>& map);

  };

// PUBLIC FUNCTIONS

template <typename Key, typename Val>
Map<Key, Val>::Map()
  : _maxDist(0), _size(0), _iteratorPos(0), _data(128) { }

template <typename Key, typename Val>
Map<Key, Val>::Map(uint32_t capacity)
  : _maxDist(0), _size(0), _iteratorPos(0), _data(capacity) { }

template <typename Key, typename Val>
uint32_t Map<Key, Val>::size() const {
  return _size;
}

template <typename Key, typename Val>
uint32_t Map<Key, Val>::capacity() const {
  return (uint32_t) _data.size();
}

template <typename Key, typename Val>
bool Map<Key, Val>::empty() const {
  return _size == 0;
}

template <typename Key, typename Val>
uint64_t Map<Key, Val>::getHash(const Key& key) const {
  return fnv(&key, sizeof(key));
}

template <typename Key, typename Val>
uint32_t Map<Key, Val>::getPos(const uint64_t& hash) const {
  return (uint32_t) (hash % (uint64_t) _data.size());
}

template <typename Key, typename Val>
bool Map<Key, Val>::contains(const Key& key) const {
  return get(key);
}

template <typename Key, typename Val>
const Val* Map<Key, Val>::get(const Key& key) const {
  uint64_t hash = getHash(key);
  uint32_t pos = getPos(hash);
  hash |= HASH_TOMBSTONE_MASK; // set full bit for probing

  for (uint32_t i = 0; i <= _maxDist; ++i) {
    if (_data[pos].hash == hash && _data[pos].key == key)
      return &_data[pos].val;
    else if ((_data[pos].hash & HASH_TOMBSTONE_MASK) == 0)
      return nullptr;
    else
      pos = (pos + 1) % _data.size();
  }

  return nullptr;
}

template <typename Key, typename Val>
void Map<Key, Val>::clear() {
  _data.clear();
  _data.resize(_data.capacity(), MapEntry<Key, Val>());
  _size = 0;
  _maxDist = 0;
  _iteratorPos = 0;
}

template <typename Key, typename Val>
void Map<Key, Val>::erase(const Key& key) {
  uint64_t hash = getHash(key);
  uint32_t pos = getPos(hash);
  hash |= HASH_TOMBSTONE_MASK; // set full bit for probing

  for (uint32_t i = 0; i <= _maxDist && (_data[pos].hash & HASH_TOMBSTONE_MASK); ++i) {
    if (_data[pos].hash == hash && _data[pos].key == key) { // Found element
      // Delete current
      --_size;
      _data[pos].hash = 0;

      // Move up all elements after with dist > 0
      uint32_t next = (pos + 1) % _data.size();
      while ((_data[next].hash & HASH_TOMBSTONE_MASK) && _data[next].dist != 0) {
        _data[pos] = _data[next];
        --_data[pos].dist;
        _data[next].hash = 0;
        pos = (pos + 1) % _data.size();
        next = (next + 1) % _data.size();
      }
      return;
    }
    pos = (pos + 1) % _data.size();
  }

  // Couldn't find element, no change to map
}

template <typename Key, typename Val>
void Map<Key, Val>::insert(const Key& key, const Val& val) {
  insertHelper(key, val);
}

template <typename Key, typename Val>
Val& Map<Key, Val>::operator[](const Key& key) {
  uint32_t pos = insertHelper(key, Val());
  return _data[pos].val;
}

template <typename Key, typename Val>
void Map<Key, Val>::rehashAndGrow(uint32_t size) {
  BBG::vector<MapEntry<Key, Val>> data(size);
  swap(data, _data); // all keys are in our local map now
  _size = 0;
  _maxDist = 0;

  for (uint32_t i = 0; i < data.size(); ++i)
    if (data[i].hash & HASH_TOMBSTONE_MASK) // there is an element
      insertHelper(data[i].key, data[i].val);
}

// PRIVATE HELPER FUNCTIONS

template <typename Key, typename Val>
uint32_t Map<Key, Val>::insertHelper(const Key& key, const Val& val) {
  // Grow if needed
  if (((double) (_size + 1) / _data.size()) > MAX_LOAD_FACTOR)
    rehashAndGrow(_data.size() * 2);

  uint64_t hash = getHash(key);
  uint32_t pos = getPos(hash);
  hash |= HASH_TOMBSTONE_MASK; // set full bit for probing
  Key currentKey = key;
  Val currentVal = val;
  bool swapped = false;
  uint32_t finalPos;
  
  for (uint32_t dist = 0; ; ++dist) {
    // Element already exists
    if (_data[pos].hash == hash && _data[pos].key == key)
      return pos; // Item already exists

    // Found an empty slot
    if ((_data[pos].hash & HASH_TOMBSTONE_MASK) == 0) {
      _data[pos].key = currentKey;
      _data[pos].val = currentVal;
      _data[pos].hash = hash;
      _data[pos].dist = dist;
      ++_size;
      if (dist > _maxDist)
        _maxDist = dist;
      if (swapped) // items were swapped
        return finalPos;
      else // no items were swapped
        return pos;
    }

    // Steal from the rich, give to the poor
    if (_data[pos].dist < dist) {
      if (dist > _maxDist)
        _maxDist = dist;
      swap(currentKey, _data[pos].key);
      swap(currentVal, _data[pos].val);
      swap(hash, _data[pos].hash);
      swap(dist, _data[pos].dist);
      if (!swapped) { // Store the inserted value to return later
        swapped = true;
        finalPos = pos;
      }
    }

    pos = (pos + 1) % _data.size();
  }
}

template <typename Key, typename Val>
bool MapEntry<Key, Val>::operator==( const MapEntry<Key, Val>& other ) const {
  if (hash != other.hash) return false;
  if (key != other.key) return false;
  if (val != other.val) return false;
  return true;
}

template <typename Key, typename Val>
bool MapEntry<Key, Val>::operator!=( const MapEntry<Key, Val>& other ) const {
  return !(*this == other);
}

template <typename Key, typename Val>
bool Map<Key, Val>::operator==( const Map<Key, Val>& other ) const {
  if (size() != other.size())
    return false;

  for (size_t i = 0; i < _data.size(); ++i) {
    if ((_data[i].hash & HASH_TOMBSTONE_MASK) == 0)
      continue;

    if (!other.contains(_data[i].key))
      return false;
    
    if (_data[i].val != *other.get(_data[i].key))
      return false;
  }
  
  return true;
}

template <typename Key, typename Val>
bool Map<Key, Val>::operator!=( const Map<Key, Val>& other ) const {
  return !(*this == other);
}

// ITERATOR FUNCTIONS

template <typename Key, typename Val>
MapEntry<Key, Val>* Map<Key, Val>::begin() {
  _iteratorPos = 0;
  if (_data[_iteratorPos].hash & HASH_TOMBSTONE_MASK)
    return &_data[_iteratorPos];
  else
    return next();
}

template <typename Key, typename Val>
MapEntry<Key, Val>* Map<Key, Val>::next() {
  if (_iteratorPos >= (uint32_t) _data.size())
    return nullptr;

  while (++_iteratorPos < capacity() 
      && (_data[_iteratorPos].hash & HASH_TOMBSTONE_MASK) == 0) { }

  if (_iteratorPos >= (uint32_t) _data.size())
    return nullptr;
  else
    return &_data[_iteratorPos];
}

// Serialization Functions

template <typename K, typename V>
void serialize(const MapEntry<K, V>& entry, Buffer& bytes) {
  serialize(entry.dist, bytes);
  serialize(entry.hash, bytes);
  serialize(entry.key, bytes);
  serialize(entry.val, bytes);
}

template <typename K, typename V>
void deserialize(Buffer& bytes, MapEntry<K, V>& entry) {
  deserialize(bytes, entry.dist);
  deserialize(bytes, entry.hash);
  deserialize(bytes, entry.key);
  deserialize(bytes, entry.val);
}

template <typename K, typename V>
void serialize(const Map<K, V>& map, Buffer& bytes) {
  serialize(map._size, bytes);
  serialize(map._maxDist, bytes);
  serialize(map._data, bytes);
}

template <typename K, typename V>
void deserialize(Buffer& bytes, Map<K, V>& map) {
  deserialize(bytes, map._size);
  deserialize(bytes, map._maxDist);
  deserialize(bytes, map._data);
}

template <typename K, typename V>
void serialize(const Map<K, V*>& map, Buffer& bytes) {
  serialize(map._size, bytes);
  serialize(map._maxDist, bytes);
  serialize(map._data, bytes);
}

template <typename K, typename V>
void deserialize(Buffer& bytes, Map<K, V*>& map) {
  deserialize(bytes, map._size);
  deserialize(bytes, map._maxDist);
  deserialize(bytes, map._data);
}

template <typename K, typename V>
void serialize(const MapEntry<K, V*>& entry, Buffer& bytes) {
  serialize(entry.dist, bytes);
  serialize(entry.hash, bytes);
  serialize(entry.key, bytes);
  if (entry.val)
    serialize(*(entry.val), bytes);
}

template <typename K, typename V>
void deserialize(Buffer& bytes, MapEntry<K, V*>& entry) {
  deserialize(bytes, entry.dist);
  deserialize(bytes, entry.hash);
  deserialize(bytes, entry.key);
  V* value = entry.hash == 0 ? nullptr : new V();
  if (value) {
    deserialize(bytes, *(value));
    entry.val = value;
  }
}



}

#endif
