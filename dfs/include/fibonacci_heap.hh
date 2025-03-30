#pragma once

#include <cstddef>
#include <shared_mutex>

namespace fsn::ds {
/**
 * @struct FibonacciHeapNode
 * @brief Represents a node in the Fibonacci Heap (for internal use only) used for storage allocation.
 *
 * Each node contains raw pointers to parent, left, right siblings, child node, bool marked and size_t key.
 */
struct FibonacciHeapNode {
  FibonacciHeapNode* parent;
  size_t key;
  FibonacciHeapNode* leftSib;
  FibonacciHeapNode* rightSib;
  bool marked;
  FibonacciHeapNode* child;
  int degree;

  FibonacciHeapNode(size_t k): key(k), parent(nullptr), leftSib(this), rightSib(this), marked(false), child(nullptr), degree(0) {}
};

/**
 * @class FibonacciHeap
 * @brief Implements the fibonacci heap that is for internal use only for the dsn to allocate storage nodes.
 */
class FibonacciHeap {
public:
  FibonacciHeap(): m_minNode(nullptr), m_numNodes(0) {}
  FibonacciHeapNode* insert(size_t k);
  FibonacciHeapNode* extract_min();
  void merge(FibonacciHeap& otherHeap);
  void decreaseKey(FibonacciHeapNode* node, size_t newKey);
  void deleteNode(FibonacciHeapNode* node);

private:
  mutable std::shared_mutex m_rwLock;
  FibonacciHeapNode* m_minNode;
  int m_numNodes;
  // This constructor is for internal use, only used to create temporary FH
  FibonacciHeap(FibonacciHeapNode* minPtr): m_minNode(minPtr), m_numNodes(0) {}
  FibonacciHeapNode* m_identifyMin(FibonacciHeap& fibHeap, FibonacciHeapNode* start, bool unparentRootList=false);
  void m_consolidate();
};

}
