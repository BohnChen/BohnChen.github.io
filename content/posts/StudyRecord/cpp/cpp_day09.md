---
title: "类域、"
date: 2026-08-27T15:25:15+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## 类型转换
类型转换函数

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




## string 的底层实现
