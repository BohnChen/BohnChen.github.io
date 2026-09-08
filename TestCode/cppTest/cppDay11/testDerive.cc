#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class A {
public:
  A(const int x) : _ix(x) {}
  A() : _ix(0) {}
  ~A() {}

  A(const A &rhs);
  void print() { cout << "_ix is " << _ix << ". " << endl; }

private:
  int _ix;
};

class B : public A {
public:
  B() : _iy(0) {}
  B(const int &x, const int &y) : A(x), _iy(y) {
    cout << "This is B(const int &y)" << endl;
  }

  // 已经显示写出了派生类的拷贝构造函数
  // 当 B b1 = b3;时，调用拷贝构造函数
  // 如果没有显示对基类拷贝构造的调用
  // 那么基类中的对象值会出现问题
  B(const B &rhs) : A(rhs) {
    _iy = rhs._iy;
    cout << "This is B(const B &rhs) . " << endl;
  }

  B &operator=(const B &rhs) {
    if (this != &rhs) {
      // 如果在派生类中缺少了这句
      // 那么基类中的_ix的值永远也不能被赋值
      A::operator=(rhs);
      _iy = rhs._iy;
      cout << "test" << endl;
    }
    return *this;
  }

  ~B() {}

  friend std::ostream &operator<<(std::ostream &os, const B &b);

  void print() { cout << "_iy is " << _iy << ". " << endl; }

private:
  int _iy;
};
std::ostream &operator<<(std::ostream &os, const B &b) {
  os << b._iy;
  return os;
}

A::A(const A &rhs) {
  this->_ix = rhs._ix;
  cout << "This is A(const A &rhs)" << endl;
}

int main(int argc, char *argv[]) {

  B b1(50, 60);
  B b2(70, 80);
  B b3 = b1;
  b1 = b2;
  b1.A::print();
  b1.B::print();
  b2.A::print();
  b2.B::print();
  b3.A::print();
  b3.B::print();
  return 0;
}
