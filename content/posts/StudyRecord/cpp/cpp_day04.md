---
title: "赋值操作符函数、特殊类对象处理"
date: 2026-08-12
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---
# 赋值操作符函数、特殊类对象处理

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
有四类比较特殊的类对象需要单独处理，分别是类内常量数据成员、类内引用数据成员、类内类对象数据成员和静态类内数据成员。

### 类内常量数据成员
const 修饰的对象，无论在类内还是类外，都需要在声明时候就直接赋值。在类内做数据成员时，需要在构造函数的初始化列表中进行初始化，而不能在构造函数体内进行。

```c++
#include <iostream>

using std::cout;
using std::endl;

class Point
{
public:
    Point(int ix = 0, int iy = 0)
    : _ix(ix)
    , _iy(iy)
    {
        cout << "Point(int = 0, int = 0)" << endl;
        /* _ix = ix;//error,赋值 */
        /* _iy = iy; */
    }

    void print()
    {
        cout << "(" << _ix << ", "
              << _iy << ")" <<  endl;
    }

    ~Point()
    {
        cout << "~Point()" << endl;
    }
private:
    const int _ix;//常量数据成员,必须在初始化列表中进行
    const int _iy;
};


```


### 类内引用数据成员
同理，由于引用也必须在声明时初始化，也需要在初始化列表中进行初始化。
```c++
#include <iostream>

using std::cout;
using std::endl;

class Point
{
public:
    Point(int ix = 0, int iy = 0)
    : _ix(ix)
    , _iy(iy)
    , _ref(_ix)
    {
        cout << "Point(int = 0, int = 0)" << endl;
        /* _ix = ix;//error,赋值 */
        /* _iy = iy; */
    }

    void print()
    {
        cout << "(" << _ix << ", "
              << _iy << ")" <<  endl;
    }

    ~Point()
    {
        cout << "~Point()" << endl;
    }
private:
    int _ix;
    int _iy;
    int &_ref;
};

//成员函数不占用类的大小，成员函数存在于程序代码区，被该类的
//左右对象共享
//
int main(int argc, char **argv)
{
    cout << "sizeof(Point) = " << sizeof(Point) << endl;

    Point pt(1, 2);//栈对象
    return 0;
}
```
> 辨析：引用数据类型是否占用内存空间？

通过`sizeof(Point)` 我们可以测出，只有两个 `int` 的类大小和多了一个引用变量的类大小：

```c++
// 没有引用变量_ref
sizeof(Point) = 8

// 有引用变量_ref
sizeof(Point) = 16
```
测试结果表明，类内引用类型的数据成员变量占用了内存空间，而且是8个字节大小的指针；且可以说明，函数不占类的空间。进一步的，函数是存在`程序段`中的。


结论：
C++ 标准（[dcl.ref]）原文：
> It is unspecified whether or not a reference requires storage.

也就是说，引用是否占用存储是"未指定"的——编译器在不需要存储时可以不分配，在必须时则分配。

<mark>实验正确证明了"引用作为类成员时按指针大小占 8 字节"，但这属于标准允许的"实现需要存储"的情形；局部引用和参数引用在底层常被消除或仅暂存于寄存器，所以不能笼统说"引用一定占内存"。</mark>
### 类内类对象数据成员
类内对象在被初始化时，要注意会调用其自身的构造函数，资源回收时，会调用自身的析构函数。

```c++
#include <iostream>

using std::cout;
using std::endl;

class Point
{
public:
    /* Point(int ix = 0, int iy = 0) */
    Point(int ix, int iy)
    : _ix(ix)
    , _iy(iy)
    {
        cout << "Point(int = 0, int = 0)" << endl;
    }

    void print()
    {
        cout << "(" << _ix << ", "
              << _iy << ")" <<  endl;
    }

    ~Point()
    {
        cout << "~Point()" << endl;
    }
private:
    int _ix;
    int _iy;
};

class Line
{
public:
    Line(int x1, int y1, int x2, int y2)
    : _pt1(x1, y1)//类对象成员需要显示进行初始化，否则就是默认值
    , _pt2(x2, y2)
    {
        cout << "Line(int, int, int,int)" << endl;
    }

    void printLine()
    {
        _pt1.print();
        _pt2.print();
    }

    ~Line()
    {
        cout << "~Line()" << endl;
    }

private:
    Point _pt1;//类对象成员(子对象)
    Point _pt2;
};
int main(int argc, char **argv)
{
    Line line(1, 2, 3, 4);
    line.printLine();
    return 0;
}

// 输出如下：
// 两个对象，因此，每个对象被创建时都调用了一次构造函数
Point(int = 0, int = 0)
Point(int = 0, int = 0)
Line(int, int, int,int)
(1, 2)
(3, 4)
~Line()
// 两个对象，因此，每个对象被销毁都调用了一次析构函数
~Point()
~Point()
```

### 类内静态数据成员
类内定义静态成员时，需要在类外全局静态区域进行初始化。该静态数据成员存储在内存的全局静态区，所有的类对象，共享一份静态数据成员变量。
对于头文件和实现文件分开的代码方式，为了避免重定义的问题，全局变量的初始化应放在 `.cc` 的实现文件中，而非头文件中。

<span style="color:red"> 四个特殊数据成员中，唯有静态数据成员不能放在构造函数的初始化列表中初始化</span>

```c++
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
```

关于头文件卫士：
我们经常在头文件中写
```c++
#ifndef __COMPUTER_H__
#define __COMPUTER_H__

// 头文件的内容

#endif
```

这是为了避免在同一个`.cc`文件中多次包含某一个头文件造成重复定义的问题。
我们想要的效果就是每一个包含该头文件的`.cc`文件都包含一次这个头文件，以便可以正常通过编译。
当多个.o文件都有一份定义时候，那么会出现链接错误。

如果你想更仔细的了解，那么这篇文章也许比较适合你：[重定义错误并不“显然”]({{< ref "posts/WeeklySummary/HeaderDefender.md" >}})

