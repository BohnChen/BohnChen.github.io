#include <iostream>
#include <set>
using std::cout;
using std::endl;

struct Cmp {                      // 会"数数"的比较器：每比较一次自增一次
  static long long cnt;
  bool operator()(int a, int b) const {
    ++cnt;
    return a < b;
  }
};
long long Cmp::cnt = 0;

int main() {
  const int N = 1 << 20;          // 1048576，一百万
  std::set<int, Cmp> s;

  // 故意"按升序"插入 —— 对不做旋转的普通二叉搜索树来说，这是最坏情况（会退化成链表）
  for (int i = 0; i < N; ++i) s.insert(i);

  Cmp::cnt = 0;
  s.find(N - 1);                  // 找最后一个元素
  cout << "set 在 size=" << N << " 中 find(" << N - 1
       << ") 用掉比较次数 = " << Cmp::cnt << endl;
  cout << "2*log2(N) = " << 2 * 20 << "（红黑树查找的比较次数理论上限）" << endl;
  return 0;
}
