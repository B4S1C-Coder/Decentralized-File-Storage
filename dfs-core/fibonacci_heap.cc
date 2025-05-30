#include "fibonacci_heap.hh"
#include <shared_mutex>
#include <mutex>
#include <cstddef>
#include <cmath>
#include <vector>

fsn::ds::FibonacciHeapNode* fsn::ds::FibonacciHeap::insert(size_t k) {
  std::unique_lock<std::shared_mutex> lock(m_rwLock);
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
  std::unique_lock<std::shared_mutex> lock(m_rwLock);
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
  std::unique_lock<std::shared_mutex> lock(m_rwLock);
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
  m_consolidate();

  return temp;
}

void fsn::ds::FibonacciHeap::m_consolidate() {
  if (m_minNode == nullptr) {
    return;
  }

  int maxDegree = static_cast<int>(log2(m_numNodes)) + 1;
  std::vector<fsn::ds::FibonacciHeapNode*> degreeTable(maxDegree, nullptr);

  std::vector<fsn::ds::FibonacciHeapNode*> rootNodes;
  fsn::ds::FibonacciHeapNode* current = m_minNode;

  do {
    rootNodes.push_back(current);
    current = current->rightSib;
  } while (current != m_minNode);

  for (fsn::ds::FibonacciHeapNode* node: rootNodes) {
    int degree = node->degree;

    while (degreeTable[degree] != nullptr) {
      fsn::ds::FibonacciHeapNode* other = degreeTable[degree];

      if (node->key > other->key) {
        std::swap(node, other);
      }

      // Make 'other' a child of 'node'
      other->leftSib->rightSib = other->rightSib;
      other->rightSib->leftSib = other->leftSib;

      other->parent = node;
      other->rightSib = node->child;
      other->leftSib = (node->child) ? node->child->leftSib : other;

      if (node->child) {
        node->child->leftSib->rightSib = other;
        node->child->leftSib = other;
      } else {
        node->child = other;
      }

      node->degree++;

      degreeTable[degree] = nullptr;
      degree++;
    }

    degreeTable[degree] = node;
  }

  // Reset min-pointer
  m_minNode = nullptr;
  for (auto node: degreeTable) {
    if (node != nullptr) {
      if (m_minNode == nullptr || node->key < m_minNode->key) {
        m_minNode = node;
      }
    }
  }
}

void fsn::ds::FibonacciHeap::decreaseKey(fsn::ds::FibonacciHeapNode* node, size_t newKey) {
  std::unique_lock<std::shared_mutex>lock(m_rwLock);

  if (newKey > node->key) { return; }

  node->key = newKey;
  fsn::ds::FibonacciHeapNode* parent = node->parent;

  if (parent == nullptr && node->key < parent->key) {
    if (node->rightSib != node) {
      node->leftSib->rightSib = node->rightSib;
      node->rightSib->leftSib = node->leftSib;
    }

    if (parent->child == node) {
      parent->child = (node->rightSib != node) ? node->rightSib : nullptr;
    }

    parent->degree--;
    node->parent = nullptr;
    node->marked = false;

    node->rightSib = m_minNode->rightSib;
    m_minNode->rightSib->leftSib = node;
    node->leftSib = m_minNode;
    m_minNode->rightSib = node;

    fsn::ds::FibonacciHeapNode* grandparent = parent->parent;

    while (grandparent != nullptr) {
      if (!parent->marked) {
        parent->marked = true;
        break;
      }

      if (parent->rightSib != parent) {
        parent->leftSib->rightSib = parent->rightSib;
        parent->rightSib->leftSib = parent->leftSib;
      }

      if (grandparent->child == parent) {
        grandparent->child = (parent->rightSib != parent) ? parent->rightSib : nullptr;
      }

      grandparent->degree--;
      parent->parent = nullptr;
      parent->marked = false;

      parent->rightSib = m_minNode->rightSib;
      m_minNode->rightSib->leftSib = parent;
      parent->leftSib = m_minNode;
      m_minNode->rightSib = parent;

      parent = grandparent;
      grandparent = grandparent->parent;
    }
  }

  if (node->key < m_minNode->key) {
    m_minNode = node;
  }
}