// atexit 函数用来自动回收资源
#include <cstdlib>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class Singleton {
public:
  static Singleton &getInstance() {
    if (nullptr == _pinstance) {
      _pinstance = new Singleton();
      std::atexit(destroy);
    }
    return *_pinstance;
  }

  void print() { cout << "this is the print function. " << endl; }

  static void destroy() {
    if (nullptr != _pinstance) {
      delete _pinstance;
      _pinstance = nullptr;
    }
  }
  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;

private:
  Singleton() { cout << "This is Singleton(). " << endl; }
  ~Singleton() { cout << "This is ~Singleton(). " << endl; }

  static Singleton *_pinstance;
};

// 所有的变量需要满足ODR原则，即“只能定义一次”
// 所以类中的局部静态变量被解读为声明
// 要使用一个变量，只有声明时，链接器找不到这个符号
// 因此报错
Singleton *Singleton::_pinstance = nullptr;

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  // 删除了拷贝构造
  // ERROR
  // Singleton s1(Singleton::getInstance());

  // 删除了负责运算符
  // ERROR
  // Singleton s2 = Singleton::getInstance();

  Singleton::getInstance().print();
  Singleton::getInstance().print();
  Singleton::getInstance().print();
  return 0;
}
