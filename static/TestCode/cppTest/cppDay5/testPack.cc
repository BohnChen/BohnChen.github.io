#include <iostream>
// #pragma pack(4)

using std::cout;
using std::endl;

struct TestStruct_1 {
  int a1;
  char b1;
  short c1;
};

struct TestStruct_2 {
  int a1;
  // int a2;
  struct TestStruct_3 {
    double d1;
    char c1;
    short s1;
  } InsideS, b; // 至少要加上一个实例, 否则是一个声明
} MyStruct;

void Test_1() {
  cout << "The size of the TestStrcutn is " << sizeof(TestStruct_1) << endl;
}
void Test_2() {
  // 如果是 struct T1{ int a1; strcut T2{double d1; char c1; short s1;}; }
  // 输出的大小将是 4，只有一个int 大小，因为内部的struct只是一个声明，不占空间
  // cout << "The size of the TestStrcutn is " << sizeof(TestStruct_2) << endl;
  // 只有当内部的struct 的;前有名字后，内部struct才开始占用空间
  cout << "The size of the TestStrcutn is " << sizeof(MyStruct) << endl;
}

int main(int argc, char *argv[]) {
  // Test_1();
  Test_2();
  return 0;
}
