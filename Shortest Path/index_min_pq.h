// written by nsrazdan
#ifndef INDEX_MIN_PQ_H_
#define INDEX_MIN_PQ_H_

#include <algorithm>
#include <sstream>
#include <utility>
#include <vector>

template <typename K>
class IndexMinPQ {
 public:
  // Constructor with max number of indexes
  explicit IndexMinPQ(unsigned int capacity);
  // Return number of items
  unsigned int Size();
  // Return top (ie index associated to minimum key)
  unsigned int Top();
  // Remove top
  void Pop();
  // Associates @key with index @idx
  void Push(const K &key, unsigned int idx);
  // Return whether @idx is a valid index
  bool Contains(unsigned int idx);
  // Change key associated to index @idx
  void ChangeKey(const K &key, unsigned int idx);

 private:
  // Private members
  unsigned int capacity;
  unsigned int cur_size;
  std::vector<K> keys;
  std::vector<unsigned int> heap_to_idx;
  std::vector<unsigned int> idx_to_heap;

  // Helper methods for indices
  unsigned int Root() {
    return 1;
  }
  unsigned int Parent(unsigned int i) {
    return i / 2;
  }
  unsigned int LeftChild(unsigned int i) {
    return 2 * i;
  }
  unsigned int RightChild(unsigned int i) {
    return 2 * i + 1;
  }

  // Helper methods for node testing
  bool HasParent(unsigned int i) {
    return i != Root();
  }
  bool IsNode(unsigned int i) {
    return i <= cur_size;
  }
  bool GreaterNode(unsigned int i, unsigned int j) {
    // Return true if node at index i is greater than node at index j, false
    // otherwise
    return (keys[heap_to_idx[i]] > keys[heap_to_idx[j]]);
  }

  // Helper methods for restructuring
  void SwapNodes(unsigned int i, unsigned int j) {
    // Swap nodes in heap
    std::swap(heap_to_idx[i], heap_to_idx[j]);
    // Update inverse mappings
    idx_to_heap[heap_to_idx[i]] = i;
    idx_to_heap[heap_to_idx[j]] = j;
  }
  void PercolateUp(unsigned int i);
  void PercolateDown(unsigned int i);

  // Helper method to check heap-order (useful for debugging)
  void CheckHeapOrder(unsigned int i) {
    if (!IsNode(i))
      return;
    if (HasParent(i) && GreaterNode(Parent(i), i)) {
      std::stringstream ss;
      ss << "Heap order error: "
         << "Parent ("
         << Parent(i) << ": " << heap_to_idx[Parent(i)] << ", "
         << keys[heap_to_idx[Parent(i)]] << ")"
         << " bigger than Child ("
         << i << ": " << heap_to_idx[i] << ", "
         << keys[heap_to_idx[i]] << ")";
      throw std::runtime_error(ss.str());
    }
    CheckHeapOrder(LeftChild(i));
    CheckHeapOrder(RightChild(i));
  }
};

template <typename K>
IndexMinPQ<K>::IndexMinPQ(unsigned int capacity)
    : capacity(capacity),
      keys(capacity),
      heap_to_idx(capacity + 1),
      idx_to_heap(capacity, 0) {
  cur_size = 0;
}

template <typename K>
unsigned int IndexMinPQ<K>::Size() {
  return cur_size;
}

template <typename K>
unsigned int IndexMinPQ<K>::Top() {
  if (!Size())
    throw std::underflow_error("Priority queue underflow!");

  return heap_to_idx[1];
}

template <typename K>
void IndexMinPQ<K>::PercolateUp(unsigned int i) {
  while (HasParent(i) && GreaterNode(Parent(i), i)) {
    SwapNodes(Parent(i), i);
    i = Parent(i);
  }
}

template <typename K>
void IndexMinPQ<K>::Push(const K &key, unsigned int idx) {
  if (idx >= capacity)
    throw std::overflow_error("Index invalid!");
  if (Contains(idx))
    throw std::runtime_error("Index already exists!");

  // Inc current size
  cur_size++;
  // Add node to end of heap
  heap_to_idx[cur_size] = idx;
  // Add node to end of heap in index map
  idx_to_heap[idx] = cur_size;
  // Add key value to keys vector
  keys[idx] = key;
  // Percolate up and fix errors
  PercolateUp(cur_size);
}

template <typename K>
void IndexMinPQ<K>::PercolateDown(unsigned int i) {
  // While node has at least one child (if one, necessarily on the left)
  while (IsNode(LeftChild(i))) {
    // Find smallest children between left and right if any
    unsigned int child = LeftChild(i);
    if (IsNode(RightChild(i)) && GreaterNode(LeftChild(i), RightChild(i)))
      child = RightChild(i);

    // Exchange node with child to restore heap-order if necessary
    if (GreaterNode(i, child))
      SwapNodes(i, child);
    else
      break;

    // Do it again, one level down
    i = child;
  }
}

template <typename K>
void IndexMinPQ<K>::Pop() {
  if (!Size())
    throw std::underflow_error("Empty priority queue!");

  // Swap first and last nodes
  SwapNodes(1, cur_size);
  // Decrease size by 1
  cur_size--;
  // PercolateDown starting from root node
  PercolateDown(1);
  // Mark the old idx_to_heap value as invalid
  idx_to_heap[cur_size + 1] = 0;
}

template <typename K>
bool IndexMinPQ<K>::Contains(unsigned int idx) {
  if (idx >= capacity)
    throw std::overflow_error("Index invalid!");
  return (idx_to_heap[idx] != 0);
}

template <typename K>
void IndexMinPQ<K>::ChangeKey(const K &key, unsigned int idx) {
  if (idx >= capacity)
    throw std::overflow_error("Index invalid!");
  if (!Contains(idx))
    throw std::runtime_error("Index does not exist!");

  // Update key in key vector
  keys[idx] = key;
  // Restore heap order. Need to perform both percolate down and percolate up
  // because we don't know whether the key has increased or decreased
  PercolateDown(idx_to_heap[idx]);
  PercolateUp(idx_to_heap[idx]);
}

#endif  // INDEX_MIN_PQ_H_
