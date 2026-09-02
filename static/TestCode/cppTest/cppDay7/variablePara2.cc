// 写法二：变参宏（__VA_ARGS__）
// 宏最大的价值：能在"调用点"展开 __FILE__/__LINE__/__FUNCTION__，
// 这是普通函数做不到的（函数内部展开会指向库文件）。
// 编译：g++ -std=c++17 variablePara2.cc -o variablePara2 && ./variablePara2
#include <cstdarg>
#include <cstdio>

// 例 1：最简单的转发宏，把可变参数原样交给 printf
#define MY_PRINT(...) std::printf(__VA_ARGS__)

// 例 2：带位置信息的宏（作业加分项的思路）
// 注意：位置参数必须排在用户参数之前，与格式串开头的 %s %d %s 一一对应；
//       ##__VA_ARGS__ 允许调用时"不带额外参数"。
#define LOG(fmt, ...)                                                          \
  std::printf("[%s:%d] %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__,       \
              ##__VA_ARGS__)

// 例 3：配合 C 风格变参函数使用（宏负责转发）
void logPrint(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}
#define LOG_PRINT(...) logPrint(__VA_ARGS__)

int main() {
  MY_PRINT("MY_PRINT: %d %s\n", 1, "x");

  LOG("value = %d", 42);
  LOG("no extra args"); // 无额外参数，##__VA_ARGS__ 起作用

  LOG_PRINT("LOG_PRINT: %s %d\n", "ok", 2);
  return 0;
}
