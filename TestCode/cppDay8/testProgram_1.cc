#include <iostream>

// 1、问题描述，编写Base类使下列代码输出为1
using std::cin;
using std::cout;
using std::endl;

class Base {
public:
  Base(int x) : _x(x) {}
  ~Base() {}

  friend int operator+(const Base &x, const Base &y);

private:
  int _x;
};

int operator+(const Base &x, const Base &y) { return y._x - x._x; }

int main(int argc, char *argv[]) {
  int i = 2;
  int j = 7;
  Base x(i);
  Base y(j);
  cout << (x + y == j - i) << endl;
  return 0;
}
