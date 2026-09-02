#include <iostream>

class Box {
public:
  Box() { std::cout << "Box()" << std::endl; };
  ~Box() { std::cout << "~Box()" << std::endl; };
};

int main(int argc, char *argv[]) {
  Box A, B, C;
  return 0;
}
