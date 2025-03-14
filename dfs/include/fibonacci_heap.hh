#pragma once
#include <cstddef>
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

  FibonacciHeapNode(size_t k): key(k), parent(nullptr), leftSib(this), rightSib(this), marked(false), child(nullptr) {}
};

/**
 * @class FibonacciHeap
 * @brief Implements the fibonacci heap that is for internal use only for the dsn to allocate storage nodes.
 */
class FibonacciHeap {
public:
  FibonacciHeap(): m_minNode(nullptr), m_numNodes(0) {}
  FibonacciHeapNode* insert(size_t k);
  size_t extract_min();
  void merge(FibonacciHeap& otherHeap);
  void decreaseKey(FibonacciHeapNode* node, size_t newKey);
  void deleteNode(FibonacciHeapNode* node);

private:
  FibonacciHeapNode* m_minNode;
  int m_numNodes;
};

}
