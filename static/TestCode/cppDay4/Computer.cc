#include "Computer.h"
#include <cstring>
#include <iostream>
#include <string.h>

using std::cout;
using std::endl;

Computer::Computer(const char *brand, float price)
    : _brand(new char[strlen(brand) + 1]()), _price(price) {
  /* _brand = new char[strlen(brand) + 1](); */
  cout << "Computer(const char *, float)" << endl;
  strcpy(_brand, brand);
}

#if 0
Computer::Computer(const Computer &rhs)
: _brand(rhs._brand)//浅拷贝
, _price(rhs._price)
{
    cout << "Computer(const Computer &)" << endl;
}
#endif

#if 1
// 问题1：拷贝构造函数参数中的引用符号可以去掉吗？
//
// 问题2：拷贝构造函数参数中的const可以去掉吗？
//
//
Computer::Computer(const Computer &rhs)
    : _brand(new char[strlen(rhs._brand) + 1]()) // 深拷贝
      ,
      _price(rhs._price) {
  cout << "Computer(const Computer &)" << endl;
  strcpy(_brand, rhs._brand);
}
#endif
#if 0
//默认赋值运算符不够用
Computer& Computer::operator=(const Computer &rhs)
{
    cout << "Computer &operator=(const Computer &)" << endl;
    _brand = rhs._brand;
    _price = rhs._price;

    return *this;
}
#endif

#if 0
//com2 = com1
Computer& Computer::operator=(const Computer &rhs)
{
    cout << "Computer &operator=(const Computer &)" << endl;
    //if (*this != rhs)
    if(this != &rhs)//1、自复制(考虑自己赋值给自己)
    {
        delete [] _brand;//2、释放左操作数(不释放就会有内存泄漏)
        _brand = nullptr;

        _brand = new char[strlen(rhs._brand) + 1]();//3、深拷贝
        strcpy(_brand, rhs._brand);

        _price = rhs._price;

    }

    return *this;//4、返回*this
}
#endif

Computer &Computer::operator=(const Computer &rhs) {
  cout << "Computer &operator=(const Computer &)" << endl;
#if 0
  // V1:
  // 此时等号左边的对象与右边的对象指向同一个地址
  // ERROR: 每一个对象的_brand存同一个地址，在回收资源时都调用一次析构函数
  // ERROR: 这就对同一个地址free了两次，造成错误
  // 所以当有堆空间时候，不可这么写
  _brand = rhs._brand;
  _price = rhs._price;
#endif

#if 0
  // V2:
  // 原先的深拷贝空间哪去了？初始化时候根据初始化的值申请了一个空间
  // 重新赋值调用此处时，如果旧空间够用，那么可以继续使用旧空间
  strcpy(_brand, rhs._brand);
  _price = rhs._price;
#endif

#if 0
  // V3
  // 如果不够用，为了不被截断，出现意料之外的结果
  // 就需要给_brand重新分配空间
  if (strlen(this->_brand) < strlen(rhs._brand)) {
    delete[] _brand;
    _brand = new char[strlen(rhs._brand) + 1]();
  }
  strcpy(this->_brand, rhs._brand);
  this->_price = rhs._price;
#endif

#if 1

  // V4
  // 但是此时如果出现自复制com1 = com1的话，会出现
  // strcpy(a._brand, a._brand);
  // 源和目标是同一地址
  // strcpy 对重叠内存区域的行为是 未定义行为（C 标准明确禁止 restrict
  // 指针重叠）。同一地址在实现上通常不会出错（从左往右拷贝，覆盖前已读完）
  // 但标准不保证。
  if (this != &rhs) { // 自赋值直接跳过
    if (strlen(this->_brand) < strlen(rhs._brand)) {
      delete[] _brand;
      _brand = new char[strlen(rhs._brand) + 1]();
    }
    strcpy(this->_brand, rhs._brand);
    this->_price = rhs._price;
  }
#endif

  return *this;
}

void Computer::setBrand(const char *brand) {
  strcpy(_brand, brand); // 先不去考虑越界的问题
}

void Computer::setPrice(float price) { _price = price; }

void Computer::print() {
  printf("brand : %p\n", _brand);
  cout << "brand : " << _brand << endl << "price : " << _price << endl;
}

Computer::~Computer() {
  cout << "~Computer()" << endl;
  /* if(_brand != nullptr) */
  // if (_brand) { // 用来防止同一个对象是否被析构两次，编译器自动的，没有必要
  /* cout << "delete [] _brand" << endl; */
  delete[] _brand;
  // _brand = nullptr;
  // }
}
