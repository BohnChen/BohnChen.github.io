// 写法四：C++17 折叠表达式
// 用一行折叠替代写法三的递归，同样类型安全。
// 编译：g++ -std=c++17 variablePara4.cc -o variablePara4 && ./variablePara4
#include <iostream>

// 逗号折叠：依次求值每个 (std::cout << args << " ") 表达式
template <typename... Args> void logPrint(Args... args) {
  ((std::cout << args << " "), ...);
  std::cout << std::endl;
}

// 也可以对"所有参数"做二元运算折叠
template <typename... Args> auto sumAll(Args... args) {
  return (... + args); // 一元右折叠：args1 + args2 + ...
}

int main() {
  logPrint(1, 2.5, "hello", 'x');
  std::cout << "sumAll(1,2,3,4) = " << sumAll(1, 2, 3, 4) << std::endl;
  return 0;
}
