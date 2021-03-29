//
//  fusiontree.hpp
//  Fusion Tree
//
//  Created by Rogerio Aristida Guimaraes Junior on 1/23/18.
//

#ifndef fusiontree_hpp
#define fusiontree_hpp

#include <stdio.h>
#include <memory>
#include <vector>

#include "big_int.hpp"

using namespace std;
class environment {
 public:
  int word_size;     // Size of the type being used as big int, in bits
  int element_size;  // Size of the element of the fusion_tree, must be a square
  int sqrt_element_size;  // Value of sqrt(element_size), necessary for most
                          // significant bit
  int capacity;           // maximum number of integers in a fusion tree

  // bitmasks precalculated to avoid use of <<
  big_int *shift_1, *shift_neg_1, *shift_neg_0;
  // integers used by fast_most_significant_bit
  big_int clusters_first_bits, perfect_sketch_m;
  // integers used in parallel comparison by cluster_most_significant_bit
  big_int repeat_int;       // integer used by parallel comparison
                            // to repeat a number multiple times
  big_int powers_of_two;    // bitmask with the powers of two
                            // in ascending order
  big_int interposed_bits;  // bitmask used to extract the bits
                            // interposed among the repetitions of a
                            // number

  environment(int word_size_ = 4000, int element_size_ = 3136,
              int capacity_ = 5);
  ~environment();

  // first step of fast_most_significant_bit
  const int cluster_most_significant_bit(big_int x) const;

  // find the most significant bit of a big_int in O(1) in word RAM model
  const int fast_most_significant_bit(big_int const &x) const;

  // find the longest common prefix between two big_ints in O(1) in word RAM
  // model
  const int fast_first_diff(big_int const &x, big_int const &y) const;
};

class fusiontree {
 private:
  environment *my_env;  // object with the specifications of the fusion tree

  big_int data;  // sketched integers

  big_int *elements;  // array with the original values of the elements of the
                      // fusiontree
  int sz;             // size of tree

  big_int repeat_int;                   // integer used by parallel comparison
                                        // repeats a number multiple times
  big_int extract_interposed_bits;      // bitmask used to extract the bits
                                        // interposed among the repetitions of a
                                        // number
  big_int extract_interposed_bits_sum;  // bitmask used to extract the sum of
                                        // bits interposed among the repetitions
                                        // of a number, after having gathering
                                        // them together

  big_int m;            // integer m
  int *m_indices;       // array to keep the position of the set bits of m
  big_int sketch_mask;  // mask of all the m_i+b_i sums

  int important_bits_count;     // number of important bits
  big_int mask_important_bits;  // mask of important bits
  int *important_bits;  // indexes of the important bits of the elements in the
                        // fusion tree

  // add numbers from a vector to array elements
  void add_in_array(vector<big_int> &elements_);

  // finds the important bits of a set of integers
  void find_important_bits();

  // finds an integer m and sketch_mask to be used for sketching
  void find_m();

  // sets the variables used in parallel comparison
  void set_parallel_comparison();

  // returns the approximate sketch, in the fusion tree, of a given number
  const big_int approximate_sketch(const big_int &x) const;

  // returns an integer with O(w^(1/5)) sketches of x, separated by zeroes
  const big_int multiple_sketches(const big_int &x) const;

  // returns the index of the biggest y in the tree succh that
  // sketch(y)<=sketch(x)
  const int find_sketch_predecessor(const big_int &x) const;

 public:
  // returns the number of integers stored
  const int size() const;

  // returns the number in a given position in the tree
  const big_int pos(int i) const;

  // returns the index of the biggest k in the tree succh that k<=x
  // or -1 if there is no such k
  const int find_predecessor(const big_int &x) const;

  // fusiontree constructor
  // v_ is a vector with the integers to be stored
  fusiontree(vector<big_int> &v_, environment *my_env_);

  // fusiontree destructor
  ~fusiontree();
};

// prints all the numbers, in binary form, in a fusion tree
std::ostream &operator<<(std::ostream &out, const fusiontree &t);

#endif /* fusiontree_hpp */
