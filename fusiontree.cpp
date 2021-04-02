//
//  fusiontree.cpp
//  Fusion Tree
//
//  Created by Rogerio Aristida Guimaraes Junior on 1/23/18.
//

#include "fusiontree.hpp"

#include <algorithm>
#include <cmath>

#include <iostream>

// environment constructor
// also initializes the tricks of the environment
environment::environment(int word_size_, int element_size_, int capacity_)
    : word_size(word_size_), element_size(element_size_), capacity(capacity_) {
  // check if restrictions were not violated, and raise error otherwise
  try {
    sqrt_element_size = sqrt(element_size);
    if (sqrt_element_size * sqrt_element_size != element_size) {
      throw(string("element_size is not a square"));
    }
    int capacity_to_4 = capacity * capacity * capacity * capacity;
    int capacity_to_5 = capacity_to_4 * capacity;
    if (capacity_to_5 > element_size) {
      throw(string("element_size is too small for the fusion tree capacity"));
    }

    if (capacity_to_5 + capacity_to_4 > word_size) {
      throw(string("word_size is too small for the fusion tree capacity"));
    }
  } catch (const string msg) {
    cerr << msg << endl;
  }

  // initializes the basic bitmasks used in bit tricks
  shift_1 = new big_int[word_size];
  shift_neg_1 = new big_int[word_size];
  shift_neg_0 = new big_int[word_size];

  // calculates the basic bitmasks used in bit tricks
  for (int i = 0; i < word_size; i++) {
    shift_1[i] = (big_int(1) << i);
    shift_neg_1[i] = (~big_int(1) << i);
    shift_neg_0[i] = (~big_int(0) << i);
  }

  // Find the value of clusters_first_bits
  for (int i = 0; i < sqrt_element_size; i++) {
    // for each cluster of bits of size sqrt_element_size
    // add the first bit of the cluster in the bitmask clusters_first_bits
    clusters_first_bits =
        clusters_first_bits |
        (shift_1[sqrt_element_size - 1 + i * sqrt_element_size]);
  }

  // Find the value of perfect_sketch_m, which we use to sketch the first bits
  // of the clusters of size sqrt_element_size. Note that the important bits are
  // such that. We will use m_i = element_size - (sqrt_element_size - 1) - i *
  // sqrt_element_size + i.
  for (int i = 0; i < sqrt_element_size; i++) {
    // for each b_i, just apply the formula
    perfect_sketch_m =
        perfect_sketch_m | (shift_1[element_size - (sqrt_element_size - 1) -
                                    i * sqrt_element_size + i]);
  }

  // Find the value of interposed_bits
  for (int i = 0; i < sqrt_element_size; i++) {
    interposed_bits =
        interposed_bits |
        (shift_1[sqrt_element_size + i * (sqrt_element_size + 1)]);
  }

  // Find the value of repeat_int
  for (int i = 0; i < sqrt_element_size; i++) {
    repeat_int = repeat_int | (shift_1[i * (sqrt_element_size + 1)]);
  }

  // Find the value of powers_of_two
  for (int i = 0; i < sqrt_element_size; i++) {
    powers_of_two =
        powers_of_two |
        (shift_1[sqrt_element_size - i - 1 + i * (sqrt_element_size + 1)]);
  }
}

// environment deconstructor
// It just needs to free the dynamically allocated arrays in the class
environment::~environment() {
  // use free to delete an array
  delete[] shift_1;
  delete[] shift_neg_1;
  delete[] shift_neg_0;
}

// returns the most significant bit of a cluster of bits of size
// sqrt_element_size. It basically uses parallel comparison to determine
// the greatest power of two that is not greater than the cluster
const int environment::cluster_most_significant_bit(big_int x) const {
  // creates sqrt repetitions of cluster x, with one bit between consecutive
  // repetiitions
  x = x * repeat_int;
  // set the bits between repetitions
  x = x | interposed_bits;
  // calculate the difference between x and powers_of_two
  // The interposed bit before repetition i will remain significant if
  // 2^i is smaller than or equal to x
  x = x - powers_of_two;
  // extract all the bits interposed among the repetitions of x
  x = x & interposed_bits;
  // the number of significant bits is the number of powers smaller then
  // x. Multiply the extracted bits by repeat_int to make then add up
  // together before the first interposed bit
  x = x * repeat_int;
  // shift the result to the right to ignore the trash created after the first
  // interposed bit
  x = x >> ((element_size) + (sqrt_element_size - 1));
  // extract only the the number of bits in a cluster to ignore trash created
  // before the interval where the extracted bits were added
  x = x & (~shift_neg_0[sqrt_element_size + 1]);
  // the number of powers of two which are not greater than x, subtracted by 1,
  // is the greatest power of two which is not greater than x, i.e., the index
  // of the most significant bit of the cluster
  return (int)x - 1;
}

// find the most significant bit of a big_int in O(1) in word RAM model
const int environment::fast_most_significant_bit(big_int const &x) const {
  // We will divide our number x of size element_size in sqrt_element_size
  // clusters of bits of size sqrt_element_size.
  // Extract the first bit of each cluster
  big_int x_clusters_first_bits = x & clusters_first_bits;

  // use XOR between x and the last result to make the first bit of each cluster
  // insignificant, i.e., consider only the rest of the cluster
  big_int x_remain = x ^ x_clusters_first_bits;

  // subtract the remains of the clusters from clusters_first_bits and only the
  // clusters with significant bits in their remains will have a zero as their
  // first bit
  x_remain = clusters_first_bits - x_remain;

  // extract only those first bits
  x_remain = x_remain & clusters_first_bits;

  // and use an XOR with cluster_first_bits to swap the value of the first
  // bits. Now the first bit is 1 if there is any significant bit in the rest of
  // the cluster
  x_remain = x_remain ^ clusters_first_bits;

  // then, an OR operator between the first bits of the clusters and x_remain
  // will result in an integer in which the first bit of each cluster is 1 if
  // there is any significant bit in that cluster
  big_int x_significant_clusters = x_remain | x_clusters_first_bits;

  // Then we have to sketch x_significant_clusters
  // We are using m_i = element_size - (sqrt_element_size - 1) - i *
  // sqrt_element_size + i. Note that, since b_i = sqrt_element_size - 1 + i *
  // sqrt_element_size, we have that m_i + b_i = element_size+1, so if we shift
  // x * perfect_sketch_m to the right by element_size bits, we already have the
  // sketch. We only have to extract the last sqrt_element_size bits.
  x_significant_clusters =
      ((x_significant_clusters * perfect_sketch_m) >> element_size) &
      (~shift_neg_0[sqrt_element_size]);

  // to find the index of the most significant cluster, i.e., the first cluster
  // with significant bits, we only have to find the most_significant_bit of
  // x_significant_clusters. Since we only have sqrt_element_size bits, we can
  // use the function cluster_most_significant_bit to do it
  int most_significant_cluster_idx =
      cluster_most_significant_bit(x_significant_clusters);

  // Then we will extract only the bits of the most significant cluster and
  // shift the number to the right until it becomes the first cluster
  big_int most_significant_cluster =
      (x >> (most_significant_cluster_idx * sqrt_element_size)) &
      (~shift_neg_0[sqrt_element_size]);

  // Now we only have to find the most significant bit of that cluster, which,
  // again, can be done using cluster_most_significant_bit. Since we know the
  // original index of the cluster, it is easy to know the original index of its
  // most significant bit and keep it in ans
  int ans = most_significant_cluster_idx * sqrt_element_size +
            cluster_most_significant_bit(most_significant_cluster);

  // if answer is negative, then there is no most significant bit, i.e., the
  // number is zero. Set ans to -1
  if (ans < 0) {
    ans = -1;
  }

  // return ans
  return ans;
}

// find the longest common prefix between two big_ints in O(1) in word RAM model
const int environment::fast_first_diff(big_int const &x,
                                       big_int const &y) const {
  // the first different bit between two integers x and y is the most
  // significant bit in x XOR y
  return fast_most_significant_bit(x ^ y);
}

// add numbers from a vector to array elements

void fusiontree::add_in_array(vector<big_int> &elements_) {
  // sets variable sz, which keeps the size of the fusion tree
  sz = elements_.size();

  // copies all the elements from the vector to array elements
  for (int i = 0; i < elements_.size(); i++) {
    elements[i] = elements_[i];
  }

  // sorts the array elements in ascending order
  std::sort(elements, elements + size());
}

// finds the important bits of a set of integers

void fusiontree::find_important_bits() {
  // if the fusion tree has a single element, there are no important bits
  if (size() == 1) return;

  // insert the elements one by one and, for each of them, find the first
  // bit that differentiates that element from any other element already
  // inserted in the fusion tree
  for (int i = 1; i < size(); i++) {
    // for each element i, find where it first differentiates from the first
    // element
    int diff_point = my_env->fast_first_diff(pos(i), pos(0));

    // then check if there is any other element that differentiate from i in a
    // later bit
    for (int j = 1; j < i; j++) {
      // for each other element j
      // find where it first differentiates from i
      int temp = my_env->fast_first_diff(pos(i), pos(j));
      // if it is later then the first element, keep the index of such bit
      if (temp < diff_point) diff_point = temp;
    }

    // the first bit that differentiates element i from any other in the fusion
    // tree is an important bit, so add it in mask_important_bits
    mask_important_bits = mask_important_bits | my_env->shift_1[diff_point];
  }

  // keep all the important bits found in array important_bits and update the
  // value of important_bits_count
  for (int i = 0; i < my_env->word_size; i++) {
    // foir each bit position i in the word size given
    // if bit in position i is an important bit
    if ((mask_important_bits & my_env->shift_1[i]) != big_int(0)) {
      // add such position to array important_bits
      important_bits[important_bits_count] = i;
      // and update the value of important_bits_count
      important_bits_count++;
    }
  }
}

// finds an integer m, used to find the sketch of a numeber, and sketch_mask,
// used to extract the important bits from it

void fusiontree::find_m() {
  // precalculates the third power of the number of important bits
  int important_bits_count_to_3 =
      important_bits_count * important_bits_count * important_bits_count;

  // find the indices of the set bits in m
  // tag is a bitmask used to denote that a certain position cannot be used
  // anymore because it would cause a collision between the position of two
  // distinct important bits after the multiplication by m to find the sketch of
  // a number
  big_int tag = 0;

  // for every important bit b_i we will find an integer m_i such that b_i+m_i
  // is distinct for all i, modulo capacity^3. If each m_i represents one set
  // bit in m, it garantees that each important bit of the number x we are
  // sketching will be taken to a different position in x*m
  for (int i = 0; i < important_bits_count; i++) {
    // for every important bit b_i
    for (int j = 0; j < important_bits_count_to_3; j++) {
      // we will search for the next position m_i available in m
      // such that b_i+m_i is unique
      if ((tag & my_env->shift_1[j]) == big_int(0)) {
        // if a position is not tagged, then it means there is no other pair
        // b_j, m_j such that b_j+m_j=b_i+m_i thus this position can be m_i
        m_indices[i] = j;

        // then we need to tag every single position p such that b_i+m_i=b_j+p,
        // for any pair of important bits b_i, b_j, so that p does not be chosen
        // as any m_j. This is the same of tagging every position p such that
        // m_i+b_i-b_j=p Thus, for every pair of important bits
        for (int k1 = 0; k1 < important_bits_count; k1++) {
          for (int k2 = 0; k2 < important_bits_count; k2++) {
            // We tag the value of m_i+b_i-b_j, if its positive
            if ((j + important_bits[k1] - important_bits[k2]) >= 0) {
              // adding a bit in the bitmask tag
              tag = tag | (my_env->shift_1[j + important_bits[k1] -
                                           important_bits[k2]]);
            }
          }
        }

        // it is garanteed that we will find enough all the values for all m_i
        // in the first capacity^3 bit positions.

        // after finding and m_i for b_i, we will break the loop and find
        // m_(i+1)
        break;
      }
    }
  }
  // we already have m_i+b_i distinct modulo capacity^3, now we have to spread
  // the values of m_i+b_i such that each of them is in a distinct interval of
  // size capacity^3, so that we can also maintain the order of the important
  // bits in the sketch of x
  for (int i = 0; i < important_bits_count; i++) {
    // adding i*capacity^3 to each m_i garantees that each m_i will be in a
    // distinct interval of capacity^3 bits. Also, m_i will be in interval i.
    // However, we want m_i+b_i to be in interval i, thus we must add
    // i*capacity^3-b_i. However it can be negative, so we will add, instead,
    // element_size-b_i+i*capacity^3, rounded down to the nearest multiple of
    // capacity^3
    m_indices[i] = m_indices[i] + (important_bits_count_to_3 *
                                   ((my_env->element_size - important_bits[i] +
                                     i * important_bits_count_to_3) /
                                    important_bits_count_to_3));
    // then we set up the bit m_i of m
    m = m | my_env->shift_1[m_indices[i]];

    // then, we have to set sketch_mask, which is a bitmask that allows us to
    // extract the important bits of x after we multiply x*m. It is easy to see
    // that each bit b_i will be found in position b_i+m_i, thus we only have to
    // set a bit mask with those bits
    sketch_mask =
        sketch_mask | (my_env->shift_1[important_bits[i] + m_indices[i]]);
    // note that the distance between the first and last important bits is at
    // most capacity*(capacity^3)=capacity^4. Since element_size > capacity^5,
    // we can store all the sketches in a single element
  }
}

// sets the variable data that will keep the sketched numbers, as well as the
// bit masks which are necessary for the parallel comparison

void fusiontree::set_parallel_comparison() {
  // precalculates important_bits_count^4
  int important_bits_count_to_4 = important_bits_count * important_bits_count *
                                  important_bits_count * important_bits_count;

  // set variable data
  // for each element in the fusiontree, add their sketch to data
  for (int i = 0; i < my_env->capacity; i++) {
    // add the interposed bit right before the element sketch to be inserted
    data = data | my_env->shift_1[(i + 1) * important_bits_count_to_4 + i];
    // then add the element that is in position capacity - 1 - i (to be in
    // decreasing order), in its right place
    data = data | (approximate_sketch(pos(my_env->capacity - 1 - i))
                   << i * (important_bits_count_to_4 + 1));
  }

  // set bitmask repeat_int, which is a repetition of 000...01, to make a
  // sequence if bits repeat itself multiple times, leaving one bit interposed
  // between two repetitionse
  for (int i = 0; i < my_env->capacity; i++) {
    // just add 1 in the end of each interval of 000...01
    repeat_int =
        repeat_int | my_env->shift_1[i * (important_bits_count_to_4 + 1)];
  }

  // set extract_interposed_bits, which is a bitmask with the positions of the
  // bits interposed among the repetions of a sequence of bits made with
  // repeat_int
  for (int i = 0; i < my_env->capacity; i++) {
    // just add 1 between in the positions between the repetitions of each
    // interval
    extract_interposed_bits =
        extract_interposed_bits |
        my_env->shift_1[(i + 1) * important_bits_count_to_4 + i];
  }

  // set extract_interposed_bits_sum, which is a bitmask for the first
  // important_bits_count^4 bits of a number. After multiplying, extracting the
  // interposed bits, multiplying again by repeat_int, and shifting the number
  // enough to the right, all interposed bits add up here
  for (int i = 0; i < important_bits_count_to_4; i++) {
    // just add 1 for each of the first important_bits_count^4 bits
    extract_interposed_bits_sum =
        extract_interposed_bits_sum | my_env->shift_1[i];
  }
}

// returns the approximate sketch, in the fusion tree, of a given number

const big_int fusiontree::approximate_sketch(const big_int &x) const {
  // extract the important bits of the number, multiply them by m and shift to
  // the right b_i+m_i positions so that the last significant bit go to position
  // 0
  return ((((x & mask_important_bits) * m) & sketch_mask) >>
          (important_bits[0] + m_indices[0]));
}

// returns an integer with capacity repetitions of the sketch of x, separated by
// one zero between any consecutive repetitions

const big_int fusiontree::multiple_sketches(const big_int &x) const {
  // calculate the approximate sketch of x and multiply by the variable
  // repeat_int
  return approximate_sketch(x) * repeat_int;
}

// returns the index of the biggest y in the tree such that
// sketch(y)<=sketch(x), using parallel comparison

const int fusiontree::find_sketch_predecessor(const big_int &x) const {
  // precalculate the value of important_bits_count^4
  int important_bits_count_to_4 = important_bits_count * important_bits_count *
                                  important_bits_count * important_bits_count;

  // calculate the difference between data and multiple_sketches(x)
  // all the interposed bits before sketches greater than sketch(x) will remain
  // significant
  big_int diff = data - multiple_sketches(x);
  // extract all the bits interposed among sketches of the elements in data
  diff = diff & extract_interposed_bits;
  // the number of significant bits is the number of sketches greater then
  // sketch(x) multiply the extracted bits by repeat_int to make then add up
  // together before the first interposed bit
  diff = diff * repeat_int;
  // shift the result to the right to ignore the trash created after the first
  // interposed bit
  diff = diff >> ((my_env->capacity * important_bits_count_to_4) +
                  (my_env->capacity - 1));
  // extract only the the number of bits in a sketch to ignore trash created
  // before the interval where the extracted bits were added
  diff = diff & extract_interposed_bits_sum;

  // the position of sketch(x) can be calculated using the number of sketches in
  // the fusion tree and the number of sketches greater than sketh(x)
  int answer = size() - (int)diff - 1;

  // check if the corner case in which the sketch is already in the fusion tree
  if (answer + 1 < size() and
      approximate_sketch(elements[answer + 1]) == approximate_sketch(x)) {
    answer++;
  }
  // return the position found
  return answer;
}

// returns the number of integers stored

const int fusiontree::size() const { return sz; }

// returns the number in a given position in the tree

const big_int fusiontree::pos(int i) const { return elements[i]; }

// returns the index of the biggest k in the tree succh that k<=x
// or -1 if there is no such k

const int fusiontree::find_predecessor(const big_int &x) const {
  // first, find the position of sketch(x) among the sketches of the elements in
  // the fusion tree keep the element right before and right after sketch(x)
  int idx1 = find_sketch_predecessor(x);
  // indices of sketch predecessor and sucessor
  int idx2 = idx1 + 1;

  // lowest common ancestor with predecessor and sucessor
  // it is the same of the longest common prefix, so we can find lca(a,b) by
  // fiding the first different bit between a and b
  int lca1, lca2;

  // if the sketch has no predecessor, tag lca1
  // else calculate the lca between x and its predecessor and keep in lca1
  if (idx1 < 0) {
    lca1 = -2;
  } else {
    lca1 = my_env->fast_first_diff(elements[idx1], x);
  }

  // if the sketch has no sucessor, tag lca2
  // else calculate the lca between x and its sucessore and keep in lca1
  if (idx2 < size()) {
    lca2 = my_env->fast_first_diff(elements[idx2], x);
  } else {
    lca2 = -2;
  }

  // if lca1 is negative, then the number is the same as its predecessor
  // return idx1
  if (lca1 == -1) {
    return idx1;
  }
  // if lca2 is negative, then the number is the same as its sucessor
  // return idx2
  if (lca2 == -1) {
    return idx2;
  }
  int lca;

  // if lca1 is tagged, there is no predecessor, then use lca2, the lca with the
  // sucessor
  if (lca1 == -2) lca = lca2;
  // if lca2 is tagged, there is no sucecessor, then use lca1, the lca with the
  // predessor
  if (lca2 == -2) lca = lca1;
  // if both lca1 and lc2 are tagged, pick the highest lca
  if (lca1 != -2 and lca2 != -2) lca = min(lca1, lca2);

  int answer;
  big_int e;

  // if the bit that first differentiates x is 1, then x lies in the right
  // subtree of the lca, therefore, this subtree is empty and the predecessor of
  // x is the greatest number in the left subtree. Since the sketches keep the
  // elements order, it is the element with the righmost sketch in that subtree.
  // If p is the path to the lca, we just need to find the sketch predecessor of
  // e = p0111...11
  if ((x & my_env->shift_1[lca]) != big_int(0)) {
    // first, add p0 to e, extracting the bits in p from x and adding 0
    e = x & my_env->shift_neg_1[lca];
    // than add a bunch of 1s
    e = e | (my_env->shift_1[lca] - big_int(1));
    // the answer will be the sketch predecessor of e
    answer = find_sketch_predecessor(e);
  }

  // if the bit that first differentiates x is 0, then x lies in the left
  // subtree of the lca therefore, this subtree is empty and the sucessor of x
  // is the smallest number in the right subtree. Since the sketches keep the
  // elements order, it is the element with the leftmost sketch in that subtree.
  // If p is the path to the lca, we just need to find the sketch predecessor of
  // e = p1000...00
  else {
    // first, add p1 to e, extracting the bits in p from x and adding 1
    e = x | my_env->shift_1[lca];
    // than add a bunch of 0s
    e = e & my_env->shift_neg_0[lca];
    // the answer will return either the sucessor of x or nothing
    answer = find_sketch_predecessor(e);
    // if it returns the sucessor, just pick the predecessor of the sucessor
    // which is easy because of array elements
    if (answer >= 0 and elements[answer] > x) {
      answer--;
    }
  }

  // return the answer
  return answer;
}

// fusiontree constructor
// v_ is a vector with the integers to be stored
// my_env is the environment with the specifications of the fusion tree

fusiontree::fusiontree(vector<big_int> &elements_, environment *my_env_) {
  // set the values of the class variables
  // see class fusiontree in the header file for comments on each variable

  my_env = my_env_;  // keeps a pointer to the environment *my_env_

  // creates the arrays necessary for the fusion tree, allocating
  // dynamically because variable lenght arrays are forbidden as class members
  elements = new big_int[my_env->word_size];
  m_indices = new int[my_env->word_size];
  important_bits = new int[my_env->capacity];
  data = 0;
  important_bits_count = 0;

  // adds the elements of the array elements_, passed as a reference in the
  // first argument in the array elements, which is a class member, and keeps
  // the elements in the fusion tree
  add_in_array(elements_);

  // finds the important bits of the elements in the fusion tree,
  // and saves them in array important_bits, as well as a bitmask of them in
  // big_int mask_important_bits, and the number of important bits in
  // important_bits_count
  find_important_bits();

  // finds the value of big_int m, which is used in parallel comparison,
  // and the value of sketch_mask, the bitmask to extract the important bits
  // from the sketched integer
  find_m();

  // the last thing to be done to initialize a fusion tree
  set_parallel_comparison();
}

// fusiontree destructor
// It just needs to free the dynamically allocated arrays in the class

fusiontree::~fusiontree() {
  // use delete [] to free an array
  delete[] elements;
  delete[] m_indices;
  delete[] important_bits;
}

// prints all the numbers, in binary form, in a fusion tree
ostream &operator<<(ostream &out, const fusiontree &t) {
  for (int i = 0; i < t.size(); i++) {
    out << t.pos(i) << endl;
  }

  return out;
}
