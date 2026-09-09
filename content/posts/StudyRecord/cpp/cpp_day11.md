---
title: "继承的构造与析构顺序、基类与派生类对象的转化、多基继承与菱形继承、派生类对象间的复制"
date: 2026-09-07T10:09:13+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## 继承的构造函数

### 构造函数不能被继承，只能被“调用”

先厘清一个关键概念：**基类的构造函数并不会“继承”到派生类里**。派生类里新增的数据成员，由派生类自己的构造函数负责初始化；而派生类对象中那一块“基类子对象”（回忆上一讲：派生类对象天然包含一份完整的基类数据），则**必须由基类的构造函数来初始化**。

于是就有了这一讲的结论：

> 简而言之，基类中的构造函数，靠派生类中的构造函数进行初始化。

具体分成两种情况：

### 情形一：基类、派生类都“没有”构造函数

当派生类和基类都没有写任何构造函数时，编译器会为两边各自合成默认无参构造函数。创建派生类对象时，合成的派生类构造函数会**自动调用基类的无参构造函数**，整个过程对用户透明：

```c++
#include <iostream>
using std::cout;
using std::endl;

class Base {};                    // 编译器生成 Base()
class Derived : public Base {};   // 编译器生成 Derived()，内部自动调用 Base()

int main() {
  Derived d;   // ✅ 一切正常，默认无参构造链自动完成
  return 0;
}
```

### 情形二：基类只有带参构造函数，派生类需要“显式调用”

一旦基类自己定义了带参构造函数，编译器就**不再自动生成无参的 Base()**。此时派生类若想把自己收到的参数传给基类，必须在**派生类构造函数的成员初始化列表中显式写出基类构造函数**：

```c++
#include <iostream>
using std::cout;
using std::endl;

class Base {
public:
  Base(int x) : _x(x) { cout << "Base(int " << _x << ")" << endl; }
  void print() const { cout << "_x = " << _x << endl; }

private:
  int _x;
};

class Derived : public Base {
public:
  // 重点：在初始化列表里显式写出基类构造函数 Base(a)
  Derived(int a, int b) : Base(a), _y(b) {
    cout << "Derived(int, int) 函数体" << endl;
  }

private:
  int _y;
};

int main() {
  Derived d(10, 20);
  d.print();
  return 0;
}
```

运行结果：

```
Base(int 10)
Derived(int, int) 函数体
_x = 10
```

如果漏掉初始化列表里的 `Base(...)`，就会编译报错，因为编译器不知道如何构造基类子对象：

```c++
class Base {
public:
  Base(int x) : _x(x) {}
  int _x;
};

class Derived : public Base {
public:
  Derived(int b) : _y(b) {}   // ❌ 没写 Base(...)
  int _y;
};
// 编译错误：基类 Base 没有可用的默认构造函数，
//          Derived 的构造函数必须显式初始化基类部分
```

### 总结

- 基类构造函数**不被继承**，但派生类对象中基类子对象的初始化必须调用它；
- 两者都没写构造 → 各自用合成默认构造，创建派生对象时自动调基类默认构造；
- 基类只有带参构造 → 派生类必须在**初始化列表**中显式写出 `Base(实参)`，否则编译失败；
- “调用”基类构造的本质是**初始化基类子对象**，不是构造出一个单独的基类临时对象。

### 补充：C++11 的“继承构造函数” `using Base::Base`

C++11 允许用 `using` 把基类的构造函数“搬”到派生类作用域，让派生类对象也能直接用基类的构造参数来构造：

```c++
class Base {
public:
  Base(int x) : _x(x) {}
  int _x;
};

class Derived : public Base {
public:
  using Base::Base;   // 继承 Base(int)，Derived 也能这样构造
};

int main() {
  Derived d(10);      // ✅ 等价于走 Base(int) 初始化基类部分
  return 0;
}
```

> 说明：`using Base::Base` 只“搬运”构造方式，派生类自己新增的成员仍然按缺省规则处理，一般用于“派生类暂不需要新增初始化逻辑”的轻量场景，正常的多基继承课程中了解即可。

## 特殊类成员的初始化顺序

“特殊类成员”指的是**类对象数据成员**（数据成员本身是某个类的对象，有些教材叫“内部类对象”）。当一个派生类既有基类、又有这样的类对象成员时，各部分的构造顺序是固定的：

**派生类对象的构造顺序（三阶段）：**
1. **先构造基类子对象**（有多个基类时按继承列表从左到右）；
2. **再按“成员声明的顺序”构造各数据成员**（类对象成员在这里被构造）；
3. **最后才执行派生类构造函数体**。

```c++
#include <iostream>
using std::cout;
using std::endl;

class Member {
public:
  Member(int v) : _v(v) { cout << "  Member(int " << _v << ")" << endl; }
  ~Member() { cout << "  ~Member()" << endl; }
private:
  int _v;
};

class Base {
public:
  Base() { cout << "  Base()" << endl; }
  ~Base() { cout << "  ~Base()" << endl; }
};

class Derived : public Base {
public:
  // 故意把初始化列表顺序写成 _m2 在前、_m1 在后
  Derived() : _m2(2), _m1(1) { cout << "Derived() 函数体" << endl; }
  ~Derived() { cout << "~Derived()" << endl; }
private:
  Member _m1;   // 先声明
  Member _m2;   // 后声明
};

int main() {
  Derived d;
  return 0;
}
```

运行结果：

```
  Base()
  Member(int 1)
  Member(int 2)
Derived() 函数体
```

观察要点：
1. **Base 一定先于任何数据成员构造**（基类子对象是“地基”）；
2. `_m1` 先于 `_m2` 构造，**尽管初始化列表里写的是 `_m2(2), _m1(1)`** —— 实际构造顺序只与**成员声明的先后顺序**有关，与初始化列表书写顺序无关；
3. 构造函数体在最后执行。

### 补充

- 初始化列表的书写顺序**不会改变实际调用顺序**。若把列表顺序写乱，编译器会给出 `-Wreorder` 警告（字段实际会先于列表顺序初始化），所以**书写顺序应尽量与声明顺序一致**，既避免告警也更易读。
- 深层含义：基类构造函数里若想访问派生类成员是“不可能的”（此时派生类成员尚未构造）；这也是 C++ 建议不要在构造函数里调用虚函数的原因之一（后面多态章节会再见到）。

## 继承的析构函数

析构函数的执行顺序与构造函数**恰好相反**，先析构的“最后构造”：

**派生类对象的析构顺序（三阶段）：**
1. **先执行派生类析构函数体**；
2. **再按“成员声明的逆序”析构各数据成员**；
3. **最后析构基类子对象**（有多个基类时按继承列表从右到左）。

```c++
#include <iostream>
using std::cout;
using std::endl;

class Base {
public:
  ~Base() { cout << "~Base()" << endl; }
};

class Member {
public:
  ~Member() { cout << "~Member()" << endl; }
};

class Derived : public Base {
public:
  ~Derived() { cout << "~Derived()" << endl; }   // 析构顺序见运行结果
private:
  Member _m;
};

int main() {
  Derived d;
  return 0;
}
```

运行结果：

```
~Derived()
~Member()
~Base()
```

为什么一定要相反？因为析构时假设对象还“完整可用”：派生类析构函数体可能要使用自己的成员，而成员可能还要使用基类部分提供的功能，所以必须从“最外层/最具体”往“最基础/最底层”逐层回收。

### 补充

- 编译器会在派生类析构函数**末尾自动插入**成员对象析构与基类析构的调用，用户无需也不能手动重复调用基类析构。
- 析构函数本身也不会被“继承”；而且与构造函数不同，基类析构函数建议声明为 **`virtual`**，否则将来“通过基类指针 `delete` 派生类对象”时只会调用基类析构函数、漏掉派生类的清理——这是多态章节的重点内容，这里先记住结论。

## 基类与派生类对象的转化

派生类对象与基类对象之间可以互相“转化”吗？答案分方向看，**向上的转化（派生 → 基类）被大力支持，向下的转化（基类 → 派生）被严格限制**。

### ① 派生类对象 → 基类对象：发生“对象切割”

用派生类对象去拷贝构造或赋值一个基类对象，编译器会做**向上转换**，把派生类对象当作基类对象来用。由于基类对象里装不下派生类的新增成员，**只拷贝/赋值“基类部分”**，派生类新增部分被丢弃——这个现象叫**对象切割（slicing）**：

```c++
#include <iostream>
using std::cout;
using std::endl;

class Base {
public:
  Base(int b = 0) : _b(b) { cout << "  Base(int " << _b << ")" << endl; }
  Base(const Base &rhs) : _b(rhs._b) { cout << "  Base(const Base&)" << endl; }
  Base &operator=(const Base &rhs) {
    _b = rhs._b;
    cout << "  Base::operator=" << endl;
    return *this;
  }
  void print() const { cout << "    _b = " << _b << endl; }
  int _b;
};

class Derived : public Base {
public:
  Derived(int b = 0, int d = 0) : Base(b), _d(d) {}
  int _d;
};

int main() {
  Derived d(10, 20);

  cout << "① 用派生类对象初始化基类对象（切割）" << endl;
  Base b1(d);        // 调用的是 Base 的拷贝构造函数
  b1.print();        // 只有 _b = 10，_d = 20 被丢弃

  cout << "② 派生类对象赋给基类对象（切割）" << endl;
  Base b2;
  b2 = d;            // 调用的是 Base::operator=
  b2.print();

  cout << "③ 派生类对象绑定到基类引用/指针（不切割）" << endl;
  Base &ref = d;     // 引用绑定的是“同一个”对象，不产生新对象
  Base *ptr = &d;
  ptr->_b = 100;     // 通过基类指针修改的仍是 d 的基类部分
  cout << "    d._b = " << d._b << endl;   // 100，说明就是 d 本体
  return 0;
}
```

运行结果：

```
① 用派生类对象初始化基类对象（切割）
  Base(int 10)
  Base(const Base&)
    _b = 10
② 派生类对象赋给基类对象（切割）
  Base(int 0)
  Base::operator=
    _b = 10
③ 派生类对象绑定到基类引用/指针（不切割）
    d._b = 100
```

关键区分：
- **值传递/值拷贝（`Base b = d`、`b = d`）→ 切割**：新对象只是一份“只有基类部分”的副本，派生类的新增数据彻底丢失；
- **引用/指针绑定（`Base &r = d`、`Base *p = &d`）→ 不切割**：仍然指向同一个完整派生类对象，只是“视野收窄”成基类那部分，只能访问基类成员。

### ② 基类对象 → 派生类对象：隐式转换不允许

反过来，基类对象里根本**没有**派生类新增的那部分成员，无法凭空补全，所以编译器不允许隐式转换：

```c++
Base b(5);
// Derived d2 = b;   // ❌ 编译错误：无法从 Base 构造 Derived
// Derived d3; d3 = b; // ❌ 编译错误：Base 不能赋给 Derived
```

> 基类指针/引用 → 派生类指针/引用（向下转换）同样不能隐式进行。若你确信原对象确实是派生类对象，可以显式 `static_cast<Derived*>(&b)` 强行转换；若不确定，则需要 `dynamic_cast` 在运行时做安全检查（要求基类含虚函数，属多态章节内容）。

### 小结

| 转换方向 | 写法 | 结果 |
|---|---|---|
| 派生 → 基类 | `Base b = d;` / `b = d;` | ✅ 允许，发生**切割**，只保留基类部分 |
| 派生 → 基类 | `Base &r = d;` / `Base *p = &d;` | ✅ 允许，**不切割**，指向原对象（向上转型） |
| 基类 → 派生 | `Derived d = b;` / `d = b;` | ❌ 不允许，缺派生类新增成员 |
| 基类 ptr/ref → 派生 | `(Derived*)&b` 等 | ⚠️ 必须显式转换，用错（对象实际是基类）属未定义行为 |

### 补充：切割的“坑”

函数**按值传参**同样会切割。例如 `void func(Base b)` 传入 `Derived d`，`func` 里拿到的只是 d 的基类副本，多态失效；想让函数处理完整的派生类对象，参数必须写成引用或指针 `void func(Base &b)`。

## 多基继承

C++ 允许一个派生类同时继承多个基类：

```c++
class D : public A, public B, public C {};
```

### 多基继承的权限

多基继承中**每一个基类都必须带上自己的继承方式**（`public/protected/private`）。因为 C++ 里用 `class` 声明时，**默认继承方式是 `private`**，稍微漏写一个，该基类的成员到类外就全部不可见了：

```c++
#include <iostream>
using std::cout;
using std::endl;

class A { public: void fa() { cout << "A::fa" << endl; } };
class B { public: void fb() { cout << "B::fb" << endl; } };
class C { public: void fc() { cout << "C::fc" << endl; } };

// 只有 A 前面写了 public；B、C 前面什么都没写 → 默认 private 继承
class D : public A, B, C {};

int main() {
  D d;
  d.fa();   // ✅ A 是 public 继承，可以调用
  // d.fb(); // ❌ B 是 private 继承，fb 对类外不可见
  // d.fc(); // ❌ C 是 private 继承，fc 对类外不可见
  return 0;
}
```

写清楚每个基类的权限才是规范做法（每个基类独占一行，方便阅读和排错）：

```c++
class D
    : public A
    , public B
    , public C
{};
```

> 提醒：`struct` 默认是 `public` 继承，而 `class` 默认是 `private` 继承。日常几乎总是需要 `public` 继承，务必逐个写上 `public`，不要依赖默认值。

### 多基继承的构造函数调用顺序

基类子对象的构造顺序**只与“继承列表（基类声明顺序）”有关，与派生类初始化列表的书写顺序无关**：

```c++
#include <iostream>
using std::cout;
using std::endl;

class A {
public:
  A(int x) : _x(x) { cout << "  A(int " << _x << ")" << endl; }
  ~A() { cout << "  ~A()" << endl; }
private:
  int _x;
};

class B {
public:
  B(int x) : _x(x) { cout << "  B(int " << _x << ")" << endl; }
  ~B() { cout << "  ~B()" << endl; }
private:
  int _x;
};

// 继承列表：A 在前、B 在后
class D : public A, public B {
public:
  // 初始化列表故意写成 B 在前、A 在后
  D(int a, int b) : B(b), A(a) { cout << "D(int, int) 函数体" << endl; }
  ~D() { cout << "~D()" << endl; }
};

int main() {
  D d(1, 2);
  return 0;
}
```

运行结果：

```
  A(int 1)
  B(int 2)
D(int, int) 函数体
~D()
  ~B()
  ~A()
```

尽管初始化列表先写了 `B(b)`，实际仍是**按继承列表先 A 后 B** 构造；析构则按相反顺序（B → A）。

### 多个基类中相同的成员函数访问

当多个基类定义了**同名成员函数**时，派生类对象直接调用会产生**二义性**——编译器不知道你想调哪一个：

```c++
#include <iostream>
using std::cout;
using std::endl;

class A { public: void print() const { cout << "A::print()" << endl; } };
class B { public: void print() const { cout << "B::print()" << endl; } };
class C { public: void print() const { cout << "C::print()" << endl; } };

class D : public A, public B, public C {};

int main() {
  D d;
  // d.print();   // ❌ 编译错误：三个基类都有 print()，无法确定调哪一个
  d.A::print();   // ✅ 用“基类名::”限定调用的是哪个基类的版本
  d.B::print();
  d.C::print();
  return 0;
}
```

运行结果：

```
A::print()
B::print()
C::print()
```

用 `对象.基类名::成员(...)`（或指针 `p->基类名::成员(...)`）可以显式指出要访问哪个基类版本，从而消除二义性。

### 菱形继承（存储二异性）

考虑这样一条继承结构：

```c++
class A { public: int _x; };
class B : public A {};          // B 内部包含一份 A 子对象
class C : public A {};          // C 内部包含一份 A 子对象
class D : public B, public C {}; // D 同时继承 B 和 C
```

B、C 各自都继承了 A，于是 **D 的对象里实际存在两份 A 的子对象**（一份来自 B，一份来自 C）。这份“重复存储”会带来两个问题：

1. **存储二异性（访问二义）**：`d._x` 时编译器无法确定你指的是 B 里那份 A，还是 C 里那份 A：

```c++
class A { public: int _x; void f() {} };
class B : public A {};
class C : public A {};
class D : public B, public C {};

int main() {
  D d;
  // d._x = 1;   // ❌ 编译错误：_x 在 B->A 和 C->A 两份子对象中同时存在
  // d.f();      // ❌ 同理二义
  return 0;
}
```

2. **状态重复与浪费**：如果 A 里放的是需要“全局唯一”的数据（比如一份共享的计数器），菱形继承会让它变成两份互不相干的副本，语义完全错误，也白白浪费空间。

解决手段就是下一节的**虚基继承**。

### 虚基继承与虚基指针

在 B、C 继承 A 时加上 `virtual`，让 A 成为**虚基类**：

```c++
class A { public: int _x; };
class B : virtual public A {};   // 虚继承 A
class C : virtual public A {};   // 虚继承 A
class D : public B, public C {}; // D 里只保留一份共享的 A 子对象
```

此时无论 D 经由几条路径继承 A，**D 的对象里只有一份 A 子对象**，`d._x`、`d.f()` 不再二义，空间也省了。

#### 内存布局：类的大小怎么变

虚继承靠给类增加一个**虚基指针（virtual base pointer，简称 vbptr）**来实现。以下面课堂里的例子来说明：

```c++
#include <iostream>
using std::cout;
using std::endl;

class A { public: double _idx = 0; };        // 只有 1 个 double → 8 字节

// —— 非虚继承 ——
class B1 : public A {};
class C1 : public A {};
class D1 : public B1, public C1 {};

// —— 虚继承 ——
class B2 : virtual public A {};
class C2 : virtual public A {};
class D2 : public B2, public C2 {};

int main() {
  cout << "sizeof(A)  = " << sizeof(A) << endl;
  cout << "sizeof(B1) = " << sizeof(B1) << endl;
  cout << "sizeof(C1) = " << sizeof(C1) << endl;
  cout << "sizeof(D1) = " << sizeof(D1) << endl;
  cout << "sizeof(B2) = " << sizeof(B2) << endl;
  cout << "sizeof(C2) = " << sizeof(C2) << endl;
  cout << "sizeof(D2) = " << sizeof(D2) << endl;
  return 0;
}
```

运行结果：

```
sizeof(A)  = 8
sizeof(B1) = 8
sizeof(C1) = 8
sizeof(D1) = 16
sizeof(B2) = 16
sizeof(C2) = 16
sizeof(D2) = 24
```

逐个解释：

- **A = 8 字节**：只有 1 个 `double _idx`，正好 8 字节；
- **B1 = 8、C1 = 8、D1 = 16（非虚继承）**：B1、C1 各自完整含有一份 A（各 8 字节），D1 继承两者 → `8 + 8 = 16`，对象里有两份 `_idx`，这就是“重复存储”的代价；
- **B2 = 16、C2 = 16（虚继承）**：在原来 A 数据的基础上，**多增加 1 个虚基指针（8 字节）**：`虚基指针(8) + _idx(8) = 16`；
- **D2 = 24**：D2 里有两份虚基指针（B2 一份、C2 一份），但 A 的 `_idx` 只有**一份共享**：`虚基指针(8) + 虚基指针(8) + _idx(8) = 24`，而不是非虚的 32（16+16）。

> 对比 D1 = 16 与 D2 = 24 的差别注意区分：非虚时两份 `_idx`（16 = 8 + 8）；虚继承后变成“两个指针 + 一份数据”（24 = 8 + 8 + 8）。虚继承用“多两个指针”的空间换来了“只存一份基类数据、消除二义”。

#### 虚基指针的工作方式

vbptr 指向一张**虚基类表（virtual base table）**，表里记录着“从当前对象到虚基类 A 子对象的偏移量”。运行时对象通过 vbptr 找到偏移，再定位到唯一的那份 A 子对象，从而完成访问。正因为 A 子对象是“共享”的，构造它的职责也交给了**最派生类 D**：当 D 构造时，由 D 统一初始化那份 A；作为中间层的 B2、C2 在作为 D 的基类部分构造时，不会再各自构造一份 A。

#### 补充

- **成员函数不占对象大小**：上述所有 sizeof 计算里都没有算任何成员函数，函数代码在代码段只存一份，不属于对象；
- **静态数据成员也不占对象大小**，同理它属于类而非对象；
- 上表的数值基于常见的 Itanium ABI（GCC / Clang 的默认实现）。C++ 标准只保证虚继承的**语义**（一份共享子对象、消除二义），并不保证具体布局，MSVC 等实现的内部细节可能略有差异，但“增加虚基指针来定位共享子对象”的思路是一致的。

## 派生类对象间的复制

### 背景：为什么这块内容要单独讲

结合前面的知识，先回顾两点：

1. 派生类对象内部有一块**完整的基类子对象**（见“基类与派生类对象的转化”），所以“派生类对象之间的复制”天然要处理两部分：**基类子对象** 与 **派生类自己新增的成员**；
2. **基类的拷贝构造函数和 `operator=` 运算符函数不能被派生类继承**。但派生类若没有自己定义它们，编译器会为派生类**自动合成**一份缺省的拷贝构造 / 赋值运算符；这份缺省版本内部会**自动调用基类对应的函数**来搞定基类子对象。

正是上面第 2 点，派生出课堂原文的“三种情况”。

### 课堂原文

> 从前面的知识，我们知道，基类的拷贝构造函数和 operator= 运算符函数不能被派生类继承，那么在执行派生类对象间的复制操作时，就需要注意以下几种情况：
>
> 1. 如果用户定义了基类的拷贝构造函数，而没有定义派生类的拷贝构造函数，那么在用一个派生类对象初始化新的派生类对象时，两对象间的派生类部分执行缺省的行为，而两对象间的基类部分执行用户定义的基类拷贝构造函数。
> 2. 如果用户重载了基类的赋值运算符函数，而没有重载派生类的赋值运算符函数，那么在用一个派生类对象给另一个已经存在的派生类对象赋值时，两对象间的派生类部分执行缺省的赋值行为，而两对象间的基类部分执行用户定义的重载赋值函数。
> 3. 如果用户定义了派生类的拷贝构造函数或者重载了派生类的对象赋值运算符=，则在用已有派生类对象初始化新的派生类对象时，或者在派生类对象间赋值时，将会执行用户定义的派生类的拷贝构造函数或者重载赋值函数，而不会再自动调用基类的拷贝构造函数和基类的重载对象赋值运算符，这时，通常需要用户在派生类的拷贝构造函数或者派生类的赋值函数中显式调用基类的拷贝构造或赋值运算符函数。

下面把三种情况逐个翻译成大白话，并用代码验证。

### 情形一：只自定义了“基类的拷贝构造”，派生类没定义

派生类的拷贝构造由编译器自动生成，生成时它**会自动去调用基类那个用户自定义的拷贝构造函数**。结果就是：**派生类自己新增的成员按缺省的逐成员方式拷贝，基类子对象则走用户定义的基类拷贝构造。**

```c++
#include <iostream>
using std::cout;
using std::endl;

class Base {
public:
  Base(int b = 0) : _b(b) {}
  Base(const Base &rhs) : _b(rhs._b) {          // 用户自定义的基类拷贝构造
    cout << "  Base(const Base&)   [用户自定义]" << endl;
  }
  int _b;
};

class Derived : public Base {
public:
  Derived(int b = 0, int d = 0) : Base(b), _d(d) {}
  void show() const { cout << "    _b = " << _b << ", _d = " << _d << endl; }
  int _d;   // 派生类自己新增的成员
};

int main() {
  Derived d1(100, 200);
  Derived d2(d1);   // 派生类没有自定义拷贝构造 → 编译器自动生成
  d2.show();
  return 0;
}
```

运行结果：

```
  Base(const Base&)   [用户自定义]
    _b = 100, _d = 200
```

结论与原文一致：**派生类部分 `_d` 执行了缺省的逐成员拷贝，基类部分 `_b` 走的是用户自定义的基类拷贝构造**（日志里打印了那条调用）。

### 情形二：只重载了“基类的赋值运算符”，派生类没重载

同理，派生类的 `operator=` 由编译器自动生成，内部**会自动调用基类用户重载的 `operator=`**：派生类新增成员按缺省逐成员赋值，基类子对象走用户定义的基类赋值函数。

```c++
#include <iostream>
using std::cout;
using std::endl;

class Base {
public:
  Base(int b = 0) : _b(b) {}
  Base &operator=(const Base &rhs) {            // 用户重载的基类赋值
    _b = rhs._b;
    cout << "  Base::operator=     [用户重载]" << endl;
    return *this;
  }
  int _b;
};

class Derived : public Base {
public:
  Derived(int b = 0, int d = 0) : Base(b), _d(d) {}
  void show() const { cout << "    _b = " << _b << ", _d = " << _d << endl; }
  int _d;
};

int main() {
  Derived d1(1, 2);
  Derived d2;        // 已经存在的派生类对象
  d2 = d1;           // 派生类没有自定义 operator= → 编译器自动生成
  d2.show();
  return 0;
}
```

运行结果：

```
  Base::operator=     [用户重载]
    _b = 1, _d = 2
```

结论与原文一致：**派生类部分 `_d` 走缺省赋值，基类部分 `_b` 走用户重载的基类赋值函数**。

### 情形三：自定义了“派生类的拷贝构造或赋值运算符”

一旦派生类**自己**定义了拷贝构造或重载了 `operator=`，编译器的“自动生成 + 自动调基类”机制就不再介入。此时基类部分**不会自动被拷贝/赋值**，需要用户在派生类版本里**显式调用**基类对应函数。如果忘了显式调用，后果很隐蔽：拷贝构造里基类子对象会退回去走基类默认构造函数（基类数据往往停留在默认值 0），赋值里基类子对象则**完全不会被更新**。

```c++
#include <iostream>
using std::cout;
using std::endl;

class Base {
public:
  Base(int b = 0) : _b(b) { cout << "  Base(int " << _b << ")" << endl; }
  Base(const Base &rhs) : _b(rhs._b) { cout << "  Base(const Base&)   [用户自定义]" << endl; }
  Base &operator=(const Base &rhs) {
    cout << "  Base::operator=     [用户自定义]" << endl;
    _b = rhs._b;
    return *this;
  }
  int _b;
};

// 反例：自定义了拷贝构造/赋值，但【没有】显式调用基类版本
class DerivedBad : public Base {
public:
  DerivedBad(int b = 0, int d = 0) : Base(b), _d(d) {}
  DerivedBad(const DerivedBad &rhs) : _d(rhs._d) {   // ❌ 漏写 Base(rhs)
    cout << "  DerivedBad 拷贝构造 (没调 Base 拷贝构造)" << endl;
  }
  DerivedBad &operator=(const DerivedBad &rhs) {     // ❌ 漏写 Base::operator=(rhs)
    if (this != &rhs) {
      _d = rhs._d;
      cout << "  DerivedBad operator= (没调 Base::operator=)" << endl;
    }
    return *this;
  }
  void show() const { cout << "    _b = " << _b << ", _d = " << _d << endl; }
  int _d;
};

// 正例：自定义的同时【显式】调用基类版本
class DerivedGood : public Base {
public:
  DerivedGood(int b = 0, int d = 0) : Base(b), _d(d) {}
  DerivedGood(const DerivedGood &rhs) : Base(rhs), _d(rhs._d) {  // ✅ Base(rhs)
    cout << "  DerivedGood 拷贝构造 (显式调用 Base(rhs))" << endl;
  }
  DerivedGood &operator=(const DerivedGood &rhs) {
    if (this != &rhs) {
      Base::operator=(rhs);   // ✅ 显式调用基类赋值
      _d = rhs._d;
      cout << "  DerivedGood operator= (显式调用 Base::operator=)" << endl;
    }
    return *this;
  }
  int _d;
};

int main() {
  DerivedBad a(10, 20), b;
  DerivedBad c(a);
  c.show();           // ❌ _b 仍是 0：基类部分没被拷贝！
  b = a;
  b.show();           // ❌ _b 仍是 0：基类部分完全没被赋值！

  cout << "----------------" << endl;
  DerivedGood x(10, 20), y;
  DerivedGood z(x);   // ✅ 基类部分被正确拷贝
  y = x;              // ✅ 基类部分被正确赋值
  return 0;
}
```

运行结果（节选关键行）：

```
DerivedBad 拷贝构造 (没调 Base 拷贝构造)
    _b = 0, _d = 20      ← 基类部分丢成了默认 0！
DerivedBad operator= (没调 Base::operator=)
    _b = 0, _d = 20      ← 基类部分根本没被更新！
----------------
DerivedGood 拷贝构造 (显式调用 Base(rhs))
DerivedGood operator= (显式调用 Base::operator=)
```

### 三种情况对照表

| 用户的自定义情况 | 派生类新增部分怎么处理 | 基类子对象怎么处理 | 是否需要显式调用基类 |
|---|---|---|---|
| 只定义基类拷贝构造，派生类未定义 | 缺省逐成员拷贝 | 自动调用户定义的基类拷贝构造 | 否 |
| 只重载基类 `operator=`，派生类未重载 | 缺省逐成员赋值 | 自动调用户重载的基类赋值函数 | 否 |
| 派生类自定义了拷贝构造/赋值 | 按派生类自定义的代码执行 | **不再自动调用**，漏写则基类部分丢失/不更新 | **是**：拷贝构造写 `: Base(rhs)`，赋值里写 `Base::operator=(rhs)` |

### 补充：派生类自定义时最容易被坑的三件事

1. **拷贝构造漏写 `: Base(rhs)`** → 基类子对象走默认构造，基类数据退化为默认值（如上例 `_b = 0`），表面不报错，数据悄悄丢失；
2. **赋值函数漏写 `Base::operator=(rhs)`** → 基类子对象完全不被赋值，两个派生类对象基类部分永远不相等；
3. **防自赋值**：派生类自定义 `operator=` 时同样要先判断 `this != &rhs`，再调用基类赋值，避免自我赋值时出现意外。

## 本章总结

- **构造函数不继承、但靠调用**：派生类默认自动调基类无参构造；基类只有带参构造时必须在初始化列表显式 `Base(实参)`。
- **初始化顺序三阶段**：基类子对象（按继承列表）→ 成员对象（按声明顺序，与初始化列表书写顺序无关）→ 构造函数体；析构严格反向。
- **转化分方向**：派生 → 基类的值拷贝/赋值会发生**切割**（只留基类部分），引用/指针绑定不切割；基类 → 派生不允许隐式转换。
- **多基继承**：每个基类都要写继承方式（`class` 默认 `private`）；基类构造按继承列表顺序执行；同名成员用 `对象.基类名::成员()` 消除二义。
- **菱形继承**：非虚时对象里存两份 A 子对象，访问 A 成员二义、空间浪费；用 `virtual` 虚基继承共享一份 A，代价是增加虚基指针（vbptr）按偏移定位共享子对象，布局示例 A=8 → 非虚 D=16 → 虚继承 B/C=16、D=24，成员函数与静态成员不计入对象大小。
- **派生类对象间复制三情形**：只自定义基类的拷贝/赋值时，派生类缺省版本会自动调用它们，无需操心；一旦自定义了派生类的拷贝构造/赋值，就必须在内部显式调用基类版本（`Base(rhs)` / `Base::operator=(rhs)`），否则基类部分会悄悄丢失或不被更新。

## 作业
一、选择题

> ✅ **1．选 A 正确。** 旁边注释不对：不是"取并集"，是**取权限更严格（较小）的一方**。A 错在"仍然"二字——private 继承下基类 protected 成员会变成派生类 private。

下面叙述错误的是（ A ）

A．基类的protected成员在派生类中仍然是protected

B．基类的protected成员在public派生类中仍然是protected的

C．基类的protected成员在private派生类中是private的

D．基类的protected成员不能被派生类的对象访问


// 看以哪种权限继承

> ✅ **2．选 D 正确。**

2、下列对派生类的描述中，( D )是错误的。

A．一个派生类可以作为另一个派生类的基类

B．派生类至少有一个基类

C．派生类的成员除了它自己的成员外，还包含了它的基类成员

D．派生类中继承的基类成员的访问权限到派生类保持不变




> ✅ **3．选 A 正确。**

3、派生类的对象对它的哪一类基类成员是可以访问的？（A）

A．公有继承的基类的公有成员       B. 公有继承的基类的保护成员

C. 公有继承的基类的私有成员       D. 保护继承的基类的公有成员


// 注：非虚菱形继承时公共基类会存两份、有二义；加 virtual 虚继承后才"只有一个"、无二义

> ❌ **4．选错了，应为 D。** B 说的是菱形继承：B、C 同继承公共基类 A（非虚）时，D 里有两份 A，访问确实可能二义（需虚继承消除）→ 叙述**正确**。错的是 D：派生类自己定义同名函数会**隐藏**基类版本，访问不产生二义。

4、关于多继承二义性的描述，( D )是错误的。

A．派生类的多个基类中存在同名成员时，派生类对这个成员访问可能出现二义性

B．一个派生类是从具有共同的间接基类的两个基类派生来的，派生类对该公共基类的访问可能出现二义性

C．解决二义性最常用的方法是作用域运算符对成员进行限定

D．派生类和它的基类中出现同名函数时，将可能出现二义性

5、设有基类定义：

```C++
class Base
{   
private: 
	int a;
protected: 
	int b;
public: 
	int c;
};
```

> ✅ **5．选 A 正确**，私有继承会把基类 protected 成员降为派生类 private。

派生类采用何种继承方式可以使成员变量b成为自己的私有成员(  A   )

A. 私有继承             B.保护继承

C. 公有继承             D.私有、保护、公有均可

二、填空题
> ✅ **基本正确。** 顺序：派生类自身 → 对象成员（按声明逆序）→ 基类子对象。中间空格建议写"对象成员/类数据成员"更规范。

1、在继承机制下，当对象消亡时，编译系统先执行<u>\__派生类_____</u>的析构函数，然后才执行<u>\_特殊数据成员比如类成员\_</u>___的析构函数，最后执行_<u>_____基类\___</u>_的析构函数。

三、改错题以及写结果题。
> ✅ **1．两处都改对了。** 错误①：protected 继承使 `move/getx/gety` 在 main 中不可见 → 改 public 继承；错误②：函数名大小写 `getlength/getwidth` → `getLength/getWidth`。

1、指出并改正下面程序中的错误。

```C++
#include<iostream>

using std::cout;
using std::endl;

class Point
{   
public:
    Point(int a=0, int b=0) 
	{
		x = a; 
		y = b;
	}
    void move(int xoffset,int yoffset) 
	{
		x += xoffset; 
		y += yoffset;
	}
	

    int getx() 
    {	
    	return x;	
    }
    
    int gety() 
    {	
    	return y;	
    }

private:
	int x,y;
};

// 保护继承后，基类公有成员变为protecetd
// 只能在类的内部使用，不能被对象使用
// 改为公有继承
class Rectangle
:protected Point
{    
public:
	Rectangle(int x, int y, int l, int w)
	: Point(x,y)
	{   
		length = l;
		width  = w;
	}
	

	int getLength()
	{	
		return length;	
	}
	
	int getWidth()	
	{	
		return width;	
	}

private:
	int length;
	int width;
};
int main()
{ 
	Rectangle r(0, 0, 8,4);
 	r.move(23,56);
	cout << r.getx() 
	     << "," << r.gety() 
         // 函数写错了
		 << "," << r.getlength() 
		 << "," << r.getwidth() << endl;
		 

	return 0;

}
```

> ✅ **2．改对了。** 错误是二义性：`myc.x`、`myc.display()` 分不清 A/B，用 `A::` / `B::` 限定即可，而非改掉对成员的访问本身。

2、指出并改正下面程序中的错误。

```C++
#include<iostream>

using std::cout;
using std::endl;

class A
{
 public:
    int x;
    A(int a = 0) 
	{
		x = a;
	}
    void display() 
	{ 
		cout<< "A.x = " << x << endl;
	}
};
class B
{ 
public:
	int x;
    B(int a = 0) 
	{
		x=a;
	}
 	

	void display() 
	{
		cout<<"B.x = " << x << endl; 
	}

};

class C
:public A
,public B
{   

 public:
    C(int a, int b, int c) 
	: A(a)
	, B(b)
    {    
		y=c;  
	}
	

   	int gety() 
   	{ 
   		return y;
   	}

private:
	int y;
};
int main()
{ 
    C myc(1, 2, 3);
    // 二义性了
    // myc.x = 10;
    myc.A::x = 10;
    myc.B::x = 10;
    // 二义性了
    // myc.display();
    myc.A::display();
    myc.B::display();

	return 0;

}
```

> ✅ **3．结果正确。** 构造顺序：先基类 `Base(13)`（`Constucting base class`），再对象成员 `_base(24)`（又一次 `Constucting base class`），后函数体；析构按反向：派生类 → 对象成员 → 基类。输出 `13,8,24` 无误。

3、看程序写结果

```C++
/*
Constucting base class
Constucting base class
Constructing derived class
13,8,24
Destructing derived class
Destructing base class
Destructing base class
*/

#include<iostream>

using std::cout;
using std::endl;

class Base
{     
public:
	Base(int n)
	{
		cout <<"Constucting base class" << endl;
		_ix=n;
	}
	

    ~Base()
    {
    	cout <<"Destructing base class" << endl;
    }
    
    void showX()
    {
    	cout << _ix << ",";
    }
    
    int getX()
    {
    	return _ix;
    }

private:
	int _ix;
};


class Derived
:public Base
{     
public:
	Derived(int n, int m, int p)
	: Base(m)
	, _base(p)
	{
		cout << "Constructing derived class" <<endl;
        j = n;
    }

    ~Derived()
    {
    	cout <<"Destructing derived class"<<endl;
    }
    
    void show()
    {
    	Base::showX();
        cout << j << "," << _base.getX() << endl;
    }

private:
	int j;
    Base _base;
};
int main()
{ 
	Derived obj(8,13,24);
 	obj.show();

	return 0;

}
```


四、简答题

> ✏️ **1．意思对，订正错别字并补一句。** `proterceted→protected`、`provate→private`。三种继承中基类 **private 成员一律不可访问（但存在）**；protected 继承后 public/protected 变 protected（类内及派生类内可访问，对象不行）；private 继承后全部变 private（仅派生类内可访问）。

1、三种继承方式对于基类成员的访问权限是怎样的？

- public继承时，基类的public成员即可在派生类内访问，也可被派生类对象访问，proterceted成员只可在类内访问，派生类对象不能访问，provate成员不可访问

- protected继承时，基类所有public,protected成员都变为protected类型，只能在类内访问，private成员不可被访问

- private继承时，基类所有public,protected成员都变为派生类的private类型，智能在派生类内访问，

> ✏️ **2．new/delete 应从列表中删掉**（它们是全局运算符/表达式，不是类成员，谈不上"继承"）。标准答案：**构造函数（含拷贝构造）、析构函数、`operator=`（赋值运算符）、友元函数与友元类**。可补充：静态成员、`using` 引入的成员可以继承。

2、继承中有哪些内容是不能进行继承的？
- new/delete
- 友元
- 构造函数
- 析构函数
- 赋值运算符函数

> ✏️ **3．需补充菱形继承这一类。** 除你列出的两类外：当多个基类来自同一公共基类（非虚）时，派生类中会**重复存储**公共基类子对象，访问其成员同样二义、还浪费空间 → 用**虚基类（virtual 继承）**解决；不同基类的同名成员用 `::` 限定消除。

3、多基派生会产生的问题有哪些？怎样解决？

- 会有二义性的问题。
通过类名加限定符`::`的方式访问来避免
- 会有权限漏写的问题，继承时对每一个类都要写权限，默认是private继承

> ✅ **4．正确，三条规则都覆盖了。**

4、派生类对象之间的复制控制规则是什么？

- 若基类写了拷贝构造函数，派生类旧对象给新对象初始化时调用派生类默认的拷贝构造函数，然后派生类默认拷贝构造函数自动调用基类的拷贝构造函数进行基类部分的赋值
- 若基类写了赋值运算符函数，派生类两个旧对象之间赋值时，会自动调用缺省的派生类赋值运算符函数，函数内部会自动调用基类的赋值运算符函数，进行基类部分的赋值
- 若派生类重写了拷贝构造函数或者赋值运算符函数，那么必须在重写的函数中显式的调用基类的拷贝构造函数或者赋值运算符函数，以避免出现基类部分没有被赋值的异常情况




五、编程题。

> ✏️ **编程 1&2：主流程缺输出。** 题目要求"分别显示圆半径、圆面积、圆周长、圆柱体积"，你的 main 只调了 `showVolume()`，圆的半径/面积/周长没显示。建议先 `c1.show()`（Cylinder 继承自 Circle，可直接调用）再 `c1.showVolume()`，或单独建一个 `Circle` 对象调用 `show()`；若想要"设置半径"，可补 `void setRadius(double)`。

1. 编写一个圆类Circle，该类拥有： 

  	① 1个成员变量，存放圆的半径；
  	② 两个构造方法
  	  Circle( )              // 将半径设为0           
  	  Circle(double  r )     //创建Circle对象时将半径初始化为r      
  	③ 三个成员方法              
  	  double getArea( )      //获取圆的面积            
  	  double getPerimeter( ) //获取圆的周长              
  	  void  show( )          //将圆的半径、周长、面积输出到屏幕 

2. 编写一个圆柱体类Cylinder，它继承于上面的Circle类，还拥有： 
   ① 1个成员变量，圆柱体的高；
   ② 构造方法           
   Cylinder (double r, double  h) //创建Circle对象时将半径初始化为r         
   ③ 成员方法
   double getVolume( )   //获取圆柱体的体积             
   void  showVolume( )   //将圆柱体的体积输出到屏幕  
   编写应用程序，创建类的对象，分别设置圆的半径、圆柱体的高，计算并分别显示圆半径、圆面积、圆周长，圆柱体的体积。
```c++
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

const double PI = 3.1415926;

class Circle {
public:
  Circle() : _r(0) {}
  Circle(const double &r) : _r(r) {}
  ~Circle() { cout << "~Circle()" << endl; }

  double getArea() { return PI * _r * _r; }

  double getPerimeter() { return 2 * PI * _r; }

  void show() {
    cout << "the radius of the circle is " << _r << endl
         << "the Area of the Circle is " << getArea() << endl
         << "the Perimeter of the Circle is " << getPerimeter() << endl;
  }

private:
  double _r;
};

class Cylinder : public Circle {
private:
  double _dheight;

public:
  Cylinder(double r, double h) : Circle(r), _dheight(h) {
    std::cout << "this is Cylinder()" << std::endl;
  }

  double getVolume() { return getArea() * _dheight; }

  void showVolume() {
    std::cout << "the Volume of the Cylinder is " << getVolume() << std::endl;
  }
};

int main(int argc, char *argv[]) {
  Cylinder c1(3, 5);
  c1.showVolume();

  return 0;
}


```
> ✏️ **编程 3：功能基本可用，三处改进。** ① 题目明确要求 Employee 用 `display()` 输出姓名/年龄/部门/工资，你实现的是 `print()`——建议定义 `Employee::display()`（内部先调 `Person::display()` 再输出部门/工资）来"隐藏/覆盖"基类版本；② 拼写错误：`anverage`→`average`、`salasy`→`salary`；③ `const string name` 传值改成 `const string &name` 更优。平均值那段用 `map` 略显绕，直接循环 `getSalary()` 累加再除以 3 即可（`map` 还引入了不必要的 C++11 依赖）。

3. 构建一个类Person，包含字符串成员name（姓名），整型数据成员age（年龄），成员函数 display()用来输出name和age。
   构造函数包含两个参数，用来对name和age初始化。构建一个类Employee由Person派生，包含department（部门），实型
   数据成员salary（工资）,成员函数display（）用来输出职工姓名、年龄、部门、工资，其他成员根据需要自己设定。
   主函数中定义3个Employee类对象，内容自己设定，将其姓名、年龄、部门、工资输出，并计算他们的平均工资。
```c++
#include <iostream>
#include <map>
#include <string>
using std::cin;
using std::cout;
using std::endl;
using std::map;
using std::string;

class Person {
public:
  Person(const string &name, const int &age) : _name(name), _age(age) {}
  ~Person() { cout << "~Person()" << endl; }

  void display() {
    cout << "The name of the Person is " << _name << endl
         << "The age of the Person is " << _age << endl;
  }
  const string &getName() { return _name; }

private:
  string _name;
  int _age;
};

class Employee : public Person {
public:
  Employee(const string name, const int age, const string &department,
           const double &salary)
      : Person(name, age), _department(department), _salary(salary) {
    cout << "This is Employee(const string&, const double &). " << endl;
  }
  ~Employee() { std::cout << "~Employee()." << std::endl; }

  void print() {
    display();
    cout << "Department is " << _department << endl
         << "Salary is " << _salary << endl;
  }

  const double &getSalary() { return _salary; }

private:
  string _department;
  double _salary;
};

int main(int argc, char *argv[]) {
  Employee e1("张东", 20, "设计部", 4300);
  Employee e2("陈敏", 24, "人事部", 3300);
  Employee e3("于晨", 39, "工程部", 4900);
  e1.print();
  e2.print();
  e3.print();
  map<string, double> m;
  m[e1.getName()] = e1.getSalary();
  m[e2.getName()] = e2.getSalary();
  m[e3.getName()] = e3.getSalary();
  double sum = 0;
  for (auto &e : m) {
    sum += e.second;
  }
  double anverageSalary = sum / m.size();
  std::cout << "the anverage salasy is " << anverageSalary << std::endl;
  return 0;
}
```
