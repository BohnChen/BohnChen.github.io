---
title: "day04"
date: 2026-08-12
draft: false
categories: ["编程语言"]
tags: ["StudyRecord", "c/c++", "技术学习"]
---
# 赋值操作符函数、

## 赋值操作符函数
```c++
// Computer.h
#ifndef __COMPUTER_H__
#define __COMPUTER_H__

class Computer
{
public:
    Computer(const char *brand, float price);
    Computer(const Computer &rhs);
    Computer &operator=(const Computer &rhs);
    void setBrand(const char *brand);
    void setPrice(float price);
    void print();
    ~Computer();

private:
    char *_brand;
    float _price;
};

#endif

```
赋值操作符函数应该怎么写？我们一步一步讨论：

```c++
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

```

同时，关于析构函数中delete的写法，其实，析构调用时，意味着对象马上就没了，所以既不用在同一个对象上避免二次free，delete后也无需赋nullptr。但是这是个好习惯。
```c++
Computer::~Computer() {
  cout << "~Computer()" << endl;
  /* if(_brand != nullptr) */
  // if (_brand) { // 用来防止同一个对象是否被析构两次，编译器自动的，没有必要
  /* cout << "delete [] _brand" << endl; */
  delete[] _brand;
  // _brand = nullptr;
  // }
}

```


