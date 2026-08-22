#include <iostream>
#include <string>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::string;

class Test {
private:
  int _a;
  int _b;

public:
  explicit Test(int a = 0, int b = 0) : _a(a), _b(b) {}
  void print() { std::cout << _a << "  " << _b << std::endl; }
  ~Test() {}
};

void test4() {
  std::ios::sync_with_stdio(false); // 全程序第一行，任何 IO 之前
  cin.tie(nullptr);
  cerr.tie(nullptr);

  string s;
  cout << "PROMPT: ";     // 应留在缓冲区，不显示
  cerr << "[读前打点]\n"; // cerr 无缓冲，立刻显示
  cin >> s;               // 光标等输入，此刻看不到 PROMPT:
  cerr << "[读后打点]\n";
  cout << "you typed: " << s << endl;
}

int main(int argc, char *argv[]) {
  Test test = 5;
  test.print();

  return 0;
}
