#ifndef BST_H_
#define BST_H_

#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <stdexcept>
#include <string>
#include <sstream>
/*
 * Class definition
 */
template <typename T>
class BST {
 public:
  /* Return floor key in tree */
  const T& floor(const T &key);
  /* Return ceil key in tree */
  const T& ceil(const T &key);
  /* Return k-th smallest key in tree; acts as helper function to recur */
  const T& kth_small(const int kth);
  /* Return whether @key is found in tree */
  bool contains(const T& key);

  /* Return max key in tree */
  const T& max();
  /* Return min key in tree */
  const T& min();

  /* Insert @key in tree */
  void insert(const T &key);
  /* Remove @key from tree */
  void remove(const T &key);

  /* Print tree in-order */
  void print();

 private:
  struct Node{
  T key;
  std::unique_ptr<Node> left;
  std::unique_ptr<Node> right;
  };
  std::unique_ptr<Node> root;

  /* Useful recursive helper methods */
  Node* min(Node *n);
  void insert(std::unique_ptr<Node> &n, const T &key);
  void remove(std::unique_ptr<Node> &n, const T &key);
  void print(Node *n, int level);

  /* Recursively find kth_small element */
  bool recur_kth_small(int& k, const int& kth, Node** kth_small, Node* n);
};

/*
 * Implementation
 */

// Find floor of tree given key iteratively
template <typename T>
const T& BST<T>::floor(const T &key) {
  // Init ret val and vars for loop
  T* floor;
  bool floor_found = false;
  Node *n = root.get();

  // If tree is empty, throw error
  if (!n) {
    throw std::runtime_error("Empty tree");
  }

  // Run loop while a leaf node has not been reached. In loop, check if the key
  // of current node is less than or equal to the given key. If it is, that
  // means that it could potentially be a floor. Thus, check to see if it is
  // smaller than current value stored for floor. If we find that the current
  // Node's key is less than or equal to the given key, we continue to the next
  // iteration of the loop using the right subtree. Else, we continue using the
  // left subtree. We have this distinction because we know that the left
  // subtree's root has a key that is always less than the current Node's.
  // Hence we only proceed left if we are looking for a lesser key. Otherwise,
  // go right.
  while (n) {
    if (n->key <= key) {
      if (!floor_found) {
      floor = &n->key;
      } else if (n->key > *floor) {
          floor = &n->key;
      }
      floor_found = true;
      n = n->right.get();
    } else {
      n = n->left.get();
    }
  }

  // If no floor was found for the given key, throw error
  if (!floor_found) {
    std::string err = "Cannot find floor for key " + std::to_string(key);
    throw std::runtime_error(err);
  }

  // Return floor found
  return *floor;
}

// Find ceiling of tree given key iteratively
template <typename T>
const T& BST<T>::ceil(const T &key) {
  // Very similar implementation to floor method given above. Major difference
  // is the subtree that is traversed. For in depth explanation, see floor
  // method. Commenting here will only note differences between floor.
  T* ceil;
  bool ceil_found = false;
  Node *n = root.get();

  if (!n) {
    throw std::runtime_error("Empty tree");
  }

  // For each node until leaf Node found, check if the Node's key is greater
  // than or equal to given key. If it is, and is less than ceil, update ceil.
  // Then, continue with left subtree. Else, continue with right subtree.
  while (n) {
    if (n->key >= key) {
      if (!ceil_found) {
      ceil = &n->key;
      } else if (n->key < *ceil) {
          ceil = &n->key;
      }
      ceil_found = true;
      n = n->left.get();
    } else {
      n = n->right.get();
    }
  }
  if (!ceil_found) {
    std::string err = "Cannot find ceil for key " + std::to_string(key);
    throw std::runtime_error(err);
  }
  return *ceil;
}

// User/helper method to find kth smallest value in tree
template <typename T>
const T& BST<T>::kth_small(const int kth) {
  // Init vars for recursion
  int k = 0;
  Node* kth_small = root.get();
  // Run recursion
  recur_kth_small(k, kth, &kth_small, root.get());

  // Return value found from recursion
  return kth_small->key;
}

// Recursively find kth smallest value in tree. Each call returns a bool.
// Returns false if this call or a call that returned into this call has reached
// k elements. Otherwise, return true. This makes sure that the recursion ends
// once k elements have been checked. Other than that check, simple LNR
// (in order) traversal.
template <typename T>
bool BST<T>::recur_kth_small(int& k, const int& kth,
  Node** kth_small, Node* n) {
  // If this Node is a NIL Node, return true and continue recursion
  if (!n) return true;
  // Recur the left subtree. If the left subtree returns false, also return
  // false and end recursion.
  if (!recur_kth_small(k, kth, kth_small, n->left.get())) return false;
  // If k has reached kth, return false and end recursion
  if (k >= kth) return false;
  // If k has not reached kth, process Node. Meaning, update kth_smallest and
  // inc k.
  *kth_small = n;
  k++;
  // Recur right subtree and return its return value
  return (recur_kth_small(k, kth, kth_small, n->right.get()));
}

template <typename T>
bool BST<T>::contains(const T &key) {
  Node *n = root.get();

  while (n) {
  if (key == n->key)
    return true;

  if (key < n->key)
    n = n->left.get();
  else
    n = n->right.get();
  }

  return false;
}

template <typename T>
const T& BST<T>::max(void) {
  if (!root) throw std::runtime_error("Empty tree");
  Node *n = root.get();
  while (n->right) n = n->right.get();
  return n->key;
}

template <typename T>
const T& BST<T>::min(void) {
  return min(root.get())->key;
}

template <typename T>
typename BST<T>::Node* BST<T>::min(Node *n) {
  if (n->left)
  return min(n->left.get());
  else
  return n;
}

template <typename T>
void BST<T>::insert(const T &key) {
  insert(root, key);
}

template <typename T>
void BST<T>::insert(std::unique_ptr<Node> &n, const T &key) {
  if (!n)
  n = std::unique_ptr<Node>(new Node{key});
  else if (key < n->key)
  insert(n->left, key);
  else if (key > n->key)
  insert(n->right, key);
  else
  std::cerr << "Key " << key << " already inserted!\n";
}

template <typename T>
void BST<T>::remove(const T &key) {
  remove(root, key);
}

template <typename T>
void BST<T>::remove(std::unique_ptr<Node> &n, const T &key) {
  /* Key not found */
  if (!n) return;

  if (key < n->key) {
  remove(n->left, key);
  } else if (key > n->key) {
  remove(n->right, key);
  } else {
  /* Found node */
  if (n->left && n->right) {
    /* Two children: replace with min node in right subtree */
    n->key = min(n->right.get())->key;
    remove(n->right, n->key);
  } else {
    /* Replace with only child or with nullptr */
    n = std::move((n->left) ? n->left : n->right);
  }
  }
}

template <typename T>
void BST<T>::print() {
  if (!root) return;
  print(root.get(), 1);
  std::cout << std::endl;
}

template <typename T>
void BST<T>::print(Node *n, int level) {
  if (!n) return;

  print(n->left.get(), level + 1);
  std::cout << n->key
    << " (" << level << ") ";
  print(n->right.get(), level + 1);
}

#endif  // BST_H_
