// 写法三：C++11 变参模板
// 类型安全：参数类型在编译期检查，写错类型直接编译报错；
// 但不支持 printf 的 %d/%s 格式串，适合"直接拼接任意类型"。
// 编译：g++ -std=c++17 variablePara3.cc -o variablePara3 && ./variablePara3
#include <iostream>

// 递归终止（0 个参数时）
void logPrint() {}

// 递归展开：处理第一个参数，其余交给下一层
template <typename T, typename... Rest> void logPrint(T first, Rest... rest) {
  std::cout << first << " ";
  logPrint(rest...);
}

// sizeof...(Args) 可以在编译期拿到参数个数
template <typename... Args> void logCount(Args... args) {
  std::cout << "参数个数 = " << sizeof...(args) << std::endl;
}

int main() {
  logPrint(1, 2.5, "hello", 'x'); // 任意类型都行
  std::cout << std::endl;

  logCount(1, 2, 3);
  return 0;
}
