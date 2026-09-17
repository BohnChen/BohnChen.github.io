#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class AbstractBase {
public:
  AbstractBase(int a) : _ia(a) { cout << "AbstractBase(int a)" << endl; }
  ~AbstractBase() { cout << "~AbstractBase()" << endl; }

  // 告诉作者，我知道你会用，但是我不知道你会怎么用
  // 所以你来负责具体实现
  // 通过纯虚函数，可以提供一个与派生类一致的接口
  virtual void display() = 0;
  virtual void print() = 0;

private:
  int _ia;
};
class AbstractDerive : public AbstractBase {
public:
  AbstractDerive(int a, int b) : AbstractBase(a), _id(b) {}
  ~AbstractDerive() {}

  void display() { cout << "AbstractDerive() :: display() " << endl; }

private:
  int _id;
};

int main(int argc, char *argv[]) {
  // AbstractBase a(1);
  AbstractDerive b(1, 2);

  return 0;
}
