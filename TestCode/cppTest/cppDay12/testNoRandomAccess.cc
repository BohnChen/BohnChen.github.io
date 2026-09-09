#include <set>

int main() {
  std::set<int> s;
  auto it = s.begin();
  auto x = it + 2;      // ❌ 编译错误：不能给 set 的迭代器加整数
  (void)x;
  return 0;
}
