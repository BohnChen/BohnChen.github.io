#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class MyClass {
public:
  MyClass() : _x(0), _y(0) {}
  ~MyClass() {}

  void print() {
    cout << "_x is " << _x << " " << endl;
    cout << "_y is " << _y << " " << endl;
  }

  MyClass &operator++() {
    cout << "MycClass operator++ " << endl;
    _x++;
    _y++;

    return *this;
  }

  MyClass operator++(int) {
    cout << "MycClass operator++(int) " << endl;
    MyClass tmp(*this);
    _x++;
    _y++;
    // 调用拷贝构造函数
    return tmp;
  }

private:
  int _x;
  int _y;
};

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;

  MyClass c1;
  MyClass c2;

  ++c1;
  MyClass c3 = c2++;

  c1.print();
  c2.print();
  c3.print();
  return 0;
}
