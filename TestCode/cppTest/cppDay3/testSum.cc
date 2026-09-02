#include <iostream>

using std::cout;
using std::endl;

int i = 1;

class Test {
public:
  Test() : _fourth(_third), _second(i++), _first(i++), _third(i++) {
    _third = i;
  }
  void print() {
    cout << "result : " << _first + _second + _third + _fourth << endl;
    std::cout << "first = " << _first << std::endl;
    std::cout << "second = " << _second << std::endl;
    std::cout << "third = " << _third << std::endl;
    std::cout << "fourth = " << _fourth << std::endl;
  }

private:
  int _first;
  int _second;
  int _third;
  int &_fourth; // 注意：与前面学的引用类比即可
};

int main() {
  Test test;
  test.print();

  return 0;
}
