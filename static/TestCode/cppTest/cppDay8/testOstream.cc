#include <iostream>
#include <ostream>

using std::cout;
using std::endl;

class MyClass {
public:
  MyClass() : _x(0), _y(0) {}
  MyClass(int a, int b) : _x(a), _y(b) {}
  ~MyClass() {}

  friend std::ostream &operator<<(std::ostream &os, const MyClass &rsh);
  // ERROR
  // std::ostream& operator<<(/*param1: 流，param2:对象*/);
private:
  int _x;
  int _y;
};

std::ostream &operator<<(std::ostream &os, const MyClass &rhs) {
  os << "x = " << rhs._x << " , y = " << rhs._y << ". " << endl;
  return os;
}

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  MyClass c1(1, 2);
  cout << c1;
  operator<<(std::cout, c1);

  return 0;
}
