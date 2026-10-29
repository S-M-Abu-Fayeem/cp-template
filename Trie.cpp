/*
 * Binary Trie (XOR Trie) template
 * ---------------------------------
 * Stores non-negative integers bit by bit, from the most significant bit
 * (bit 30) down to bit 0. Each node remembers how many inserted numbers
 * pass through it, which lets us answer counting queries quickly.
 *
 * Supported operations (each O(BITS) = O(31)):
 *   insert(value)                  -> add a number to the trie
 *   countXorLessThan(x, limit)     -> how many stored v satisfy (v ^ x) < limit
 *   maxXor(x)                      -> max over stored v of (v ^ x)
 *   minXor(x)                      -> min over stored v of (v ^ x)
 *   freeTree(node)                 -> release memory (useful for multiple test cases)
 *
 * Example problem solved in main():
 *   Given an array a[0..n-1] and k, count subarrays whose XOR is >= k.
 */

#include <bits/stdc++.h>
using namespace std;

struct BinaryTrie {
  static const int BITS = 31;  // numbers up to 2^31 - 1 fit (int range)

  struct Node {
    Node* child[2];   // child[0] = next bit is 0, child[1] = next bit is 1
    int passCount;    // how many inserted numbers go through this node

    Node() {
      child[0] = child[1] = nullptr;
      passCount = 0;
    }
  };

  Node* root;

  BinaryTrie() {
    root = new Node();
  }

  // Add `value` to the trie, walking from the highest bit to the lowest.
  void insert(int value) {
    Node* node = root;
    node->passCount++;
    for (int bit = BITS - 1; bit >= 0; bit--) {
      int bitValue = (value >> bit) & 1;
      if (node->child[bitValue] == nullptr) node->child[bitValue] = new Node();
      node = node->child[bitValue];
      node->passCount++;
    }
  }

  // Count stored numbers v such that (v ^ x) < limit.
  //
  // Idea: walk down the trie while keeping (v ^ x) equal to `limit` on all
  // bits seen so far. At each bit:
  //   - If limit's bit is 1: every v whose XOR bit here is 0 is already
  //     smaller than limit, so add that whole subtree. Then continue along
  //     the branch where the XOR bit is 1 (still tied with limit).
  //   - If limit's bit is 0: the XOR bit must be 0 to stay tied
  //     (a 1 would make it larger), so continue along that branch only.
  // Numbers whose XOR equals `limit` exactly are never counted (strict <).
  int countXorLessThan(int x, int limit) {
    Node* node = root;
    int count = 0;
    for (int bit = BITS - 1; bit >= 0; bit--) {
      if (node == nullptr) break;  // no stored number is still tied with limit
      int xBit = (x >> bit) & 1;
      int limitBit = (limit >> bit) & 1;

      // The child with the same bit as x gives XOR bit 0;
      // the opposite child gives XOR bit 1.
      Node* xorZero = node->child[xBit];
      Node* xorOne = node->child[!xBit];

      if (limitBit == 1) {
        if (xorZero) count += xorZero->passCount;  // XOR bit 0 < 1: all smaller
        node = xorOne;                              // XOR bit 1 == 1: still tied
      } else {
        node = xorZero;                             // XOR bit 0 == 0: still tied
      }
    }
    return count;
  }

  // Maximum of (v ^ x) over all stored v. Trie must not be empty.
  // Greedy: at each bit, prefer the child with the opposite bit of x,
  // since that makes this XOR bit 1.
  int maxXor(int x) {
    Node* node = root;
    int result = 0;
    for (int bit = BITS - 1; bit >= 0; bit--) {
      int xBit = (x >> bit) & 1;
      result <<= 1;
      if (node->child[!xBit]) {
        node = node->child[!xBit];
        result |= 1;               // got a 1 in this XOR bit
      } else {
        node = node->child[xBit];  // forced to take 0 in this XOR bit
      }
    }
    return result;
  }

  // Minimum of (v ^ x) over all stored v. Trie must not be empty.
  // Greedy: at each bit, prefer the child with the same bit as x,
  // since that makes this XOR bit 0.
  int minXor(int x) {
    Node* node = root;
    int result = 0;
    for (int bit = BITS - 1; bit >= 0; bit--) {
      int xBit = (x >> bit) & 1;
      result <<= 1;
      if (node->child[xBit]) {
        node = node->child[xBit];  // got a 0 in this XOR bit
      } else {
        node = node->child[!xBit];
        result |= 1;               // forced to take 1 in this XOR bit
      }
    }
    return result;
  }

  // Recursively free every node in the subtree (call freeTree(root)).
  void freeTree(Node* node) {
    for (int b = 0; b < 2; b++)
      if (node->child[b]) freeTree(node->child[b]);
    delete node;
  }
} trie;

int32_t main() {
  ios_base::sync_with_stdio(0);
  cin.tie(0);

  int n, k;
  cin >> n >> k;

  // Key fact: XOR of subarray a[l..r] = prefix[r+1] ^ prefix[l],
  // where prefix[i] = a[0] ^ a[1] ^ ... ^ a[i-1] and prefix[0] = 0.
  //
  // Strategy: count ALL subarrays, then subtract those with XOR < k.
  // For each right end, query the trie of earlier prefix XORs to count
  // how many left ends give (prefixXor ^ earlierPrefix) < k.

  int prefixXor = 0;
  long long answer = 1LL * n * (n + 1) / 2;  // total number of subarrays

  trie.insert(prefixXor);  // prefix[0] = 0 (empty prefix)

  for (int i = 0; i < n; i++) {
    int value;
    cin >> value;
    prefixXor ^= value;                               // now prefix[i+1]
    answer -= trie.countXorLessThan(prefixXor, k);    // subarrays ending at i with XOR < k
    trie.insert(prefixXor);                           // make it available as a left end
  }

  cout << answer << '\n';  // subarrays with XOR >= k
  return 0;
}
