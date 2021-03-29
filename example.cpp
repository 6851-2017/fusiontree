//
// Created by Rogério Aristida Guimarães Junior on 3/28/21.
//

#include <iostream>
#include "big_int.hpp"
#include "fusiontree.hpp"

int main() {
  vector<big_int> small_squares;
  for (int i = 1; i <= 5; i++) {
    small_squares.push_back(i * i);
  }

  environment *env = new environment;
  fusiontree my_fusiontree(small_squares, env);

  int idx1 = my_fusiontree.find_predecessor(3);
  int idx2 = my_fusiontree.find_predecessor(9);
  int idx3 = my_fusiontree.find_predecessor(0);

  cout << "Fusion Tree size:" << endl;
  cout << my_fusiontree.size() << endl;

  cout << "Queried positions:" << endl;
  cout << idx1 << " " << idx2 << " " << idx3 << endl;

  cout << "Queried elements:" << endl;
  cout << (int)my_fusiontree.pos(idx1) << " " << (int)my_fusiontree.pos(idx2)
       << endl;
}
