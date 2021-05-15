# Fusion Tree
Fusion Tree implementation in C++

This repository contains an implementation of a Fusion
Tree that can perform predecessor queries using a 
constant number of arithmetic operations of a Big 
Integer class.

In this README file, we will explain the public 
functions and operators needed from the big integer 
class, in case the reader wants to use another or 
implement her own. Then we will how to declare an 
environment and a fusion tree and explain explain the 
public methods available in our fusion tree, as well 
as show a code example. A video demo that explains 
this documentation and shows how to use this code
can be found on [YouTube](https://youtu.be/pnl-vN4RLrM).

Besides this file and the demo, there is also a 
complete report in this repository, 
[report.pdf](report.pdf), with not only this 
documentation, but also a brief review of fusion trees 
and more detailed explanation of the implementation of
the methods here presented and design choices. The source
code in [fusiontree.hpp](fusiontree.hpp) and 
[fusiontree.cpp](fusiontree.cpp) is also extensively 
commented.

## Big Integer

This subsection provides documentation to the 
```big_int``` class that is imported by the 
```fusiontree``` class. The following public methods 
signatures can be found in the file 
[big_int.hpp](big_int.hpp) and 
are used by our ```fusiontree``` class. In case the 
reader wants to use his own big integer class, she 
needs to provide all these public methods and 
operators in a class with the same name. The other 
public methods and operators present in ```big_int``` 
are not necessary for ```fusiontree```.

```C++
big_int(int x = 0);
```
Class constructor. Takes an ```int x``` as input 
(default is zero) and returns a ```big_int``` with the
value of ```x```.

```C++
operator int() const;
```
Returns an ```int``` with the value of the 
```big_int``` if it lies between the value bounds of 
an ```int```.

All the following operators must perform the same 
operation they do in a standard C++ ```int```, 
which can be found on https://www.cplusplus.com/doc/tutorial/operators/.

```C++
big_int operator~() const;

bool operator<(const big_int x) const;

bool operator>(const big_int x) const;

bool operator==(const big_int x) const;

bool operator!=(const big_int x) const;

big_int operator<<(const int x) const;

big_int operator>>(const int x) const;

big_int operator|(const big_int x) const;

big_int operator&(const big_int x) const;

big_int operator^(const big_int x) const;

big_int operator-(const big_int x) const;

big_int operator*(const big_int x) const;
```

## Environment

The ```environment``` class is very simple to use and 
is defined in the file 
[fusiontree.hpp](fusiontree.hpp). 
An instance of an environment is used to inform the 
fusion tree of some constants of the computational 
environment, as well as pre-calculate and keep many 
constants that depend only on the computational 
environment and that are used by the ```fusiontree``` 
class in its operations. The idea is that a B-Tree 
should have a single environment instance and pass it 
to all of its fusion trees. The only public function 
of ```environment``` that the user must use is the 
constructor, which can be seen below:

```C++
environment(int word_size_ = 4000, int element_size_ = 3136, int capacity_ = 5);
```
* ```wordsize_```: The maximum size (in bits) of the 
```big_int``` class, which should be the word size 
assumed for the computer. Must be greater or equal to
```(capacity_)^5+(capacity_^4```, so that the process 
of finding the *em mask m* can be done without 
extrapolating the number of bits in a word.

* ```element_size_```: the size (in bits) of the 
  maximum element that can be stored in the fusion 
  tree. Must be a perfect square, which helps 
  implementing the *most significant bit* operation. 
  Must also be greater or equal to 
  ```(capacity_)^5```, which allows that the 
  *approximately sketched* version of all the elements
  stored in the tree fit in a single ```big_int```.

* ```capacity_```: The maximum number of elements that
  can be stored in a fusion tree. Thus, it is also the
  branching factor of a B-Tree that uses fusion trees 
  as nodes.

The default arguments given in the environment allow 
for a fusion tree that can correctly function and 
store up to 5 elements. The following command will 
use the ```environment``` constructor to create a 
pointer to an environment with the arguments passed.

```C++
environment *env = new environment(word_size, element_size, capacity);
```

## Fusion Tree

The ```fusiontree``` class is defined in the file
[fusiontree.hpp](fusiontree.hpp) 
and is very simple to use. A ```fusiontree``` class 
has the following public methods that can be called by
the user:

```C++
fusiontree(vector<big_int> &v_, environment *my_env_);
```
Class **constructor**. Takes as input a reference 
to a ```vector<big_int> &v_``` and a pointer to an 
```environment *my_env_``` and returns an instance of 
a ```fusiontree``` with the characteristics defined by
`````*my_env_````` that contains the elements stored 
in ```v_```. The length of ```v_``` cannot exceed the 
capacity with which `````*my_env_````` was initialized.

```C++
const int size() const;
```
Returns the size of the ```fusiontree``` instance, 
*i.e.*, the number of elements stored in it, in 
constant time.

```C++
const big_int pos(int i) const;
```
Returns the element that occupies position ```i``` in 
the ```fusiontree``` instance, in increasing order and
starting from zero, in constant time.

```C++
const int find_predecessor(const big_int &x) const;
```
Returns the position of the largest element in the 
fusion tree that is not larger than ```x```, or 
```-1``` if there is no such element.

Uses a constant number of basic arithmetic operations 
on instances of ```big_int``` to answer the 
predecessor query. Therefore, its time complexity 
depends on the complexity of ```big_int``` arithmetic
operators, and it will take constant time if these 
operators also take constant time.

Notice that the ```fusiontree``` is a static, 
immutable data type.

## Make File

In order to use the classes presented in a program, 
the user must compile the code using the 
[Makefile](Makefile) 
script that is present in the repository. 
The user must first create a new C++ file in a 
directory with the files
[big_int.hpp](big_int.hpp), 
[big_int.cpp](big_int.cpp),
[fusiontree.hpp](fusiontree.hpp),
[fusiontree.cpp](fusiontree.cpp), and
[Makefile](Makefile). 
This file must import
[big_int.hpp](big_int.hpp) and
[fusiontree.hpp](fusiontree.hpp) 
and have a ```main``` function, as shown below:

```C++
#include "big_int.hpp"
#include "fusiontree.hpp"

int main() {
  // your code here...
}
```

Then, the user must open the terminal in the directory
of these files and execute the
[Makefile](Makefile)
script. In a MacOS environment, this is simply typing
the following command in the terminal:

```shell
$ make
```

This command will create all the necessary files for 
the compilation and execution of the user program. 
Among the generated files there will be an executable 
called ```main.exe```, which runs the ```main``` 
function in the user code. To execute it, the user 
must type in the terminal the following command:
```shell
$ ./main.exe
```

The
[Makefile](Makefile) script also offers two other commands
that can be useful:
```shell
$ make clean
```
Removes all the files generated by a previous 
compilation through a ```make``` command.

```shell
$ make format
```
Formats all source code according to 
[Google's format for C++](https://google.github.io/styleguide/cppguide.html).


## Example

Although its implementation can be quite complicated, 
using it is fairly simple and relies only on its four
public methods. In our repository, the file 
[example.cpp](example.cpp) can be found and contains a
brief example of how to construct and query a 
```fusiontree```. Its main function can be seen below:

```C++
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
  cout << (int)my_fusiontree.pos(idx1) << " "
       << (int)my_fusiontree.pos(idx2) << endl;
}
```

When we compile and run the code in ```example.cpp``` 
as described in the Make File section, we should 
obtain the following output:

```
Fusion Tree size:
5
Queried positions:
0 2 -1
Queried elements:
1 9
```

A brief explanation of the example code and its output
can be seen below:

In lines 10-13 of our example code, we are 
simply declaring a vector ```small_squares``` of 
```big_int``` which contains the first five positive 
squares, *i.e.*, ```[1, 4, 9, 16, 25]```.

```C++
vector<big_int> small_squares;
for (int i = 1; i <= 5; i++) {
  small_squares.push_back(i * i);
}
```

Then, in line 15  we are declaring a pointer to an 
```environment``` which is initialized with its 
default arguments, *i.e.*, those of a fusion tree that
can store up to five elements.

```C++
environment *env = new environment;
```

In line 16, we use the ```fusiontree``` constructor to
create an instance of a ```fusiontree``` using the 
environment constants defined by `````*env````` that 
contains the elements in ```small_squares```.

```C++
fusiontree my_fusiontree(small_squares, env);
```

In lines 18-20, we query the ```fusiontree``` for the 
predecessors of 3, 9, and 0, respectively.

```C++
int idx1 = my_fusiontree.find_predecessor(3);
int idx2 = my_fusiontree.find_predecessor(9);
int idx3 = my_fusiontree.find_predecessor(0);
```

In lines 22-23, we print the size of the 
```fusiontree``` instance, which is 5.

```C++
cout << "Fusion Tree size:" << endl;
cout << my_fusiontree.size() << endl;
```

In lines 25-26, we print the positions of the 
predecessor of 3, 9, and 0, respectively, which are 0,
2, and -1 (since no element in the ```fusiontree``` 
is smaller or equal to zero).

```C++
cout << "Queried positions:" << endl;
cout << idx1 << " " << idx2 << " " << idx3 << endl;
```

Then, in lines 28-30, we print the elements in the 
valid positions 0, and 2, which are 1 and 9.

```C++
cout << "Queried elements:" << endl;
cout << (int)my_fusiontree.pos(idx1) << " "
     << (int)my_fusiontree.pos(idx2) << endl;
```

## Contributing
Pull requests are welcome. For major changes, please 
open an issue first to discuss what you would like to 
change.

## License
[MIT](https://choosealicense.com/licenses/mit/)
