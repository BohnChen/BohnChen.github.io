#include <iostream>
#include <stdlib.h>
#include <string.h>
namespace nice {
namespace nice {
void test() { std::cout << "This is nice::nice::test()" << std::endl; }
} // namespace nice
} // namespace nice

int print = 10;
int ar = 10;

namespace OverWrite {
int print = 20;
void test(int print = 1) {
  std::cout << "形式参数 print = " << print << std::endl;
  std::cout << "命名空间内的 print = " << OverWrite::print << std::endl;
  std::cout << "全局变量 print = " << ::print << std::endl;
}

} // namespace OverWrite

void CPointerUse() {
  // 1. 申请堆空间
  int *heapA = (int *)malloc(sizeof(int));
  // 2. 初始化
  memset(heapA, 0, 1);
  // 3. 使用
  *heapA = 10;
  std::cout << "heapA = " << *heapA << std::endl;
  // 4. 释放堆空间
  free(heapA);
  // 5. 防止悬垂指针
  heapA = NULL;
}

void CppPointerUse() {
  // c++
  // 1. 同时进行了初始化和赋值
  int *heapB = new int(0);
  std::cout << "heapB = " << *heapB << std::endl;
  // 2. 使用
  *heapB = 20;
  // 3. 释放
  delete heapB;
  // 4. 防止悬垂指针
  heapB = nullptr;
}

int main(int argc, char *argv[]) {
  // new/delete与malloc/free的区别是什么?
  CPointerUse();
  CppPointerUse();

  nice::nice::test();
  OverWrite::test();
  return 0;
}
