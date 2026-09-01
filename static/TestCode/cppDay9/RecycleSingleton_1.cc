#include <iostream>
using std::cout;
using std::endl;

class MySingleton {
public:
  void print() { cout << "This print()" << endl; }

  static MySingleton &getInstance() {
    // 函数内部的 static 是局部变量的定义
    static MySingleton instance; // ① 首次调用构造
    return instance;             // ② 程序退出时自动析构
  }

  MySingleton(const MySingleton &c) = delete;
  MySingleton &operator=(const MySingleton &c) = delete;

private:
  MySingleton() { cout << "this is MySingleton()." << endl; }
  ~MySingleton() { cout << "this is ~MySingleton()." << endl; }
};

int main() {
  MySingleton &ms1 = MySingleton::getInstance();
  MySingleton &ms2 = MySingleton::getInstance();
  MySingleton &ms3 = MySingleton::getInstance();

  ms1.print();
  ms2.print();
  ms3.print();
  return 0; // 无需手动释放，instance 自动析构
}
