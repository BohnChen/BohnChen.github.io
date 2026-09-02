#include <iostream>
#include <stdio.h>
#include <string>
#include <unistd.h>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ios;
using std::string;

void test1() {
  string str;
  // 如果没有tie，那么提示信息是不会输出内容的
  cout << "请输入几个字符。";
  cin >> str;
}

void test2() {
  ios::sync_with_stdio(false); // 关键：必须先关同步，断开与 C stdio 的共用
  cin.tie(nullptr);            // 再解绑

  string str;
  // std::ostream *p = std::cin.tie(); //
  // 返回当前关联的输出流指针（默认是&cout）
  // std::cin.tie(&std::cerr); // 改为关联到 cerr（读取 cin 时刷新
  // cerr）
  //  如果没有tie，那么提示信息是不会输出内容的
  cout << "请输入几个字符: ";
  cin >> str;
  std::cout << str << std::endl;
}

void test3() {
  auto stream = cin.tie();
  cout << "stream:" << stream << endl;
  cout << "&cout: " << &cout << endl;

  auto st = 0;
  cin >> st;
  std::cout << st << std::endl;
}

void test4() {
  ios::sync_with_stdio(false); // 全程序第一行，任何 IO 之前
  cin.tie(nullptr);
  cerr.tie(nullptr);

  string s;
  cout << "PROMPT: ";     // 应留在缓冲区，不显示
  cerr << "[读前打点]\n"; // cerr 无缓冲，立刻显示
  cin >> s;               // 光标等输入，此刻看不到 PROMPT:
  cerr << "[读后打点]\n";
  cout << "you typed: " << s << endl;
}

void test5() {
  // 程序结束后，才进行刷新
  cout << "test";
  sleep(5);
}
int main(int argc, char *argv[]) {

  // test1();
  test5();

  return 0;
}
