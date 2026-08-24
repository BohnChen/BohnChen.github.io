---
title: "运算符重载、友元"
date: 2026-08-23T21:17:55+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## 运算符重载
由于现有的运算符含义只针对内置类型有效，要对自己定义的对象进行操作，就必须要有运算符重载。

```c++
class MyClass {};

int main {
    MyClass c1();
    MyClass c2();
    
    // 进行重载后调用赋值运算符函数
    MyClass c3 = c1 + c2; /*  ERROR 无法直接相加 */

}
```
运算符重载不能改变符号原本的定义，也不能改变原本符号的操作数数量和顺序，且操作数中至少需要一个自定义类型或者枚举类型。

```c++
// ERROR 
// * overload operator+ must have at least
// one parameter of class or enumeration type
int operator+(int a, int b) {
    return a + b;
}
```

运算符重载之普通函数，访问私有成员，需要用类内`get, set`函数

```c++
/*   这里是重点内容

MyClass operator+(MyClass &a, MyClass &b) {
    MyClass tmp();
  /*  tmp._x = a._x + b._x; */ // ERROR
  /*  tmp._y = a._y + b._y; */ // ERROR
    tmp._x = a.getx() + b.getx();
    tmp._y = a.gety() + b.gety();
}

*/ // 下面是完整代码

#include <iostream>

using std::cout;
using std::endl;

class MyClass {
public:
  MyClass() : _x(0), _y(0) {}
  MyClass(int a, int b) : _x(a), _y(b) {}
  ~MyClass() {}

  int getx() const { return _x; }
  int gety() const { return _y; }

  void setx(int x) { _x = x; }
  void sety(int y) { _y = y; }

  void print() const {
    cout << "_x is " << _x << ", _y is " << _y << ". " << endl;
  }

private:
  int _x;
  int _y;
};

MyClass operator+(const MyClass &a, const MyClass &b) {
  MyClass tmp;
  tmp.setx(a.getx() + b.getx());
  tmp.sety(a.gety() + b.gety());
  return tmp;
}

int main(int argc, char *argv[]) {
  cout << "Today is a nice day." << endl;

  MyClass c1(1, 1);
  MyClass c2(2, 2);
  MyClass c3 = c1 + c2;
  c1.print();
  c2.print();
  c3.print();

  return 0;
}


```
运算符重载之友元函数，可以直接访问私有成员。所以，通过友元函数，上面的代码就可以写成：
```c++
/*
类中的 friend 声明，然后实现时候直接访问 private 成员
MyClass operator+(const MyClass &a, const MyClass &b) {
  MyClass tmp;
  tmp._x = a._x;
  tmp._y = a._y;
  return tmp;
}
*/
#include <iostream>

using std::cout;
using std::endl;

class MyClass {
public:
  MyClass() : _x(0), _y(0) {}
  MyClass(int a, int b) : _x(a), _y(b) {}
  ~MyClass() {}

  friend MyClass operator+(const MyClass &a, const MyClass &b);
  void print() const {
    cout << "_x is " << _x << ", _y is " << _y << ". " << endl;
  }

private:
  int _x;
  int _y;
};

MyClass operator+(const MyClass &a, const MyClass &b) {
  MyClass tmp;
  tmp._x = a._x + b._x;
  tmp._y = a._y + b._y;

  return tmp;
}

int main(int argc, char *argv[]) {
  cout << "Today is a nice day." << endl;

  MyClass c1(1, 1);
  MyClass c2(2, 2);
  MyClass c3 = c1 + c2;
  c1.print();
  c2.print();
  c3.print();

  return 0;
}
```


运算符重载之成员函数，由于非静态成员函数的第一个位置是`this`指针，需要注意参数个数，比如`+`运算符只有两个参数

[类内的二元运算符函数报错](/images/7_binary_operator_within_class.png)

普通的运算符重载，比如 + 运算符，两个值返回一个新的值，推荐以友元函数进行运算符重载。

特殊的运算符重载，比如符合运算符 `*=,+=,/=` 等运算符由于需要改第一个操作数，所以建议用成员函数的方式进行重载。


```c++
MyClass &MyClass::operator*=(const MyClass &rhs) {
  this->_x *= rhs._x;
  this->_y *= rhs._y;
  return *this;
}


```

自增运算符重载
由于
```c++
int a = 3;
++a; // 表达式为 4， a = 4
a++; // 表达式为 4, a = 5
```
所以我们要考虑两个不同的 `++` 运算符重载

```c++
MyClass &operator++() {
  cout << "MycClass operator++ " << endl;
  _x++;
  _y++;

  return *this;
}

// 用一个无用的int来做前置++和后置++的区分
MyClass operator++(int) {
  cout << "MycClass operator++(int) " << endl;
  MyClass tmp(*this);
  _x++;
  _y++;
  // 返回临时对象，调用拷贝构造函数
  return tmp;
}

```

输出流运算符的重载，不能放到类中去，因为对于输出流运算符，第一个操作数是`ostream`，如果放到类内，就改变了操作数顺序，这是不可以的。

另外，当把输出流运算符的重载放到全局中时，作为普通函数进行输出流运算符重载，会发现数据成员是私有的，想要访问，我们需要使用类内的`get、set` 函数，因此我们最好将其声明为友元，这样重载函数可以直接对齐操作。

```c++
// ERROR
// std::ostream& operator<<(/*param1: 流，param2:对象*/);

std::ostream &operator<<(std::ostream &os, const MyClass &rhs) {
  os << "x = " << rhs._x << " , y = " << rhs._y << ". " << endl;
  return os;
}

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  MyClass c1(1, 2);
  cout << c1;
  operator<<(std::cout, c1);

  return 0;
}
```


输入流运算符的重载



函数调用运算符(小括号)



下标访问运算符（中括号）


总结

除了二元运算符，其他运算法重载不是建议以成员函数的方式重载，就是必须以成员函数的方式重载。

## 友元
友元不受 `public，private，protect` 控制，友元破坏了封装性，所以应该尽量合理使用。

友元之普通函数

友元之成员函数

友元之友元类

友元函数需要单独设计为友元，不会因为函数名相同，就设计了一批友元函数。

单向：而且友元是单向的，在 A 类中设计了别的友元后，别的友元可以访问 A 中的私有成员，但是 A 中的函数无法访问这个友元中的值。

无传递性：A -> B -> C，但是 C 不一定是 A 的友元

无继承：友元不能被继承


