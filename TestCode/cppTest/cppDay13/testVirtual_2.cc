#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class AbstractBase {
protected:
  AbstractBase(int a) : _ia(a) { cout << "AbstractBase(int a)" << endl; }
  ~AbstractBase() { cout << "~AbstractBase()" << endl; }

private:
  int _ia;
};

class AbstractDerive : public AbstractBase {
public:
  // 没问题
  // 当基类构造函数protected时，在派生类的构造中调用，是唯一可以用的方式
  AbstractDerive(int a, int b) : AbstractBase(a), _id(b) {}
  ~AbstractDerive() {}

  // 在类体内部，Type a(...)
  // 会被编译器当作成员函数声明来解析（为了避免与函数声明产生类似 Most Vexing
  // Parse 的二义性，C++ 明确禁止在类内使用圆括号 () 进行成员就地初始化）。
  // AbstractBase a(1);                    // ERROR: 报语法错误
  // AbstractBase b = new AbstractBase(1); // ERROR : 报调用了protected 构造函数
  void display() { cout << "AbstractDerive() :: display() " << endl; }

private:
  int _id;
};

int main(int argc, char *argv[]) {
  // AbstractBase a(1);
  AbstractDerive b(1, 2);

  return 0;
}
