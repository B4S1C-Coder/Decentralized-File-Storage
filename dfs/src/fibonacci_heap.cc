#include "fibonacci_heap.hh"
#include <cstddef>

fsn::ds::FibonacciHeapNode* fsn::ds::FibonacciHeap::insert(size_t k) {
  fsn::ds::FibonacciHeapNode* node = new fsn::ds::FibonacciHeapNode(k);
  
  if (m_minNode == nullptr) {
    m_minNode = node;
  } else {
    // I would be adding new nodes in the root list to the right of the minimum node
    node->rightSib = m_minNode->rightSib;
    m_minNode->rightSib->leftSib = node;
    node->leftSib = m_minNode;
    m_minNode->rightSib = node;

    if (node->key < m_minNode->key) {
      m_minNode = node;
    }
  }

  ++m_numNodes;
  return node;
}
