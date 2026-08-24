#include <iostream>

using std::cout;
using std::endl;

class MyClass {
public:
  MyClass() : _x(0), _y(0) {}
  MyClass(int a, int b) : _x(a), _y(b) {}
  ~MyClass() {}

  int getx() const { return _x; }
  int gety() const { return _y; }

  void setx(int x) { _x = x; }
  void sety(int y) { _y = y; }

  void print() const {
    cout << "_x is " << _x << ", _y is " << _y << ". " << endl;
  }

private:
  int _x;
  int _y;
};

MyClass operator+(const MyClass &a, const MyClass &b) {
  MyClass tmp;
  tmp.setx(a.getx() + b.getx());
  tmp.sety(a.gety() + b.gety());
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

  return 0;
}
