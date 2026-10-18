#include <iostream>

void func() {
  static int a = 0;
  ++a;
  std::cout << "a = " << a << std::endl;
}

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  func();
  // 局部静态变量的作用域在大括号内
  // 但是每次调用其值会累计
  // a++; // ERROR
  func();
  return 0;
}
