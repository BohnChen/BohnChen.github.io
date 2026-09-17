---
title: "多态"
date: 2026-09-15T19:54:20+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## 多态
前面我们已经初步介绍了多态存在的意义是为了降低代码的耦合性，多态的使用方式是在基类中声明虚函数或者纯虚函数，派生类继承有虚函数的基类，定义基类对象指针或引用指向派生类，使用指针或者引用调用重写的虚函数。今天我们深入到多态的实现机制中，从多态的实现原理出发，认识一下多态实现过程中产生的概念，熟悉一下多态的用法

## 虚函数
从前面我们知道，用`virtual`关键字修饰的函数是虚函数，比如下方的`display()`就是一个虚函数，一旦基类中写了`virtual`关键字，派生类中即使不写`virtual`关键字，也是虚函数。我们推荐写上`override`关键字，是因为这样可以让编译器帮你检查是否有错误。
```
class Base {
public:
  Base(int a) : _ia(a) { cout << "Base(int a)" << endl; }

  ~Base() { cout << "~Base()" << endl; }

  virtual void display() {
    std::cout << "Base() :: display(), _ia is " << _ia << std::endl;
  }

private:
  int _ia;
};
```

## 纯虚函数和抽象类
纯虚函数指的是没有函数体的基类中的虚函数，它只提供了一个声明，需要派生类自己实现接口的具体定义。如果一个类中有一个纯虚函数，那么这个类就是一个抽象类，比如下方，因为有了纯虚函数`display();`，类`AbstractBase`变成了抽象类。

```c++
class AbstractBase {
public:
  AbstractBase(int a) : _ia(a) { cout << "AbstractBase(int a)" << endl; }
  ~AbstractBase() { cout << "~AbstractBase()" << endl; }

  // 告诉作者，我知道你会用，但是我不知道你会怎么用
  // 所以你来负责具体实现
  // 通过纯虚函数，可以提供一个与派生类一致的接口
  virtual void display() = 0;
  virtual void print() = 0;

private:
  int _ia;
};
```
一个类可以包含多个纯虚函数，只要类中有一个纯虚函数，那么这个类就是抽象类，一个抽象类只能作为基类来派生新类，不能创建抽象类的对象。
```c++

int main() {
    AbstractBase b(1); // ERROR : Variable type "AbstractBase" is an abstract class 
    return 0;
}

```

继承了抽象类的派生类，一般要对所有的纯虚函数进行重定义，如果没有对所有的纯虚函数进行重定义，那么这个派生类也变成了抽象类。AbstractDerive只实现了一个纯虚函数，所以它继承了另一个纯虚函数而没有实现，那么它也变成了抽象类，不能创建对象。
```c++
class AbstractDerive : public AbstractBase {
public:
  AbstractDerive(int a, int b) : AbstractBase(a), _id(b) {}
  ~AbstractDerive() {}

  void display() { cout << "AbstractDerive() :: display() " << endl; }

private:
  int _id;
};

```

除此以外，还有另外一种形式的抽象类。对一个类来说，如果只定义了protected型的构造函数而没有提供public构造函数，无论是在外部还是在派生类中作为其对象成员都不能创建该类的对象，但可以由其派生出新的类，这种能派生新类，却不能创建自己对象的类是另一种形式的抽象类
```c++
// 现在这就是另一个形式的抽象类
class AbstractBase {
protected:
  AbstractBase(int a) : _ia(a) { cout << "AbstractBase(int a)" << endl; }
  ~AbstractBase() { cout << "~AbstractBase()" << endl; }

private:
  int _ia;
};

class AbstractDerive : public AbstractBase {
public:
  // 没问题
  AbstractDerive(int a, int b) : AbstractBase(a), _id(b) {}
  ~AbstractDerive() {}

  // 在类体内部，Type a(...)
  // 会被编译器当作成员函数声明来解析（为了避免与函数声明产生类似 Most Vexing
  // Parse 的二义性，C++ 明确禁止在类内使用圆括号 () 进行成员就地初始化）。
  AbstractBase a(1);                    // ERROR: 报语法错误
  AbstractBase b = new AbstractBase(1); // ERROR : 报调用了protected 构造函数
  void display() { cout << "AbstractDerive() :: display() " << endl; }

private:
  int _id;
};

int main(int argc, char *argv[]) {
  // AbstractBase a(1); // ERROR: 调用了 protected 构造函数
  AbstractDerive b(1, 2);

  return 0;
}

```

## 虚析构函数
基类的析构函数被设置为虚函数之后，派生类的析构函数自动变为虚函数

为什么这样可以，是因为编译器做了一个优化，因为析构函数对于任何一个类都只有一个，具有唯一性，编译器就将析构函数改为destructor,当基类将析构函数定义为虚函数之后，只要派生类定义了自己的析构函数，编译器就认为这是一种重写（重定义）

虚析构函数的设计，是为了防止内存泄漏。
这里可以看一个例子，这里需要帮我生成一个例子说明

## 重载、隐藏、覆盖
 重载：***发生在同一个作用域中***，函数名称相同，但是参数的类型、个数、顺序不同（参数列表不一样）
 ```c++
 class A {
    A() {}

    void print() { cout << "print()" << endl;}
    void print(int a) { cout << "print() : a = " << a << endl;}
    void print(int a, int b) { cout << "print() : a = " << a << ", b = " << b << endl;}

 };

 ```
 覆盖：发生在基类与派生类中，同名虚函数，参数列表亦完全相同
 ```c++
class Base {
public:
  Base(int a) : _ia(a) { cout << "Base(int a)" << endl; }

  ~Base() { cout << "~Base()" << endl; }

  virtual void display() {
    std::cout << "Base() :: display(), _ia is " << _ia << std::endl;
  }

private:
  int _ia;
};

class Derive : public Base {
public:
  Derive(int a, const string str) : Base(a), _str(str) {}
  ~Derive() {}

  // 虚函数与其重写的函数只有函数体不一样
  void display() override {
    std::cout << "Derive() :: display(), _str is " << _str << std::endl;
  }

private:
  string _str;
};

 ```
 隐藏：发生在基类与派生类中，指的是某些情况下，派生类中的函数屏蔽了基类中的同名函数。隐藏既可以发生在同名的函数上，也可以发生在同名的数据成员上
 ```c++
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class Base {
public:
  Base(int a) : _ia(a) { cout << "Base()" << endl; }
  ~Base() {}

  void print() {
    cout << "Base::print()" << endl;
    _ia = 2;
  }

private:
  int _ia;
};

class Derive : public Base {
public:
  Derive(int a, int b) : Base(a), _id(b) {
    cout << "Derive(int a, int b)" << endl;
  }
  ~Derive() {}

  void print() { cout << "Derive::print(int a), _id = " << _id << endl; }

private:
  int _id;
};

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  Derive d(1, 2);
  d.print();
  d.Base::print();
  return 0;
}

 ```

 所以，在不同的作用域内（）排除重载，不是虚函数（排除覆盖），那就是一定是隐藏

## 多态原理
一些基本的概念我们写在前面：
+  虚基指针
 虚拟继承：class B : virtual class A {};

+  虚函数指针

多态是通过每个对象自己的空间前的虚基指针和和偏移量，虚函数指针共同完成的，重写的虚函数存在内存的rodata区，当新建了一个对象后，有虚函数的类对象，会维护自己的虚基指针，当调用虚函数时，通过虚基指针找到虚表，再通过偏移量找到虚表中的虚函数指针，然后仿真指针读到内存的 rodata 区，这时，读到对象自己要调用的虚函数实现的多态。

这里可以给我生成一张原理图，地址放在/Users/bohn/04_Project/06_MyWeb/BohnChen.github.io/static/images/cppstudy/，图片插入方式参考其他文章

## 加上虚函数后的多继承
