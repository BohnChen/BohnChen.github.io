#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

class Base {
public:
  Base(int a) : _ia(a) { cout << "Base(int a)" << endl; }

  ~Base() { cout << "~Base()" << endl; }

  virtual void display() {
    std::cout << "Base() :: display(), _ia is " << _ia << std::endl;
  }

private:
  int _ia;
};

class Derive : public Base {
public:
  Derive(int a, const string str) : Base(a), _str(str) {}
  ~Derive() {}

  // 虚函数与其重写的函数只有函数体不一样
  void display() override {
    std::cout << "Derive() :: display(), _str is " << _str << std::endl;
  }

private:
  string _str;
};

void func(Base *pbase) {
  pbase->display();
  pbase->Base::display();
}

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  Base *pbaseptr1 = new Derive(1, "nice");
  Base *pbaseptr2 = new Derive(1, "nice");
  func(pbaseptr1);
  return 0;
}
