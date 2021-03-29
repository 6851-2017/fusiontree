//
//  big_int.cpp
//  Fusion Tree
//
//  Created by Rogerio Aristida Guimaraes Junior on 1/31/18.
//

#include "big_int.hpp"
#include <iostream>

using namespace std;

int big_int::word_size() const { return WSIZE; }

big_int::operator int() const { return int(bs.to_ulong()); }

big_int::big_int(int x) { bs = x; }

big_int::big_int(const bitset<WSIZE> &b) { bs = b; }

big_int big_int::operator~() const { return big_int(~bs); }

big_int big_int::operator-() const { return ((~(*this)) + big_int(1)); }

bool big_int::operator<(const big_int x) const {
  return bs.to_string() < x.bs.to_string();
}

bool big_int::operator<=(const big_int x) const {
  return bs.to_string() <= x.bs.to_string();
}

bool big_int::operator>(const big_int x) const {
  return bs.to_string() > x.bs.to_string();
}

bool big_int::operator>=(const big_int x) const {
  return bs.to_string() >= x.bs.to_string();
}

bool big_int::operator==(const big_int x) const {
  return bs.to_string() == x.bs.to_string();
}

bool big_int::operator!=(const big_int x) const {
  return bs.to_string() != x.bs.to_string();
}

big_int big_int::operator<<(const int x) const { return big_int(bs << x); }

big_int big_int::operator>>(const int x) const { return big_int(bs >> x); }

big_int big_int::operator|(const big_int x) const { return big_int(bs | x.bs); }

big_int big_int::operator&(const big_int x) const { return big_int(bs & x.bs); }

big_int big_int::operator^(const big_int x) const { return big_int(bs ^ x.bs); }

big_int big_int::operator+(const big_int x) const {
  bitset<WSIZE> carry, res;

  for (int i = 0; i < WSIZE; i++) {
    res[i] = carry[i] ^ bs[i] ^ x.bs[i];

    if ((bs[i] & x.bs[i]) or ((bs[i] | x.bs[i]) & carry[i])) {
      if (i < WSIZE - 1) {
        carry[i + 1] = 1;
      }
    }
  }

  return big_int(res);
}

big_int big_int::operator-(const big_int x) const { return ((*this) + (-x)); }

big_int big_int::operator*(const big_int x) const {
  big_int res;

  for (int i = 0; i < WSIZE; i++) {
    if (x.bs[i]) {
      res = res + (big_int((*this) << i));
    }
  }

  return res;
}

ostream &operator<<(ostream &out, const big_int &bi) {
  for (int i = PSIZE - 1; i >= 0; i--) {
    if ((bi & (big_int(1) << i)) != big_int(0))
      out << 1;
    else
      out << 0;
    if ((PSIZE - 1 - i) % PINTERV == PINTERV - 1) {
      out << " ";
    }
  }

  return out;
}
