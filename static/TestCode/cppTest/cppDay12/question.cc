#include <map>
#include <set>
#include <unordered_map>

template <typename T>
struct Debug;                       // 只有声明，没有定义：一实例化就报错

int main() {
  std::set<int> s;
  std::map<int, int> m;
  std::unordered_map<int, int> um;

  Debug<decltype(s.begin())> d1;    // 看 set 迭代器的底层类型
  Debug<decltype(m.begin())> d2;    // 看 map 迭代器的底层类型
  Debug<decltype(um.begin())> d3;   // 看 unordered_map 迭代器的底层类型
  (void)d1; (void)d2; (void)d3;     // 防"未使用变量"警告
  return 0;
}
