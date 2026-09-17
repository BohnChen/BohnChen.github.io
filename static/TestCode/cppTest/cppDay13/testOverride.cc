#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class Base {
public:
  Base(int a) : _ia(a) { cout << "Base()" << endl; }
  ~Base() {}

  void print() {
    cout << "Base::print()" << endl;
    _ia = 2;
  }

private:
  int _ia;
};

class Derive : public Base {
public:
  Derive(int a, int b) : Base(a), _id(b) {
    cout << "Derive(int a, int b)" << endl;
  }
  ~Derive() {}

  void print() { cout << "Derive::print(int a), _id = " << _id << endl; }

private:
  int _id;
};

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  Derive d(1, 2);
  d.print();
  d.Base::print();
  return 0;
}
