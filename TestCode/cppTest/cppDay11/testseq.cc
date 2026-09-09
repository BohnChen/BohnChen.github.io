#include <iostream>

using std::cout;
using std::endl;

class Base {
protected:
  int ptest;

public:
  Base(int n) {
    cout << "Constucting base class" << endl;
    _ix = n;
  }

  ~Base() { cout << "Destructing base class" << endl; }

  void showX() { cout << _ix << ","; }

  int getX() { return _ix; }

private:
  int _ix;
};

/*
Constucting base class
Constucting base class
Constructing derived class
13,8,24
Destructing derived class
Destructing base class
Destructing base class
*/

// 验证
/*
Constucting base class
Constucting base class
Constructing derived class
13,8,24
Destructing derived class
Destructing base class
Destructing base class
*/

class Derived : private Base {
public:
  Derived(int n, int m, int p) : Base(m), _base(p) {
    cout << "Constructing derived class" << endl;
    j = n;
  }

  ~Derived() { cout << "Destructing derived class" << endl; }

  void show() {
    Base::showX();
    ptest = 1;
    cout << j << "," << _base.getX() << endl;
  }

private:
  int j;
  Base _base;
};
int main() {
  Derived obj(8, 13, 24);
  obj.show();

  return 0;
}
