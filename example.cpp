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

  int i1 = my_fusiontree.find_predecessor(3);
  int i2 = my_fusiontree.find_predecessor(9);
  int i3 = my_fusiontree.find_predecessor(0);

  cout << "Fusion Tree example:" << endl;
  cout << i1 << " " << i2 << " " << i3 << endl;
  cout << my_fusiontree.pos(i1).to_int() << " "
       << my_fusiontree.pos(i2).to_int() << endl;
}
