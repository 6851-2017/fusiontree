//
//  big_int.hpp
//  Fusion Tree
//
//  Created by Rogerio Aristida Guimaraes Junior on 1/31/18.
//

#ifndef big_int_hpp
#define big_int_hpp

#include <stdio.h>

#include <bitset>

#define PSIZE 4000   // Number of bits printed in cout << big_int
#define PINTERV 100  // Number of bits in printing intervals
#define WSIZE 4000   // Size the big_int must have - O(max(K^5+K^4,w+sqrt(w))

class big_int {
 private:
  std::bitset<WSIZE> bs;

 public:
  operator int() const;
  int word_size() const;

  big_int(int x = 0);

  big_int(const std::bitset<WSIZE> &b);

  big_int operator~() const;

  big_int operator-() const;

  bool operator<(const big_int x) const;

  bool operator<=(const big_int x) const;

  bool operator>(const big_int x) const;

  bool operator>=(const big_int x) const;

  bool operator==(const big_int x) const;

  bool operator!=(const big_int x) const;

  big_int operator<<(const int x) const;

  big_int operator>>(const int x) const;

  big_int operator|(const big_int x) const;

  big_int operator&(const big_int x) const;

  big_int operator^(const big_int x) const;

  big_int operator+(const big_int x) const;

  big_int operator-(const big_int x) const;

  big_int operator*(const big_int x) const;
};

std::ostream &operator<<(std::ostream &out, const big_int &bi);

#endif /* big_int_hpp */
