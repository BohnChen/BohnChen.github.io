---
title: "类型转换函数、作用域、Pimpl设计模式、单例的自动释放、string 的底层原理"
date: 2026-08-27T15:25:15+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## 类型转换

### 类型转换函数

**产生原因**：内置类型之间（int、double、bool……）编译器已经内置了转换规则，可以直接隐式转换；但**自定义类**对其他类型的转换，编译器一无所知。为了让自定义类用起来像内置类型一样自然，C++ 提供了**类型转换函数**（转换运算符），允许用户自己定义"类 → 其他类型"的转换规则。它是运算符重载家族的一员——把"类型转换"当成一种运算符来重载。

**语法与规则**：

```c++
operator 目标类型() const { ... }
```

- 必须写成 `operator 目标类型()`，**不写返回类型**（返回类型就是目标类型本身）；
- **不能带参数**，**必须是成员函数**（转换的是"自己"）；
- 触发时机：编译器在"需要目标类型的值、而手里是类对象"的地方自动隐式调用。

> 注意方向：**构造函数**负责"其他类型 → 类"，**类型转换函数**负责"类 → 其他类型"，二者方向相反、互为逆转换。

**代码示例 1：类 → double（数值类）**

复数类取实部转成 double，让 `Complex` 能直接参与 double 的运算：

```c++
#include <iostream>
using std::cout;
using std::endl;

class Complex {
public:
  Complex(double real, double imag = 0) : _real(real), _imag(imag) {}

  // 类型转换函数：Complex → double，取实部
  operator double() const { return _real; }

private:
  double _real;
  double _imag;
};

int main() {
  Complex c(3.14, 2.0);

  double d = c;                      // 隐式调用 operator double()
  double d1 = static_cast<double>(c); // 显式调用
  double d2 = (double)c;             // C 风格强制转换

  cout << d << " " << d1 << " " << d2 << endl;  // 输出 3.14 3.14 3.14
  return 0;
}
```

**代码示例 2：operator bool 判空（包装对象）**

`std::shared_ptr`、`optional` 等都能直接 `if (p)` 判断是否为空，靠的就是 `operator bool`：

```c++
#include <iostream>
using std::cout;
using std::endl;

class MyPtr {
public:
  explicit MyPtr(int *p) : _ptr(p) {}

  // 类型转换函数：MyPtr → bool
  operator bool() const { return _ptr != nullptr; }

private:
  int *_ptr;
};

int main() {
  int x = 5;
  MyPtr p(&x);
  if (p) { cout << "p 非空" << endl; }  // 条件上下文自动调用 operator bool()
  return 0;
}
```

**应用场景**：

1. **COW string 的代理类**（关联本系列的 stringCow.cc）：`Agency` 里的 `operator char()` 就是类型转换函数。它让 `char c = s3[0];` 走"读路径"直接取字符，**不触发 detach 复制**；而 `s3[0] = 'H';` 走代理的 `operator=`，才先 detach 再写。同一个代理类，靠"转成 char（读）"和"被赋值（写）"两个运算符区分了读写路径；
2. **智能指针 / 包装对象判空**：`operator bool`（示例 2）；
3. **自定义数值类型与内置类型互转**：复数、分数、大数等，让它们能直接参与内置类型的运算和比较；
4. **需要把对象"值"交给内置类型的场合**：`char c = s[0];`、`if (ptr)`、`obj + 1` 等。

其中第 1 点的 `operator char()` 核心代码：

```c++
operator char() const {              // 读：转成 char 的值，不 detach
  return _idx < _self->size() ? _self->_pstr[_idx] : '\0';
}
```

**注意事项**：

- 必须是成员函数，**不能写返回类型**，**不能带参数**；
- 一个类可以定义多个转换函数，但转换路径过多时编译器可能产生**歧义**而报错；
- C++11 起可写 `explicit operator bool() const`，限制只能在条件上下文隐式转换，避免 `int i = p;` 这类意外转换。

## 作用域

作用域可以分为**类作用域、类名的作用域以及对象的作用域**几部分内容。

在类中定义的成员变量和成员函数的作用域是整个类，这些名称只有在类中（包含类的定义部分和类外函数实现部分）是可见的，在类外是不可见的，因此，可以在不同类中使用相同的成员名。

另外，类作用域意味着不能从外部直接访问类的任何成员，即使该成员的访问权限是public，也要通过对象名来调用，对于static成员函数，要指定类名来调用。

### 类作用域
类定义的花括号 { } 之间就是一个类作用域，类内声明的所有成员（数据成员、成员函数、嵌套类型）都属于它。

```c++
class widget {
public:
    // 数据成员，在类作用域内
    int x = 1;

    // 成员函数体，也在类作用域内，可直呼 x
    void setX(int v) { x = v; }

    // 嵌套类，也在类作用域内
    class Inner{};


};


```
要点：
- 类内可直接用成员名，无需任何修饰（上例 x = v 直接用 x）。
- 类外必须加限定符：Widget::setX、w.x、p->x。裸写 x 在类外是找不到的。
- 作用域会向外延伸：即使在类外定义成员函数体，函数体内仍然属于类作用域：

```c++

// 类外定义，但函数体仍是类作用域
void Widget::setX(int v) {
    // 依然可以直接用 x
    x = v;              
}
```
### 类名作用域
说的是**类名**，比如我们的例子里，就是`Widget`，指的是，这个名字在哪些地方可见、能被当作类型使用。它和成员无关，是类名作用一个标识符的作用域。

```c++
// 未见类的声明，因此报错
Widget w();

// 类名 Widget 声明在全局作用域
class Widget {
public:
  Widget() {}
  ~Widget() {}

  // 类体内直接写 Widget
  Widget *self = nullptr;
};

// 类已经声明，所以可用
Widget w();
```
### 对象作用域
这是对象变量名的生死范围。说的是"对象变量"（如 w）这个标识符存在于哪个作用域，以及它在那里存活多久。

```c++
class Widget {
public:
  int x = 1;
};

Widget g; // ① 全局对象：声明在全局作用域，程序结束才销毁

int main() {
  Widget w1;   // ② 块对象：名字只在 main 的块作用域内可见
  {            //    —— 内层块开始
    Widget w2; // ③ 内层块对象：只在内层块内可见
  } //    w2 在此销毁（离开块作用域）
  // w2 在这里已不存在，访问会报错
  return 0; // w1 在此销毁，g 在 main 之后销毁
}

```


### 内部类应用之 Pimpl 设计模式

PIMPL（Private Implementation 或Pointer to Implementation）是通过一个私有的成员指针，将指针所指向的类的内部实现数据进行隐藏。PIMPL又称作“编译防火墙”、“云中猫cheshire cat”，它的实现中就用到了嵌套类。它也是最经典的嵌套类工程应用之一。

传统的方式中，我们在头文件中插入了我们需要的所有头文件：

```c++
#include <iostream>
#include <string>
#include <vector>

class Widget {
private:
  std::string str;
  std::vector<int> myInt;
};
```
此时，我们发现，这样的实现方式有三个痛点：
- 头文件强制包含一堆无关头，任何 include 了 widget.h 的文件都得跟着编译它们，拖慢构建。
- 改动私有成员（哪怕只是换个内部容器）会触发所有使用方重新编译。
- 实现细节完全暴露，想隐藏内部库、算法都做不到。

Pimpl设计模式，就可以一次性解决上面的三个痛点。它的实现方式，使用一个指针，将所有的实现方式隐藏起来：


```c++
// Widget.h
#pragma once
class Widget {
public:
  Widget() {}
  ~Widget() {}

private:
  // 当然 struct 也是可以的
  // 这里只做前置声明，实现放进 cpp 实现文件
  class WidgetPimpl;
  WidgetPimpl *_pimpl;
};
```


```c++
// PimplDesign.cc
#include "Pimp.h"

class Widget::WidgetPimpl {
public:
  WidgetPimpl(int x, int y) : _x(x), _y(y) {}
  void printWidgetPimpl() const;
  ~WidgetPimpl() {}

private:
  int _x;
  int _y;
};

void Widget::WidgetPimpl::printWidgetPimpl() const {
  cout << "_x is " << _x << ", _y is " << _y << endl;
}

Widget::Widget() : _pimpl(new WidgetPimpl(1, 2)) {}

Widget::~Widget() {
  if (_pimpl) {
    delete _pimpl;
    _pimpl = nullptr;
  }
}
Widget::Widget(
    const Widget
        &other) // 拷贝构造
                // WidgetPimpl 也要有可用的拷贝构造，因为数据成员是int，所以省略
    : _pimpl(new WidgetPimpl(*other._pimpl)) {}

Widget &Widget::operator=(const Widget &other) { // 拷贝赋值
  if (this != &other) {                          // 防自赋值
    delete _pimpl;                               // 释放旧内存
    _pimpl = new WidgetPimpl(*other._pimpl);
  }
  return *this;
}
```
#### 
PIMPL设计模式有如下优点：
1. 提高编译速度；
2. 实现信息隐藏；
3. 减小编译依赖，可以用最小的代价平滑的升级库文件；
4. 接口与实现进行解耦；
5. 移动语义友好。


#### Pimpl更现代的写法

传统写法的核心痛点是：裸指针 + 手动 `new`/`delete`，既要防忘了 `delete`，又要防双重释放，还得手写完整的五法则（析构、拷贝构造、拷贝赋值、移动构造、移动赋值）。现代写法用**智能指针 `std::unique_ptr` + 移动语义 + `noexcept`**，把大部分样板代码交给编译器，安全性也更高。

##### 1. 头文件（对外暴露的接口）

```c++
// Widget.h
#pragma once
#include <memory>   // 引入 std::unique_ptr

class Widget {
public:
  Widget();
  ~Widget();

  Widget(Widget &&other) noexcept;            // 移动构造
  Widget &operator=(Widget &&other) noexcept; // 移动赋值
  Widget(const Widget &other);                // 拷贝构造
  Widget &operator=(const Widget &other);     // 拷贝赋值

  void print() const;

private:
  class WidgetPimpl;
  std::unique_ptr<WidgetPimpl> _pimpl;        // 智能指针替代裸指针
};
```

##### 2. 实现文件（隐藏的实现）

```c++
// PimplDesign.cc
#include "Widget.h"

#include <iostream>
#include <memory>

using std::cout;
using std::endl;

class Widget::WidgetPimpl {
public:
  WidgetPimpl(int x, int y) : _x(x), _y(y) {}
  void printWidgetPimpl() const;

private:
  int _x;
  int _y;
};

void Widget::WidgetPimpl::printWidgetPimpl() const {
  cout << "_x is " << _x << ", _y is " << _y << endl;
}

// 构造：make_unique 一步完成 "分配内存 + 构造对象"
Widget::Widget() : _pimpl(std::make_unique<WidgetPimpl>(1, 2)) {}

// 析构：必须放在 .cc 且写 = default（原因见下文 新知识点1）
Widget::~Widget() = default;

// 移动：unique_ptr 天然支持移动，直接 default
Widget::Widget(Widget &&other) noexcept = default;
Widget &Widget::operator=(Widget &&other) noexcept = default;

// 拷贝：unique_ptr 不可拷贝，必须手动深拷贝
Widget::Widget(const Widget &other)
    : _pimpl(std::make_unique<WidgetPimpl>(*other._pimpl)) {}

Widget &Widget::operator=(const Widget &other) {
  if (this != &other)
    _pimpl = std::make_unique<WidgetPimpl>(*other._pimpl);
  return *this;
}

void Widget::print() const {
  _pimpl->printWidgetPimpl();
}
```

##### 3. 用法验证

```c++
int main() {
  Widget w1;
  w1.print();                    // 输出: _x is 1, _y is 2

  Widget w2 = w1;                // 拷贝构造（深拷贝）
  Widget w3 = std::move(w1);     // 移动构造（w1 的资源所有权转给 w3）
  return 0;                      // 所有 Widget 析构时自动释放各自的 pimpl
}
```

##### 4. 新旧写法对比

| 维度 | 传统写法 | 现代写法 |
|------|----------|----------|
| 指针类型 | `WidgetPimpl *`（裸指针） | `std::unique_ptr<WidgetPimpl>` |
| 内存释放 | 手动 `delete`（易忘/易双删） | 智能指针析构自动释放 |
| 析构函数 | 手写 `delete` 逻辑 | `= default` |
| 移动语义 | 无 | 移动构造 + 移动赋值（`noexcept`） |
| 代码量 | 多、易错 | 少、安全 |

##### 5. 新知识点详解

###### 新知识点 1：智能指针 std::unique_ptr

- **含义**：`unique_ptr` 是"独占所有权"的智能指针，同一时刻只有一个 `unique_ptr` 拥有所指向的对象。
- **RAII**：资源获取即初始化。`unique_ptr` 自身析构时，会自动 `delete` 它指向的对象，无需手动释放。
- **不能拷贝，可以移动**：独占意味着拷贝两个指针指向同一块内存是不允许的（会双重释放），但所有权可以"搬移"。
- **make_unique**：`std::make_unique<T>(args...)` 一步完成分配 + 构造，比 `new` 更安全（不会出现分配成功但构造失败的内存泄漏）。

```c++
// 手动 new 的写法
std::unique_ptr<WidgetPimpl> p1(new WidgetPimpl(1, 2));
// 推荐：make_unique 的写法
auto p2 = std::make_unique<WidgetPimpl>(1, 2);

// unique_ptr 禁止拷贝：
// std::unique_ptr<WidgetPimpl> p3 = p1;  // 编译错误！
// 只能移动（所有权转移）：
std::unique_ptr<WidgetPimpl> p3 = std::move(p1); // 现在 p3 拥有它，p1 为空
```

**关键点**：正因为 `unique_ptr` 在析构时要求所指向类型是"完整类型"，而 `_pimpl` 在头文件里只是前置声明（不完整类型），所以**析构函数必须在 .cc 文件里写成 `= default`**——在那里 `WidgetPimpl` 已定义完整，编译器才知道如何 `delete`。

###### 新知识点 2：移动语义（move semantics）

- **左值 vs 右值**：能取地址、有名字的表达式叫左值（如 `w1`）；临时对象、字面量这类"用完即弃"的值叫右值（如 `Widget()`、`1`）。
- **右值引用 `&&`**：专门用来绑定右值，从而"捕获"临时对象。
- **移动构造 `Widget(Widget &&other)`**：把 `other` 的资源（这里是 pimpl 指针）"搬"到自己手里，而不是复制一份数据。搬完后通常要把 `other` 置空，避免它析构时二次释放。
- **好处**：对大对象、资源类（含指针成员）来说，拷贝要重新分配 + 复制，移动只是换手指针，成本极低。

```c++
Widget w1;
Widget w2 = w1;             // 拷贝构造：w1 不动，w2 深拷贝一份
Widget w3 = std::move(w1);  // 移动构造：w1 的资源被搬给 w3，w1 通常被置空
```

> `std::move` 本身不搬任何东西，它只是把左值"标记"成右值，让编译器调用移动构造/移动赋值而不是拷贝。

###### 新知识点 3：noexcept 关键字

- **含义**：声明"这个函数保证不抛出异常"。若它内部真的抛异常，程序会直接终止（`std::terminate`）。
- **为什么移动构造要标 `noexcept`**：标准容器（如 `std::vector`）扩容搬迁元素时，会优先使用移动；但只有元素的移动操作被声明为 `noexcept`，容器才敢采用移动，否则会保守地退回拷贝。标了 `noexcept`，容器才敢放心用"移动"，从而享受它的高性能。
- **顺带一提**：C++11 之后析构函数默认是 `noexcept` 的，不用显式标注。

```c++
Widget(Widget &&other) noexcept;   // 告诉编译器：移动操作不会抛异常
Widget &operator=(Widget &&other) noexcept;
```

##### 6. 小结

现代 Pimpl 的写法和传统写法对比：

- **析构 + 移动**：靠 `unique_ptr` 和 `= default` 免费获得，不再手写 `delete`；
- **拷贝**：`unique_ptr` 禁止拷贝，所以只保留手动深拷贝；
- **安全性**：由"手动管理资源"升级为"RAII 自动管理资源"，从根源上消除忘记释放 / 双重释放的问题。

一句总结，这就是把类的私有成员全部打包进一个嵌套的实现类，对外只暴露一个指向这个实现类的指针，从而让类的头文件不暴漏任何实现细节和依赖。

### 内部类之单例模式的自动释放
传统单例模式的的实现方式就是将构造函数和析构函数都私有化，这样来防止栈对象和堆对象的新建。
传统代码为：
```c++
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class MySingleton {
public:
  void print() { cout << "This print()" << endl; }

  static MySingleton &getInstance() {
    if (nullptr == _pinstance) {
      _pinstance = new MySingleton();
    }
    return *_pinstance;
  }

  MySingleton(const MySingleton &c) = delete;
  MySingleton &operator=(const MySingleton &c) = delete;

  static void destroy() {
    if (nullptr != _pinstance) {
      delete _pinstance;
      _pinstance = nullptr;
    }
  }

private:
  MySingleton() { cout << "this is MySingleton()." << endl; }
  ~MySingleton() { cout << "this is ~MySingleton()." << endl; }
  static MySingleton *_pinstance;
};

MySingleton *MySingleton::_pinstance = nullptr;

int main(int argc, char *argv[]) {
  MySingleton &ms1 = MySingleton::getInstance();
  MySingleton &ms2 = MySingleton::getInstance();
  MySingleton &ms3 = MySingleton::getInstance();

  ms1.destroy();
  ms2.destroy();
  ms3.destroy();

  return 0;
}
```

上面的传统代码里，创建靠 `getInstance()`，释放靠手动调用 `destroy()`。这里就引出了一个问题：**万一调用方忘了调 `destroy()`，堆上的实例就泄漏了**。于是我们需要"自动释放"。

#### 为什么需要自动释放

传统单例存在两个天生的隐患：

1. **内存泄漏**：`_pinstance = new MySingleton()` 在堆上分配，但没有任何机制保证它会 `delete`。手动调用 `destroy()` 依赖调用方"记得"，一旦遗漏就泄漏，且程序运行越久泄漏越明显。
2. **谁负责、何时释放**：单例是全局共享对象，没有明确的"所有者"。没有一个自然的作用域来回收它——它不像局部变量那样离开作用域自动析构。

所以在 C++ 中，单例这种"没有 owner"的对象，需要**借 C++ 现成的自动回调点**（局部静态对象析构、`atexit` 注册、静态对象析构）把释放时机固定下来，这就是"自动释放"。

自动释放的核心思想就一句话：**把 `delete _pinstance` 挂到 C++ 程序退出时一定会自动执行的某个节点上**。

#### 自动释放的四种方法

##### 方法一：局部静态变量（Meyers' Singleton）

函数内的静态局部变量有两大特性：**首次调用时构造、程序退出时自动析构**。这就是最优雅的自动释放——连指针、`delete`、`destroy()` 全都不需要了。

```c++
#include <iostream>
using std::cout;
using std::endl;

class MySingleton {
public:
  void print() { cout << "This print()" << endl; }

  static MySingleton &getInstance() {
    static MySingleton instance;   // ① 首次调用构造
    return instance;               // ② 程序退出时自动析构
  }

  MySingleton(const MySingleton &c) = delete;
  MySingleton &operator=(const MySingleton &c) = delete;

private:
  MySingleton() { cout << "this is MySingleton()." << endl; }
  ~MySingleton() { cout << "this is ~MySingleton()." << endl; }
};

int main() {
  MySingleton &ms1 = MySingleton::getInstance();
  ms1.print();
  return 0;   // 无需手动释放，instance 自动析构
}
```

运行结果：
```
this is MySingleton().
This print()
this is ~MySingleton().
```

> 附带收益：C++11 起，局部静态变量的初始化是**线程安全**的，天然解决了懒汉模式的并发问题。这是最推荐的写法。

##### 方法二：atexit 注册销毁函数

`atexit(函数指针)` 可以把一个函数注册到"程序退出时自动调用"的清单里，程序结束时按注册顺序的反序自动执行。用它来注册 `destroy` 即可。

```c++
#include <iostream>
#include <cstdlib>   // atexit 头文件
using std::cout;
using std::endl;

class MySingleton {
public:
  static MySingleton *getInstance() {
    if (nullptr == _pinstance) {
      _pinstance = new MySingleton();
      atexit(destroy);            // 首次创建时注册，程序退出时自动调用
    }
    return _pinstance;
  }

  static void destroy() {
    if (nullptr != _pinstance) {
      delete _pinstance;
      _pinstance = nullptr;
    }
  }

  MySingleton(const MySingleton &c) = delete;
  MySingleton &operator=(const MySingleton &c) = delete;

private:
  MySingleton() { cout << "this is MySingleton()." << endl; }
  ~MySingleton() { cout << "this is ~MySingleton()." << endl; }
  static MySingleton *_pinstance;
};

MySingleton *MySingleton::_pinstance = nullptr;

int main() {
  MySingleton *ms1 = MySingleton::getInstance();
  ms1->print();
  return 0;   // 程序退出 → 自动调用 destroy() → delete 实例
}
```

> 注意：懒汉 + atexit 时，`new` 和 `atexit` 注册都发生在运行时，多线程下需要配合锁或 `call_once` 才线程安全。

##### 方法三：嵌套类 Garbo（垃圾回收类）

利用"**静态对象程序退出时自动析构**"的特性：在单例内部嵌套一个 `Garbo` 类，它的析构函数负责 `delete` 单例。因为嵌套类是单例的"内部类"，天然可以访问单例的私有成员，不需要 friend。

```c++
#include <iostream>
using std::cout;
using std::endl;

class MySingleton {
public:
  static MySingleton *getInstance() {
    if (nullptr == _pinstance) {
      _pinstance = new MySingleton();
    }
    return _pinstance;
  }

  void print() { cout << "This print()" << endl; }

  MySingleton(const MySingleton &c) = delete;
  MySingleton &operator=(const MySingleton &c) = delete;

private:
  MySingleton() { cout << "this is MySingleton()." << endl; }
  ~MySingleton() { cout << "this is ~MySingleton()." << endl; }

  static MySingleton *_pinstance;

  class Garbo {                    // 嵌套回收类
  public:
    ~Garbo() {                     // 静态对象退出时自动析构 → 触发这里
      if (nullptr != MySingleton::_pinstance) {
        delete MySingleton::_pinstance;
        MySingleton::_pinstance = nullptr;
      }
    }
  };
  static Garbo _garbo;             // 关键：静态成员对象，程序结束时自动析构
};

MySingleton *MySingleton::_pinstance = nullptr;
MySingleton::Garbo MySingleton::_garbo;   // 静态成员也要在类外定义

int main() {
  MySingleton *ms1 = MySingleton::getInstance();
  ms1->print();
  return 0;   // _garbo 自动析构 → Garbo 析构函数 delete 掉实例
}
```

释放链条：`main` 结束 → 静态成员 `_garbo` 自动析构 → 调用 `Garbo::~Garbo()` → 在函数里 `delete _pinstance`。这也是嵌套类应用的又一典型场景。

##### 方法四：友元类回收

和 Garbo 思路相同，但回收类定义在**外部**，通过 `friend` 友元声明获得访问私有析构的权限。

```c++
#include <iostream>
using std::cout;
using std::endl;

class MySingleton {
  friend class Recycle;            // 授权外部 Recycle 访问私有成员

public:
  static MySingleton *getInstance() {
    if (nullptr == _pinstance) {
      _pinstance = new MySingleton();
    }
    return _pinstance;
  }

  void print() { cout << "This print()" << endl; }

  MySingleton(const MySingleton &c) = delete;
  MySingleton &operator=(const MySingleton &c) = delete;

private:
  MySingleton() { cout << "this is MySingleton()." << endl; }
  ~MySingleton() { cout << "this is ~MySingleton()." << endl; }
  static MySingleton *_pinstance;
};

MySingleton *MySingleton::_pinstance = nullptr;

class Recycle {                    // 外部回收类
public:
  ~Recycle() {
    if (nullptr != MySingleton::_pinstance) {
      delete MySingleton::_pinstance;   // 友元可调用私有析构
      MySingleton::_pinstance = nullptr;
    }
  }
};

static Recycle recycle;            // 静态对象，程序退出时自动析构 → 触发回收

int main() {
  MySingleton *ms1 = MySingleton::getInstance();
  ms1->print();
  return 0;   // recycle 自动析构 → Recycle 析构函数 delete 掉实例
}
```

> 与 Garbo 的区别：Garbo 是嵌套类，天然能访问私有成员；Recycle 是外部类，必须靠 `friend` 声明才能访问私有析构。代价是 friend 破坏了一点封装性。

#### 自动释放单例的最优写法

**结论：方法一（Meyers' 局部静态变量）是最优解。**

原因：
1. **代码最少**：没有指针成员、没有 `delete`、没有 `destroy()`、没有额外类；
2. **自动释放**：静态局部对象在程序退出时自动析构，绝不泄漏；
3. **线程安全**：C++11 起初始化线程安全，懒汉无需加锁；
4. **懒加载**：首次调用才创建，不占用启动开销。

```c++
#include <iostream>
using std::cout;
using std::endl;

class MySingleton {
public:
  static MySingleton &getInstance() {
    static MySingleton instance;   // 一行搞定：懒加载 + 线程安全 + 自动释放
    return instance;
  }

  void print() { cout << "This print()" << endl; }

  MySingleton(const MySingleton &c) = delete;
  MySingleton &operator=(const MySingleton &c) = delete;

private:
  MySingleton() { cout << "this is MySingleton()." << endl; }
  ~MySingleton() { cout << "this is ~MySingleton()." << endl; }
};

int main() {
  MySingleton &ms1 = MySingleton::getInstance();
  MySingleton &ms2 = MySingleton::getInstance();
  ms1.print();
  return 0;
}
```

各方法对比：

| 方法 | 自动释放 | 线程安全(C++11+) | 代码量 | 依赖 |
|------|:---:|:---:|:---:|------|
| 传统手动 destroy | ❌ 靠调用方 | ❌ | 中 | 无 |
| 局部静态变量（最优） | ✅ | ✅ | **最少** | 无 |
| atexit 注册 | ✅ | ❌ | 中 | `<cstdlib>` |
| 嵌套类 Garbo | ✅ | ❌ | 多 | 无 |
| 友元类 Recycle | ✅ | ❌ | 多 | 无 |

**结论：现代 C++ 首选 Meyers' 局部静态变量写法。** Garbo、atexit、友元这些方案更多用于理解"静态对象析构时机"和 C++11 之前的场景。

## string 的底层实现

实现string原本有几种方式，分别是：
- 深拷贝。就是将每一个string类型字符串都用一个新地址存储。
- 写时复制COW。用的原理是浅拷贝+引用计数。让很多字符串都指向同一片内存地址，只有进行写操作时，才申请新空间。2026 年的现在，由于多线程下引用计数有竞态、与 C++11 移动语义互斥（&s[0] 取指针再改就破坏了 COW 的承诺），所以GCC 在 GCC 5（2015）彻底移除了 COW 实现。
- 短字符串优化SSO。就是当字符串是短字符串的时候，让它直接在栈上，其核心思想是：发生拷贝时要复制一个指针，对小字符串来说，为啥不直接复制整个字符串呢，说不定还没有复制一个指针的代价大

我们要实现的，是`string`的写时复制原理，因为这里可以联系运算符的重载。




## 作业
编程题

1、实现Line类的PIMPL模式 	
[Pimpl.h](/TestCode/cppDay9/Pimpl.h)
[PimplDesign.cc](/TestCode/cppDay9/PimplDesign.cc)
[testPimpl.cc](/TestCode/cppDay9/testPimpl.cc)
2、实现单例模式的自动释放（4种方式）
[RecycleSingleton_1.cc](/TestCode/cppDay9/RecycleSingleton_1.cc)
[RecycleSingleton_2.cc](/TestCode/cppDay9/RecycleSingleton_2.cc)
[RecycleSingleton_3.cc](/TestCode/cppDay9/RecycleSingleton_3.cc)
[RecycleSingleton_4.cc](/TestCode/cppDay9/RecycleSingleton_4.cc)

3、实现COW的String，让其operator[]能够区分出读写操作

[stringCow.cc](/TestCode/cppDay9/stringCow.cc)
