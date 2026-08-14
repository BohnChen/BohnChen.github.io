---
title: "赋值操作符函数、特殊类对象处理"
date: 2026-08-12
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---
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


## 作业

一、简答题

1. 当定义类时，编译器会为类自动生成哪些函数？这些函数各自都有什么特点？
    1. 无参构造函数
      + 自动为类中的成员进行初始化，并且默认初始化为0
      + 一旦显示的写出了构造函数，此构造函数将不再被调用
    
    2. 拷贝构造函数
      + 在对类对象进行拷贝时，会调用此构造函数，被调用有三个时机
        + 一个已有的类对象赋值给一个新的类对象时调用。
        + 类对象作为参数进行传递，实参和形参结合时调用。
        + 返回类型是类类型时，调用拷贝构造函数。
      + 默认的调用是浅拷贝，两个对象指向的是同一片内存空间，当有占用堆上空间的数据成员，发生过已有对象给新对象初始化的情况，当资源回收，两个对象先后调用析构函数时，会出现`double-free`的严重错误。
      + 一般情况下需要自己重新写拷贝构造函数，为在堆上的数据成员重新申请空间，再进行值的复制，以完成拷贝。

    1. 赋值运算符函数
      + 其实是一个赋值运算符的重载，等号两边是运算符重载函数的两个参数。
      + 需要考虑自复制的情况，因此写此函数需要判断`if(*this != rhs)`
      + 其返回类型不能是void；因为需要考虑 `a = b = c;`
      + 这个函数的参数是`(const ClassName &rhs)`，`&`如果去掉，会先进行一次值传递，然后调用了参数仍然为`(const ClassName &rhs)`的拷贝构造函数，多了一次调用；因此，参数中的`&`不能去掉。
      + 如果去掉`const`，那么它将不能接收右值。
    
    2. 析构函数
      + 用于回收对象的资源
      + `new`出的对象还要手动`delete`来主动调用析构函数回收资源，其他的资源在`return`时被调用回收

2. 什么是左值与右值，拷贝构造函数中的引用与const为什么不能去掉？
    + 左值和右值的区别是左值可以进行取地址，但是右值不可以取地址。常见的右值有临时对象，自面值，`lamda` 表达式等
    
    + 拷贝构造函数中的`const`不能去掉。原因是如果去掉，新定义的对象将无法接收一个本该被接受的右值，比如 `Point p1 = func();`，其中`func()`的返回类型是`Point`。


3. this指针是什么?
    + `this` 指针指向是一个指向类对象本身的指针，其实它隐藏在非静态成员函数的第一个参数位置。


1. 必须在构造函数列表中初始化的3种情况?
    - 类中的 const 修饰的常量数据成员；因为 const 必须定义即初始化。
    - 类中的 & 引用类型成员的声明；同理，引用也必须定义即初始化
    - 类中的类对象成员函数；声明类对象会调用构造函数，销毁时调用析构函数


二、写出下面程序结果。
1、写出以下程序运行的结果。


***// 由于Distance 构造函数中不是引用传参，又调用了 Point 的拷贝构造函数***

```c++
// 首先，main 创建 Point 对象被调用两次
point构造函数被调用
point构造函数被调用
// Distance 类创建的两个 Point 对象在初始化时，调用两次拷贝构造
X = 1 Y=1Point拷贝构造函数被调用
X = 4 Y=5Point拷贝构造函数被调用

***// 由于Distance 构造函数中不是引用传参，又调用了 Point 的拷贝构造函数***
X = 1 Y=1Point拷贝构造函数被调用
X = 4 Y=5Point拷贝构造函数被调用

Distance构造函数被调用
The distance is:5
```

```C++
#include <math.h>
#include <iostream>

using std::endl;
using std::endl;

class Point	
{
public:
    Point(int xx = 0, int yy = 0) 
	{
		X = xx;
		Y = yy;
		cout << "point构造函数被调用" << endl;
	}
	

   	Point(Point &p);
   	
   	int GetX() 
   	{
   		return X;
   	}
   	
   	int GetY() 
   	{
   		return Y;
   	}

private:
	int X,Y;
};

Point::Point(Point &p)	
{
	X = p.X;
	Y = p.Y;
	cout << "X = " << X << " Y=" << Y << "Point拷贝构造函数被调用" << endl;
}

class Distance	
{
public:	
	Distance(Point xp1, Point xp2);
	double GetDis()
	{
		return dist;
	}
private:	
	Point p1,p2;	
	double dist;	
};

Distance::Distance(Point xp1, Point xp2)
: p1(xp1)
,p2(xp2)
{
	cout << "Distance构造函数被调用" << endl;
	double x = double(p1.GetX() - p2.GetX());
	double y = double(p1.GetY() - p2.GetY());
	dist = sqrt(x * x + y * y);
}

int main()
{
	Point myp1(1,1), myp2(4,5);
	Distance myd(myp1, myp2);
	cout << "The distance is:" ;
	cout << myd.GetDis() << endl;
	
	return 0;
}
```



2、写出以下程序运行的结果。
// 输出
```c++
122444

```

```C++
#include<iostream>
using namespace std;
class MyClass
{
public:
    MyClass(int i = 0)
    {
        cout << i;
    }
    MyClass(const MyClass &x)
    {
        cout << 2;
    }
    MyClass &operator=(const MyClass &x)
    {
        cout << 3;
        return *this;
    }
    ~MyClass()
    {
        cout << 4;
    }
};
int main()
{
    MyClass obj1(1), obj2(2);
    MyClass obj3 = obj1;
    return 0;
}
```



3、不考虑任何编译器优化(如:NRVO),下述代码的第10#会发生


// 输出
```c++
B()
B()
B func(const B &)
B(const B&)
B &operator=(const B &s)
~B()
~B()
~B()
```

```C++
#include <iostream>

using std::cout;
using std::endl;

classB
{
public:
	B()
	{
        cout << "B()" << endl;
    }

    ~B()
    {
    	cout << "~B()" << endl;
    }
    
    B(const B &rhs)
    {
        cout << "B(const B&)" << endl;
    }
    
    B &operator=(const B &rhs)
    {
    	cout << "B &operator=(const B &s)" << endl;
    
        return  *this;
    }
};

B func(const B &rhs)
{
    cout << "B func(const B &)" << endl;
    return rhs;
}


int main(int argc, char **argv)
{
	B b1,b2;
    // 走进func 函数体，return 时调用拷贝构造函数做一次拷贝
    // 回到 main ，做一次赋值，调用赋值构造函数。
    b2=func(b1);//10#

	return 0;
}
```



三、编程题。
1、实现一个自定义的String类，保证main函数对正确执行

实现代码：[TestSting.cc](/TestCode/cppDay4/TestSting.cc)

```C++
class String
{
public:
	String();
	String(const char *pstr);
	String(const String &rhs);
	String &operator=(const String &rhs);
	~String();
	void print();

private:
	char * _pstr;
};

int main()
{
	String str1;
	str1.print();
	

	String str2 = "Hello,world";
	String str3("wangdao");
	
	str2.print();		
	str3.print();	
	
	String str4 = str3;
	str4.print();
	
	str4 = str2;
	str4.print();
	
	return 0;
}
```



2、用C++实现一个双向链表		

```C++
struct Node
{
	int data;
 	Node * pre;
 	Node * next;
}; 

class List
{
public:
	List();
 	~List();
				
     void push_front(int data);//在头部进行插入         
     void push_back(int data);//在尾部进行插入
                 
     void pop_front();//在链表头部进行删除          
     void pop_back();//在链表的尾部进行删除
     
     bool find(int data);//在链表中进行查找
     void insert(int pos, int data);//在指定位置后面插入pos 
     void display() const; //打印链表	
     void erase(int data);//删除一个指定的节点			 		

private:
	Node * _head;
 	Node * _tail;
 	int    _size;
};		
```
