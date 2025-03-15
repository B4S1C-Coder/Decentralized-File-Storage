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

void fsn::ds::FibonacciHeap::merge(fsn::ds::FibonacciHeap& otherHeap) {
  if (m_minNode == nullptr) {
    m_minNode = otherHeap.m_minNode;
    m_numNodes += otherHeap.m_numNodes;
    otherHeap.m_minNode = nullptr;
    otherHeap.m_numNodes = 0;
    return;
  }

  if (otherHeap.m_minNode == nullptr) {
    return;
  }

  // Four boundary nodes: min nodes of each heap, and the left nodes of each of the mins
  FibonacciHeapNode* A = m_minNode; 
  FibonacciHeapNode* C = otherHeap.m_minNode;
  FibonacciHeapNode* B = A->leftSib;
  FibonacciHeapNode* D = C->leftSib;
  
  A->leftSib = D;
  D->rightSib = A;
  C->leftSib = B;
  B->rightSib = C;

  m_minNode = (m_minNode->key < otherHeap.m_minNode->key) ? m_minNode : otherHeap.m_minNode;
  m_numNodes += otherHeap.m_numNodes;
  otherHeap.m_minNode = nullptr;
  otherHeap.m_numNodes = 0;
}

fsn::ds::FibonacciHeapNode* fsn::ds::FibonacciHeap::m_identifyMin(
  fsn::ds::FibonacciHeap& fibHeap, fsn::ds::FibonacciHeapNode* start, bool unparentRootList
) {
  if (fibHeap.m_numNodes == 0 || start == nullptr) { return nullptr; }
  
  fsn::ds::FibonacciHeapNode* current = start;
  fsn::ds::FibonacciHeapNode* encounteredMin = start;

  while (current->rightSib != start) {
    if (current->key < encounteredMin->key) {
      encounteredMin = current;
    }
    current = current->rightSib;
    
    if (unparentRootList) {
      current->parent = nullptr;
    }
  }

  // Check for the last node
  if (current->key < encounteredMin->key) {
    encounteredMin = current;
  }
  if (unparentRootList) {
    current->parent = nullptr;
  }
  return encounteredMin;
}

fsn::ds::FibonacciHeapNode* fsn::ds::FibonacciHeap::extract_min() {
  if (m_minNode == nullptr || m_numNodes == 0) { return nullptr; }
  if (m_minNode != nullptr && m_numNodes == 1) {
    fsn::ds::FibonacciHeapNode* tmp = m_minNode;
    m_minNode = nullptr;
    m_numNodes = 0;
    return tmp;
  }
  // Add the child root list of m_minNode to the root list of this heap
  m_minNode->leftSib->rightSib = m_minNode->rightSib;
  m_minNode->rightSib->leftSib = m_minNode->leftSib;
    
  fsn::ds::FibonacciHeapNode* temp = m_minNode;
    
  // Since the heap is not empty, it won't return a nullptr
  m_minNode = fsn::ds::FibonacciHeap::m_identifyMin(*this, m_minNode->leftSib);

  temp->leftSib = temp;
  temp->rightSib = temp;

  m_numNodes--;
  
  // If no child root list then we are done
  if (temp->child == nullptr) { return temp; }
  int currentNumNodes = m_numNodes;
  
  // Now, we treat the child subtree as another fibonacci heap and we merge the two heaps
  FibonacciHeap remFH = FibonacciHeap(temp->child);
  // Properly set the min node of this FH and un parent all the root list nodes
  remFH.m_minNode = fsn::ds::FibonacciHeap::m_identifyMin(remFH, temp->child, true);
  
  fsn::ds::FibonacciHeap::merge(remFH);
  m_numNodes = currentNumNodes;

  // TO-DO: Add consolidation step here

  return temp;
}
