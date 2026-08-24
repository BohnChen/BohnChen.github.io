#include <iostream>

using std::cout;
using std::endl;

class MyClass {
public:
  MyClass() : _x(0), _y(0) {}
  MyClass(int a, int b) : _x(a), _y(b) {}
  ~MyClass() {}

  friend MyClass operator+(const MyClass &a, const MyClass &b);

  MyClass &operator*=(const MyClass &rhs);

  void print() const {
    cout << "_x is " << _x << ", _y is " << _y << ". " << endl;
  }

private:
  int _x;
  int _y;
};
MyClass &MyClass::operator*=(const MyClass &rhs) {
  this->_x *= rhs._x;
  this->_y *= rhs._y;
  return *this;
}

MyClass operator+(const MyClass &a, const MyClass &b) {
  MyClass tmp;
  tmp._x = a._x + b._x;
  tmp._y = a._y + b._y;
  return tmp;
}

int main(int argc, char *argv[]) {
  cout << "Today is a nice day." << endl;

  MyClass c1(1, 1);
  MyClass c2(2, 2);
  MyClass c3 = c1 + c2;
  c1.print();
  c2.print();
  c3.print();
  c2 *= c3;
  c2.print();

  return 0;
}
