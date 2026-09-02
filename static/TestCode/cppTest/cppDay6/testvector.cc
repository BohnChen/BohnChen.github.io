#include <iostream>
#include <vector>

using std::vector;

void printVec(const vector<int> &vec) {
  std::cout << "the size of the vec is " << vec.size() << std::endl;
  std::cout << "the capacity of the vec is " << vec.capacity() << std::endl;
}

int main(int argc, char *argv[]) {
  vector<int> ivec;

  for (int i = 0; i < 9; i++) {
    ivec.push_back(i);
    printVec(ivec);
  }

  return 0;
}
