// 写法一：C 风格可变参数（stdarg）
// 最接近 printf，支持 %d/%s 等格式串；但类型不检查，个数靠约定。
// 编译：g++ -std=c++17 variablePara1.cc -o variablePara1 && ./variablePara1
#include <cstdarg>
#include <cstdio>

// 例 1：用 vprintf 直接把可变参数格式化输出到屏幕
void logPrint1(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt); // 游标对准第一个可变参数
  vprintf(fmt, args);  // v 系函数直接消费 va_list
  va_end(args);        // 与 va_start 配对
}

// 例 2：用 vsnprintf 把可变参数格式化进缓冲区（供 log4cpp 等库使用）
void logPrint2(char *buf, size_t size, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, size, fmt, args); // 有上限，安全
  va_end(args);
}

// 例 3：不用 v 系函数，用 va_arg 自己逐个取参数（需约定个数与类型）
double average(int count, ...) {
  va_list args;
  va_start(args, count);
  double sum = 0;
  for (int i = 0; i < count; ++i) {
    sum += va_arg(args, double); // 类型必须和实参严格一致
  }
  va_end(args);
  return sum / count;
}

int main() {
  logPrint1("logPrint1: name=%s, id=%d\n", "bohn", 42);

  char buf[256];
  logPrint2(buf, sizeof(buf), "logPrint2: %s %d", "hello", 7);
  std::printf("buf = %s\n", buf);

  std::printf("average(3) = %.2f\n", average(3, 1.5, 2.5, 4.0));
  return 0;
}
