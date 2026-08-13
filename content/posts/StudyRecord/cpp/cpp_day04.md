---
title: "day04"
date: 2026-08-12
draft: false
categories: ["编程语言"]
tags: ["StudyRecord", "c/c++", "技术学习"]
---
# 赋值操作符函数

## 赋值操作符函数的正确写法
[Computer.cc](/TestCode/cppDay4/Computer.cc)

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
我们一步一步讨论赋值操作符函数的写法：

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

> 同时，关于析构函数中 `delete` 的写法，其实，析构调用时，意味着对象马上就没了，所以既不用在同一个对象上判空避免二次 `free`，`delete` 后也无需赋 `nullptr`。但是这是个好习惯。
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

## 赋值操作符函数三问
[Point2.cc](/TestCode/cppDay4/Point2.cc)
```c++
// 赋值操作符函数实际写法
Point &operator=(const Point &rhs)
{
    cout << "Point &operator=(const Point &)" << endl;
    _ix = rhs._ix;//赋值
    _iy = rhs._iy;

    return *this;
}



```
### 赋值构造函数参数中的 & 能被去掉吗？

```c++
//  默认情况下编译器会自动生成赋值运算符函数
// pt2 = pt1;
// Q：引用符号可以去掉吗？

// 如果去掉，会变为如下
Point &operator=(const Point rhs)
{
    cout << "Point &operator=(const Point &)" << endl;
    _ix = rhs._ix;//赋值
    _iy = rhs._iy;

    return *this;
}

Point P1;
Point P2;
// 此时调用赋值运算符函数
P2 = P1;
// 调用赋值运算符函数，由于参数值传递，将 P1 值赋值给 rhs
// 此时，const Point rhs = P1; 
// 用已经存在的对象给新对象赋值，触发拷贝构造函数调用
// 拷贝构造函数
Point(const Point &rhs)//Point pt2 = pt1
: _ix(rhs._ix)
, _iy(rhs._iy)
{
    cout << "Point(const Point &)" << endl;
}

// 拷贝构造函数参数为 const Point& rhs = P1 
// 拷贝构造函数将引用 P1 的 rhs 的值赋值给了赋值运算符参数中的非引用 rhs


// 答：如果去掉，会多执行一次拷贝构造函数，效率就会降低
```

### 赋值构造函数的参数中的 const 能否被去掉呢？

```c++
// Q:const可以去掉吗：

// 如果去掉，赋值运算符函数就变成了:
Point &operator=(Point &rhs)
{
    cout << "Point &operator=(const Point &)" << endl;
    _ix = rhs._ix;//赋值
    _iy = rhs._iy;

    return *this;
}

Point func() {
  Point p3;
  return p3;
}

void test5() {
  Point p4;
  // 赋值构造函数如果没有 const，p4 将变成非 const，
  // 无法绑定到临时对象这种右值
  p4 = func(); ERROR: "No viable overloaded '=' "
}

// 答：如果去掉 const，右操作数是右值的时候，就会产生非 const 左值引用不能绑定到右值的报错
```

### 赋值构造函数的返回类型能否是 void？

```c++
// Q:赋值运算符函数的返回类型可以是void？

// 如果去掉，赋值运算符函数就变成了:
void operator=(const Point &rhs)
{
    cout << "Point &operator=(const Point &)" << endl;
    _ix = rhs._ix;//赋值
    _iy = rhs._iy;

    return *this;
}

Point p5;
Point p6;
Point p7;
p5 = p6 = p7;
// 语句会有语法错误
// 先执行 p6 = p7 没有问题，然后返回了 void 类型
// 此时将执行 p5 = void 类型的值将出现问题
// A:考虑连等情况
```

## 类内特殊成员处理

## 类内常量数据成员

## 类内引用数据成员

## 类内类对象数据成员


## 类内静态数据成员
