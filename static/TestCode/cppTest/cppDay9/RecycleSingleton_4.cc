// 使用友元类进行单例资源的自动回收
#include <iostream>

using std::cout;
using std::endl;

class Singleton {
public:
  static Singleton *getInstance() {
    if (nullptr == _pinstance) {
      _pinstance = new Singleton();
    }
    return _pinstance;
  }

  void print() { cout << "this is the print function. " << endl; }

  Singleton(const Singleton &) = delete;
  Singleton &operator=(const Singleton &) = delete;

private:
  Singleton() { cout << "This is Singleton(). " << endl; }
  ~Singleton() { cout << "This is ~Singleton(). " << endl; }

  static Singleton *_pinstance;

  // 嵌套类声明
  friend class Recycle;
};

class Recycle {
public:
  Recycle() {}
  ~Recycle() {
    if (nullptr != Singleton::_pinstance) {
      delete Singleton::_pinstance;
      Singleton::_pinstance = nullptr;
    }
  }
};

// 与 嵌套类 不同，我们需要将这个静态成员
// 声明为全局的
// 必须有一个静态成员对象
// 因为我们企图在回收这个静态成员时
// 自动回收单例资源
static Recycle rec;

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

  // 返回指针，比返回引用多了一次拷贝动作
  Singleton *ps1 = Singleton::getInstance();
  Singleton *ps2 = Singleton::getInstance();
  Singleton *ps3 = Singleton::getInstance();

  ps1->print();
  ps2->print();
  ps3->print();

  return 0;
}
