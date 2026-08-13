---
title: "面向对象、代码规范、对象的创建、对象的销毁、拷贝构造函数、this 指针、赋值运算符函数"
date: 2026-08-07
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

# 面向对象、代码规范、对象的创建、对象的销毁、拷贝构造函数、this 指针、赋值运算符函数

## 面向对象
`C` 语言是面向过程的语言，`C++` 是面向对象的语言。面向过程的 `C` 语言中，我们需要时刻关注资源的回收情况，出现一个疏忽，就可能出现`double-free`，出现`use-after-free`造成的程序崩溃；但是在 `cpp` 中，从机制上减少可能犯的错误：使用构造函数让对象知道自己诞生时候的状态；使用封装机制，使得外部代码无法直接操控数据，内部操作可以防止二次释放等操作；使用析构函数，管理和释放资源。结合这些机制，组成了资源获取即初始化(Resource Acquisition Is Initialization)，其含义就是：在对象构造时获取资源，在对象析构时释放资源，把资源生命周期绑定到对象生命周期上。减少由于人的疏忽造成的巨大问题。

除此之外，`cpp`相较于`c`的优势还在于：
1. 其使用封装管理，对数据成员的访问采用 `get,set` 方法访问，而不是使用`C`中的结构体变量那样的`.x`方式访问，其好处就是，当需要更改对象类型时候，不用将整个程序中也许几千几万处的"`.x`"改正，只需要修改方法中的 `get` 和 `set` 方法就够了。
2. 其使用继承机制，不需要写太多的重复代码
3. 其使用多态，消除了分支逻辑的散落和扩散
```c++
// 没有多态时，你会这样写：
void draw(void* shape, int type) {
    switch (type) {
        case CIRCLE: drawCircle(shape); break;
        case RECT:   drawRect(shape);   break;
        // 加一种新形状 → 所有 switch 都要加分支
    }
}
```
问题：每加一个新类型，你要找到所有分散在各处的 switch/if-else，逐个加上新分支。漏一个就是一个 bug。

```c++
// 有多态后：
class Shape { virtual void draw() = 0; };
class Circle : public Shape { void draw() override { ... } };
class Rect : public Shape { void draw() override { ... } };
// 加新形状 → 写一个新子类即可，已有的 Shape、Circle、Rect 一行不动
```

这就是开闭原则（Open/Closed Principle）：对扩展开放，对修改封闭。 新功能通过添加新代码实现，而不是修改已有稳定代码。这是控制大型系统复杂度的核心手段。
## 代码规范

c++代码规范可以参考google C++ 开发标准。

### 头文件引用规范：
```c++
// 最先引入自己写的头文件
#include "Myhead.h"

// 其次引入c 的头文件
#include <cstring>

// 最后是 cpp 的头文件
#include <iostream>

// 类名使用大驼峰
// 成员函数用小驼峰
class MyClass {
protected:
    void getParam(); 

// 数据成员以_开头，容易发现
private:
    int _myint;
};
```

## 对象的创建
在`cpp`中，对象在创建时就获取了资源。这个过程通过类的构造函数机制完成。
### 类外成员函数的定义
```c++
// computer.h
#ifndef __COMPUTER_H__
#define __COMPUTER_H__

class Computer
{
public:
    //成员函数
    void setBrand(const char *brand);
    void setPrice(float price);
    void print();

private://封装性
    //数据成员
    char _brand[20];//m_brand   brand_
    float _price;
};
#endif

/**************************************************************/

// computer.cc 
#include "Computer.h"
#include <string.h>
#include <iostream>

using std::cout;
using std::endl;

void Computer::setBrand(const char *brand)
{
    strcpy(_brand, brand);//先不去考虑越界的问题
}

void Computer::setPrice(float price)
{
    _price = price;
}

void Computer::print()
{
    cout << "brand : " << _brand << endl
         << "price : " << _price << endl;
}

/**************************************************************/

// testcomputer.cc 
#include "Computer.h"
#include <iostream>

using std::cout;
using std::endl;

int main(int argc, char **argv)
{
    //对象的创建
    Computer com;
    com.setBrand("huawei");
    com.setPrice(6000);
    com.print();

    return 0;
}
```

### 构造函数
构造函数完成了初始化工作
不写构造函数时，编译器会生成默认的无参构造函数
写了构造函数后，新建对象时候，必须调用构造函数初始化对象
数据成员被初始化的顺序，只与声明顺序有关，与初始化顺序无关。
自动进行，不能加.调用。

**构造函数可以重载，但是不能被显式调用。**

```c++
#include <iostream>

using std::cout;
using std::endl;

class Point
{
public:

    //构造函数是可以进行重载的
    //
    //默认情况下，编译器会生成一个默认构造函数（无参构造函数）
    //
    //如果自定义构造函数，这个时候编译器就不会为我们生成默认的
    //（无参的）构造函数，如果此时还想调用无参构造函数，必须显示
    //的定义出来
    //
    Point() 
        : _ix(0)//真正初始化数据成员的位置，称为初始化表达式
          , _iy(0)//或者称为初始化列表
    {
        /* int a = 0; */
        /* int b(0);//也是初始化变量 */
        cout << "Point()" << endl;
        /* _ix = 0;//赋值 */
        /* _iy = 0; */
    }
    
    //构造函数的特点：完成数据成员的初始化
    Point(int ix, int iy)
    : _ix(ix)
    , _iy(iy)
    {
        cout << "Point(int, int)" << endl;
        /* _ix = ix;//赋值 */
        /* _iy = iy; */
    }

    void print()
    {
        cout << "(" << _ix << ", "
              << _iy << ")" <<  endl;
    }

    //析构函数的作用：完成数据成员的清理工作
    //析构函数没有返回类型，没有参数，所以具有唯一性，不能重载
    //对象在离开其作用域被销毁的时候，会自动调用析构函数
    //
    //默认情况下，编译器会自动生成析构函数
    ~Point()
    {
        cout << "~Point()" << endl;
        //print();
    }
private:
    int _ix;
    int _iy;
};

void test()
{
    //int a;
    /* Computer com; */
    Point pt1;//栈对象
    pt1.print();

    cout << endl;
    Point pt(1, 3);//对象创建的时候会自动调用构造函数
    /* pt.Point(1, 3);//error,构造函数不能以对象加点的形式进行调用 */
    // Point(2, 5).print();//ok,构造函数的调用会创建对象

    pt.~Point();//析构函数是可以显示调用的,一般不建议显示调用

    cout << "pt = ";
    pt.print();
}

int main(int argc, char **argv)
{
    cout << "start test...." << endl;
    test();
    cout << "finish test...." << endl;
    return 0;
}
```

> **c的struct和cpp的struct有什么区别**

|               | C struct                             | C++ struct                      |
| --            | --                                   | --                              |
| 数据成员      | 有                                   | 有                              |
| 成员函数      | 无（只能放函数指针）                 | 有                              |
| 构造/析构函数 | 无                                   | 有                              |
| 访问控制      | 无（全公开）                         | 有 public / private / protected |
| 继承          | 不支持                               | 支持                            |
| 虚函数/多态   | 不支持                               | 支持                            |
| 类型名使用    | 必须写 struct Foo f;（除非 typedef） | 直接 Foo f;                     |
| 默认初始化    | 靠手动 {1,2} 或 memset               | 构造函数自动执行                |

联系： C++ 的 struct 是 C struct 的超集。C 代码里的 struct 定义在 C++ 编译器中完全合法。核心差异一句话——C 的 struct 只是数据袋子，C++ 的 struct 是带方法、带自动初始化和清理的全功能类。

> **c 的struct 和cpp 的class有什么区别和联系呢？**

| 维度         | C struct                 | C++ class                   |
|--------------|--------------------------|-----------------------------|
| 数据封装     | 无保护，任何代码能直接改 | private 默认外部不能碰      |
| 行为绑定     | <BS> 数据和操作分离      | 数据和操作绑在一块          |
| 对象生命周期 | 靠人手动 init / cleanup  | 构造/析构自动管理           |
| 代码复用     | 没有继承机制             | 有继承链                    |
| 多态         | 靠手写函数指针表模拟     | virtual + vtable 原生支持   |
| 类型安全     | 弱（void* + 类型强转）   | 强（模板 + 继承链类型检查） |

一句话： C 的 struct 是把几个变量捆在一起方便搬运；C++ 的 class 是把数据、行为、约束、生命周期全部封装成一个独立的、自管理的抽象单元。前者帮程序员"少写几个参数"，后者帮程序员"杜绝一整类 bug"。

> **cpp 的struct 和 cpp 的class 有什么区别和联系?**

唯一的区别：默认访问权限。
```c++
struct S { int x; };    // x 默认是 public
class C { int x; };     // x 默认是 private

// 继承同理：
struct D : B {};  // 默认 public 继承
class D : B {};   // 默认 private 继承
```
除此之外完全等价。

`struct` 可以有构造/析构、虚函数、`private` 成员；`class` 也可以全写 `public`。这是设计上的一个"习惯提示"——`struct` 暗示"主要是数据聚合"，`class` 暗示"抽象数据类型，有封装语义"。

```c++
#include <string.h>
#include <iostream>

using std::cout;
using std::endl;

// C++中对struct的功能做了提升，不仅可以定义数据也可以定义函数，
// struct与class的唯一区别就是：struct的默认访问权限是public
// class默认访问权限是private
struct Computer
{
    //类的内部，左大括号到右大括号称为类的内部
public:
    //成员函数
    inline
    void setBrand(const char *brand)
    {
        strcpy(_brand, brand);//先不去考虑越界的问题
    }

    inline
    void setPrice(float price)
    {
        _price = price;
    }

    inline
    void print()
    {
        cout << "brand : " << _brand << endl
             << "price : " << _price << endl;
    }
private://封装性
    //数据成员
    char _brand[20];//m_brand   brand_
    float _price;
};

int main(int argc, char **argv)
{
    //可以吧Computer看成一种类型，类型称为自定义类型
    //int a;
    //对象的创建
    Computer com;
    com.setBrand("huawei");
    com.setPrice(6000);
    com.print();
    /* Computer com2; */

    /* com._price = 3000;//error, 封装性的特点 */
    return 0;
}


```

## 对象的销毁
在`CPP`中，对象的销毁通过析构函数的自动调用完成。

### 析构函数
默认情况下，编译器自动生成析构函数
对象在销毁时候，自动调用，在函数中时，函数栈销毁前调用析构函数回收资源。自动进行，但是能加"`.`"调用。

**析构函数不能被重载，但是可以被显式调用（但是不建议显示调用）**

```c++
#include <iostream>

using std::cout;
using std::endl;

class Point
{
public:

    //构造函数的特点：完成数据成员的初始化
    Point(int ix = 0, int iy = 0)
    : _ix(ix)
    , _iy(iy)
    {
        cout << "Point(int = 0, int = 0)" << endl;
    }

    //默认情况下，编译器会自动生成拷贝构造函数
    Point(const Point &rhs)//Point pt2 = pt1
    : _ix(rhs._ix)
    , _iy(rhs._iy)
    {
        cout << "Point(const Point &)" << endl;
    }

    void print()
    {
        cout << "(" << _ix << ", "
              << _iy << ")" <<  endl;
    }

    //析构函数的作用：完成数据成员的清理工作
    //析构函数没有返回类型，没有参数，所以具有唯一性，不能重载
    //对象在离开其作用域被销毁的时候，会自动调用析构函数
    //
    //默认情况下，编译器会自动生成析构函数
    ~Point()
    {
        cout << "~Point()" << endl;
        //print();
    }
private:
    int _ix;
    int _iy;
};

void test()
{
    //int a;
    /* Computer com; */
    Point pt1;//栈对象
    pt1.print();

    cout << endl;
    Point pt(1, 3);//对象创建的时候会自动调用构造函数
    /* pt.Point(1, 3);//error,构造函数不能以对象加点的形式进行调用 */
    /* Point(2, 5).print();//ok,构造函数的调用会创建对象 */

    pt.~Point();//析构函数是可以显示调用的,一般不建议显示调用

    cout << "pt = ";
    pt.print();
}

```

### 对象在不同的位置
对象在栈上，静态空间内，可以被自动管理；但是当对象中有指针时，需要将其放在堆上上，必须手动申请和释放。有了析构函数后，那就需要在析构函数中进行处理。

```c++
Computer::Computer(const char *brand, float price)
: _brand(new char[strlen(brand) + 1]())
, _price(price)
{
    /* _brand = new char[strlen(brand) + 1](); */
    cout << "Computer(const char *, float)" << endl;
    strcpy(_brand, brand);
}


Computer::~Computer()
{
    cout << "~Computer()" << endl;
    /* if(_brand != nullptr) */
    if(_brand)
    {
        /* cout << "delete [] _brand" << endl; */
        delete [] _brand;
        _brand = nullptr;
    }
}

void test()
{
    cout << "start main... " << endl;
    //对象的创建
    Computer com("huawei", 6000);//栈对象
    /* com.setBrand("huawei"); */
    /* com.setPrice(6000); */

    /* com.~Computer();//不要显示调用析构函数 */

    /* com.print(); */

    /* int *pInt = new int(10); */

    Computer *pc = new Computer("mac", 20000);//堆对象一定要进行delete操作
    pc->print();

    delete pc;

    cout << "finish main..." << endl;

}

```

## 拷贝构造函数
调用拷贝构造函数的时机：
1. 当用已经存在的对象初始化一个新对象时候调用
```c++
void test2()
{
    int a(3);
    int b = a;

    Point pt1(1, 3);
    cout << "pt1 = ";
    pt1.print();

    cout << endl;
    //拷贝构造函数调用时机1：用一个已经存在的对象初始化一个
    //刚刚创建的对象时候，会调用拷贝构造函数
		Point p4(pt1);
    // Point(const Point &rhs)
    // const Point &rhs = pt1
    Point pt2 = pt1;//pt2对象创建会有构造函数的调用
    cout << "pt2 = ";
    pt2.print();

}
```
2. 当函数参数类型为对象类型，实参和形参结合时候，会调用拷贝构造函数
```c++
//拷贝构造函数的调用时机2：当函数的参数是类类型的时候，
//在调用函数的时候，进行形参与实参结合的时候，会调用
//拷贝构造函数
void func2(Point pt)//Point pt = pt3;
{
    pt.print();
}
```
3. 当返回类型是对象类型时，会调用拷贝构造函数
```c++
//拷贝构造函数的调用时机3：当函数的返回类型是类类型的时候，
//在执行return语句的时候会执行拷贝构造函数
Point func3()
{
    Point pt1(1, 4);
    cout << "pt1 = ";
    pt1.print();

    return pt1;
}
```

### 浅拷贝和深拷贝
浅拷贝只拷贝值，内存空间指向同一片位置，但是深拷贝构造函数不止拷贝值，还会申请一个新的内存空间。

```c++
#include "Computer.h"
#include <string.h>
#include <iostream>

using std::cout;
using std::endl;

Computer::Computer(const char *brand, float price)
: _brand(new char[strlen(brand) + 1]())
, _price(price)
{
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
Computer::Computer(const Computer &rhs)
: _brand(new char[strlen(rhs._brand) + 1]())//深拷贝
, _price(rhs._price)
{
    cout << "Computer(const Computer &)" << endl;
    strcpy(_brand, rhs._brand);
}
#endif

```

```c++
void Computer::setBrand(const char *brand)
{
    strcpy(_brand, brand);//先不去考虑越界的问题
}

void Computer::setPrice(float price)
{
    _price = price;
}

void Computer::print()
{
    printf("brand : %p\n", _brand);
    cout << "brand : " << _brand << endl
         << "price : " << _price << endl;
}

Computer::~Computer()
{
    cout << "~Computer()" << endl;
    /* if(_brand != nullptr) */
    if(_brand)
    {
        /* cout << "delete [] _brand" << endl; */
        delete [] _brand;
        _brand = nullptr;
    }
}
```
**// 问题1：拷贝构造函数参数中的引用符号可以去掉吗？**

不可以省略。

这个`&`引用符号不能被省略，是因为如果省略`&`引用符号，那么，当调用拷贝构造函数时，rhs = p1，p1已存在，但是rhs是一个新对象，所以重新调用拷贝构造函数，陷入循环，并且没有出口，程序直到栈溢出崩溃。

例：

```c++
#include <string>
#include <iostream>

class Point() {
    Point(string word, int intPara)
    :_word(word)
    ,_int(intPara){
        
    }
    Point(const Point &rhs) {
        // example.
    }
private:
    string _word;
    int _int;
};

Point p1("nice", 10);

// Point(const Point rhs) 
// 当调用拷贝构造函数时，rhs = p1，p1已存在，但是rhs是一个新对象
// 所以重新调用拷贝构造函数，陷入循环，并且没有出口，程序直到栈溢出崩溃。
Point p2 = p1; 
```

**// 问题2：拷贝构造函数参数中的const可以去掉吗？**



那么，拷贝构造函数中的const可以去掉吗？

答：不能，为什么呢？我们有时需要传递右值，（比如临时对象、临时变量、匿名对象），如果没有const修饰，我们将无法传递右值对象给对象，因为“非const左值引用不能绑定到右值”，因此，为了我们灵活使用语言，`const`也不能去掉。
我们可以写一个返回类型是对象的函数，然后新建一个对象，将返回的对象赋值给它会报错：
```c++
Point func() {
    Point pt4;
    return pt4;
}
Point p3 = func(); // ERROR
// 若拷贝构造函数为 Point(Point &rhs) 
// 因为临时对象是右值，右值不能绑定到非const对象，所以使用Point p4 = func()
// 时会报错，因为在Pont &rhs = func()时，报错了
const Point p4 = func(); // YES,临时对象绑定到const对象是可以的。
```
什么是右值什么是左值呢？

**右值就是不能被取地址的值，左值是能被取地址的值。**
```c++
void test4()
{
    int number = 100;
    int &ref = number;
    const int &ref2 = 100;
    /* &100;//error,字面值常量 */
    &number; // 左值 l-value

    //Point(Point &rhs)
    /* &func3();//error */ // 右值 r-value
    const Point &rhs = func3();
    Point pt2 = func3();
    cout << "pt2 = ";
    pt2.print();
}


```

所以，这就是为什么拷贝构造函数的参数类型是`(const MyType &typeUseCase)`，`const`和`&`都不能少。
```c++
#include <iostream>

using std::cout;
using std::endl;

class Point
{
public:

    //构造函数的特点：完成数据成员的初始化
    Point(int ix = 0, int iy = 0)
    : _ix(ix)
    , _iy(iy)
    {
        cout << "Point(int = 0, int = 0)" << endl;
    }

    //Q:拷贝构造函数参数中的引用符号可以去掉吗？
    //A:不能去掉，如果去掉引用符号，在满足拷贝构造函数的
    //情况，会满足拷贝构造函数的调用时机1，然后进行无限循环调用
    //拷贝构造函数，函数的参数是会入栈，这样的话栈空间肯定会溢出
    //的，所以引用符号不能去掉
    //
    //Q：拷贝构造函数参数中的const可以去掉吗？
    //A:不能去掉，当传递的是右值（临时对象、临时变量、匿名对象）
    //的时候，会返回报错（非const左值引用不能绑定到右值），当传递
    //临时对象的时候，就会报错
    //
    //默认情况下，编译器会自动生成拷贝构造函数
    /* Point(const Point &rhs)//Point pt2 = pt1 */
    Point(const Point &rhs)//Point pt2 = pt1
    : _ix(rhs._ix)
    , _iy(rhs._iy)
    {
        cout << "Point(const Point &)" << endl;
    }

    void print()
    {
        cout << "(" << _ix << ", "
              << _iy << ")" <<  endl;
    }

    //析构函数的作用：完成数据成员的清理工作
    //析构函数没有返回类型，没有参数，所以具有唯一性，不能重载
    //对象在离开其作用域被销毁的时候，会自动调用析构函数
    //
    //默认情况下，编译器会自动生成析构函数
    ~Point()
    {
        cout << "~Point()" << endl;
        //print();
    }
private:
    int _ix;
    int _iy;
};

void test4()
{
    int number = 100;
    int &ref = number;
    const int &ref2 = 100;
    /* &100;//error,字面值常量 */
    &number;

    //Point(Point &rhs)
    /* &func3();//error */
    const Point &rhs = func3();
    Point pt2 = func3();
    cout << "pt2 = ";
    pt2.print();
}
int main(int argc, char **argv)
{
    test4();
    return 0;
}
```
----

## this 指针
this 指针其实是隐藏于非静态成员函数的第一个参数位置的参数。既然是指针，需要用`"->"`调用

C++ 的 this 指针
this 是一个隐含的指针参数，存在于每个非静态成员函数中，指向调用该函数的那个对象。
它是什么
编译器实际处理时，会把：
```c++
class Point {
    int x, y;
public:
    void move(int dx, int dy) {
        x += dx;   // 实际是 this->x += dx
        y += dy;
    }
};

Point p;
p.move(3, 5);
```
等价转换为：
```c++
// 编译器内部做了这个转换
void Point_move(Point* const this, int dx, int dy) {
    this->x += dx;
    this->y += dy;
}

Point_move(&p, 3, 5);  // 对象地址作为第一个参数传入
```

this 就是该对象的地址，类型是 Point* const——允许修改指向的对象，但不允许改变指针自身指向。
三个关键特性
1. 是编译器自动传递的隐含参数
你不需要声明，不需要手动传，每个非静态成员函数调用时编译器自动塞进去。

2. 区分成员和参数同名时用
```c++
class Person {
    string name;
public:
    void setName(string name) {
        this->name = name;  // 左边是成员，右边是参数
    }
};
```
没有 this->，name = name 就是自己赋给自己，啥也没干。


3. 支持链式调用
```c++
class Builder {
public:
    Builder& addA() { /*...*/ return *this; }
    Builder& addB() { /*...*/ return *this; }
};

Builder().addA().addB().addC();  // 链式调用
```

返回 *this（对当前对象的引用），才能让调用连起来。

静态函数里没有 this
```c++
static void func() {
    this;   // 错误！静态函数不绑定任何对象，没有 this
}

```
-----
静态成员函数属于类本身，不属于任何具体对象，所以不存在指向具体对象的 this。
const 对 this 的影响
```c++
class Point {
public:
    int getX() const { return x; }
    // 这里 this 的类型是 const Point* const
    // 不能修改 this 指向的对象，也不能改 this 本身
};

```
const 成员函数就是告诉编译器：这个函数里 this 指向的是一个 const 对象，你不能通过 this 修改成员。这让你能在 const 对象上调用该方法，并且编译器帮你检查你不会意外修改。

## 赋值运算符函数
所以，到目前为止，我们的编译器给我们生成了四个函数，分别是无参的构造函数、析构函数、拷贝构造函数、赋值运算符函数
```c++
class Empty {
public:
    Empty();                              // 1. 默认构造函数
    ~Empty();                             // 2. 析构函数
    Empty(const Empty&);                  // 3. 拷贝构造函数
    Empty& operator=(const Empty&);       // 4. 拷贝赋值运算符
    // 之后还会有如下两个，我们慢慢了解
    Empty(Empty&&);                       // 5. 移动构造函数（C++11）
    Empty& operator=(Empty&&);            // 6. 移动赋值运算符（C++11）
};

```

## 作业
一、简答题
1.设A为test类的对象且赋有初值,则语句test B(A); 表示。

```bash
用 A 初始化新对象 B
```

2.利用“对象名.成员变量”形式访问的对象成员仅限于被声明为 (1)的成员；若要访问其他成员变量，需要通过 (2) 函数

```bash
（1）public 或者子类访问protected 的

（2）成员
```

3、浅拷贝与深拷贝区别？

```bash
浅拷贝不申请新地址，两个对象同用一个地址；深拷贝不仅拷贝值，还为新对象开辟新的空间

```

二、写出下面程序结果。
1、写出以下程序运行的结果。（ ）

运行后知：

i=0,k=2

i=0,k=2

```C++
#include <iostream>

using std::cout;
using std::endl;

class Sample 
{
public:
      Sample();
      void Display();
private:
      int i;
      static int k;
};
Sample::Sample() 
{
	i=0;
	k++;
}

void Sample::Display() 
{
   cout << "i=" << i << ",k=" << k << endl;
}

int Sample::k=0;

int main( ) 
{
    Sample a, b;
    a.Display();
    b.Display();
    
    return 0;
}
```

2、设有如下程序结构：

答：3 次构造函数，3 次析构函数
```C++
class Box
{
    //....
};

int main()
{
	Box A,B,C; 
}
该程序运行时调用__次构造函数；调用 __次析构函数。
```

3、写出下面程序的运行结果（）

```bash
Constructor1
Constructor2
i=0
i=10
Destructor
Destructor
```


```C++
#include <iostream>

using std::cout;
using std::endl;

class Sample 
{
	int i;
public:
	Sample();
	Sample(int val);
	void Display();
	~Sample();
};
Sample::Sample() 
{
	cout << "Constructor1" << endl;
	i=0;
}

Sample::Sample(int val) 
{
	cout << "Constructor2" << endl;
    i=val;
}

void Sample::Display() 
{
   cout << "i=" << i << endl;
}

Sample::~Sample() 
{
   cout << "Destructor" << endl;
}

int main() 
{
     Sample a, b(10);
     a.Display();
     b.Display();
	 
     return 0;
}
```

4、设已经有A,B,C,D4个类的定义，程序中A,B,C,D析构函数调用顺序为？
```C++
C c;
void main()
{
    A *pa=new A();
    B b;
    static D d;
    delete pa;
}
```

// 输出如下：
// C()
// A()
// B()
// D()
// ~A()
// ~B()
// ~D()
// ~C()

你的理解不完全对。D 的析构在 C 之前，不是因为栈销毁，而是因为 C 和 D 都有*静态存储期 (static storage duration)*，它们遵循的规则是：

> 同一编译单元内的静态对象，按构造的反序析构。

析构顺序：

| 顺序 | 输出 | 原因                                                    |
| --   | --   | --                                                      |
| 1    | ~A() | delete pa 在 return 前显式释放堆对象                    |
| 2    | ~B() | return 时局部栈对象出作用域，栈展开                     |
| 3    | ~D() | 静态局部对象，构造顺序为 2，静态对象反序析构，故先于 C |
| 4    | ~C() | 全局对象，构造顺序为 1，最后析构                        |

所以 D 在 C 之前析构，纯粹是因为 D 后构造。 静态对象的生命周期由运行时管理，和栈无关——栈销毁只影响局部自动对象（B），不影响 static/global 对象。





5、写出下面程序的结果：


```C++
#include<iostream>

using std::cout;
using std::endl;

int i = 1;

class Test
{
public:
	Test()
	:_fourth(_third)
	,_second(i++)
	,_first(i++)
	,_third(i++)
	{
		_third = i;
	}
	void print()
	{
		cout << "result : " << _first + _second + _third + _fourth << endl;
	}
private:
	int _first;
	int _second;
	int _third;
	int &_fourth;//注意：与前面学的引用类比即可
};

int main()
{
	Test test;
	test.print();
	
	return 0;
}
```
我答：11。因为first = 1, second = 2, third = 4, fourth = 4。
核心在于：
1. 初始化顺序安照生命顺序走
2. i++是先引用后自增
3. 构造函数中，先初始化列表执行，后函数体执行



6、下列代码在编译时会产生错误的是()

```C++
#include <iostream>

using std::cout;
using std::endl;

struct Foo
{
	Foo()
	{
	}
	

	Foo(int)
	{
	}
	
	void func()
	{
	}

};

int main(void)
{
	Foo a(10);//语句1
	a.fun();//语句2
	Foo b();//语句3
	b.fun();//语句4 
	return 0;
}
```

> 因为 C++ 语法规定：任何能被解析为函数声明的语句，就必须被解析为函数声明。 和你是不是 struct 无关。
Foo b(); 从语法形式上完全匹配 "返回值类型 函数名(参数列表)" 这一模式。编译器在语法分析阶段只看形式，不看上下文——Foo 是 struct 没关系，因为函数完全可以返回 struct 类型（比如 std::string getName()）。
这是继承自 C 的语法规则：int f(); 在 C 里就是函数声明，C++ 无法改变这一点，否则会破坏兼容性。所以即使你本意是构造一个对象，编译器也只能把它当成函数声明。
这就是 Most Vexing Parse 的由来。



三、改错题。
例题1：分析找出以下程序中的错误，说明错误原因，给出修改方案使之能正确运行。
[testError1.cc](/TestCode/cppDay3/testError1.cc)

```C++
#include <iostream>

using std::cout;
using std::endl;

class Base
{ 
	int a1,a2;
public:
	Base(int x1 = 0, x2 = 0);
};

int main()
{
	Base data(2,3);
 	cout << data.a1 << endl;
 	cout << data.a2 << endl;
    
    return 0;
}
```



例题2：分析以下程序的错误原因，给出修改方案使之能正确运行。

[testError2.cc](/TestCode/cppDay3/testError2.cc)
```C++
#include <iostream>

using std::cout;
using std::endl;

class Base
{
	float _ix;
	float _iy;
public:
    Base(float ix,float iy)
    {
        _ix = ix;
        _iy = iy;
    }
 	
    float gain();
};

Base::float gain()
{ 
	return _iy/_ix; 
}

int main()
{
	Base base(5.0,10.0);
 	cout << "The gain is => " << gain() << endl;
    
    return 0;

}
```



四、编程题。
1、定义一个学生类，其中有3个数据成员：学号、姓名、年龄，以及若干成员函数。同时编写main函数使用这个类，实现对学生数据的赋值和输出。
[Exercise1.cc](/TestCode/cppDay3/Exercise1.cc)

2、编写一个程序计算两个给定的长方形的周长和面积。
[Exercise2.cc](/TestCode/cppDay3/Exercise2.cc)

3、编写一个类，实现简单的栈。栈中有以下操作：
[Exercise3.cc](/TestCode/cppDay3/Exercise3.cc)

		   > 元素入栈     void push(int);
		   > 元素出栈     void pop();
		   > 读出栈顶元素 int top();
		   > 判断栈空     bool emty();
		   > 判断栈满     bool full();
	 如果栈溢出，程序终止。栈的数据成员由存放10个整型数据的数组构成。（可以自己设计入栈出栈的数据）

提示：就是用C++类的方式实现一个栈，然后写出栈的基本操作，入栈、出栈、栈为空、栈为满的函数，以及模拟栈的入栈出栈的操作。



4、编写一个类，实现简单的队列。队列中有以下操作：
[Exercise4.cc](/TestCode/cppDay3/Exercise4.cc)

       > 元素入队             void push(int);
       > 元素出队             void pop();
       > 读取队头元素         int front();
       > 读取队尾元素         int back();
       > 判断队列是否为空     bool emty();
	   > 判断队列是否已满     bool full();

注意循环队列的使用

提示：就是用C++类的方式实现一个队列，然后写出队列的基本操作，入队列、出队列、队列为空、队列为满的函数，以及模拟队列的入队列出队列的操作。
