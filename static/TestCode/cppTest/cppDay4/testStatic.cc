#include <cstring>
#include <iomanip>
#include <iostream>
#include <stdlib.h>

class TestClass {
  // 不占类空间，直接放在全局静态区被类对象共享
  static float _totalPrice;
  char *_brand;
  float _price;

public:
  TestClass(char *brand, float price)
      : _brand(new char[strlen(brand) + 1]()), _price(price)
  /*,_totalPrice(0.0)*/
  {}
  ~TestClass() {}

  void caculateSum() { _totalPrice += _price; }
  void printSum() {
    std::cout << std::fixed << std::setprecision(2) << _totalPrice << std::endl;
  }
};

// 这里不能加 `static` 修饰符
float TestClass::_totalPrice = 0;

int main() {
  TestClass tc1("mac", 12000);
  tc1.caculateSum();
  tc1.printSum();
  TestClass tc2("huawei", 1200000);
  tc2.caculateSum();
  tc2.printSum();
  // 由于内存对齐机制，大小为 16; 将 float 换成 double 也是 16
  std::cout << "the size of TestClass is " << sizeof(TestClass) << std::endl;
  return 0;
}
