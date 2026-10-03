#include <algorithm>
#include <iostream>
#include <string>

using std::cin;
using std::cout;
using std::endl;
using std::string;

void test() {
  std::cout << "hello world!" << std::endl;
  string str = "nice";
  string str_2 = std::move(str);
  // str 已经被移动了， 所以下面这行什么也不会输出
  cout << str << endl;
  // str_2 实际上已经拿到了原本给 str 的 “nice”
  cout << str_2 << endl;
  // 当然 str 并没有消亡，它只是资源被转移了，若要重新使用，需要重新赋值
  str = "hi";
  cout << str << endl;
}

int main(int argc, char *argv[]) {
  test();
  return 0;
}
