---
title: "多态的内存布局和疑难点分析"
date: 2026-09-21T21:56:07+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

在上一篇文章中，我们已经讲清楚了多态存在的意义是可以降低代码的耦合性，用同样的代码调用函数，执行不同的函数体、多态的使用方式需要有基类中有virtual函数，派生类继承基类，派生类重写基类虚函数，基类指针或者引用指向派生类和基类指针或引用调用虚函数这五个条件和多态的内存原理是每个对象维护一个虚函数指针，该指针指向一张虚函数表，表中存着所有虚函数的函数指针，通过找到对应的指针执行 rodata 中存着的不同的虚函数，所有的对象，共享同一张虚表；如果派生类没有重写基类的虚函数，那么派生类虚表中的该函数指针，指向基类虚函数。

今天我们就多态中出现的难以区分的概念和使用形式做一点区分。比如，虚拟继承后的，带虚函数的多基派生、虚基指针和虚函数指针的关系、


## 带虚函数的多基派生

### 1. 概念澄清与核心结论

#### 什么是 vptr？
`vptr`（Virtual Table Pointer，虚函数表指针）本质上就是一个普通的指针变量（在 64 位系统下占 8 字节，32 位下占 4 字节）：
- **谁拥有它**：只要类中声明了 `virtual` 虚函数，编译器就会在对象实例中插入一个隐藏的指针成员。
- **存的是什么**：存储的是该类对应的虚函数表（vtable）的起始地址，虚表存放在只读数据区（`.rodata`）。
- **生命周期**：在对象构造时由构造函数初始化指向对应虚表；析构时随析构过程变动。
- **运行时作用**：调用 `ptr->func()` 时，CPU 通过对象首部的 `vptr` 查虚表槽位获得函数入口地址并执行跳转（动态绑定）。

#### 什么是“非虚直接基类”？
- **直接基类**：在继承列表中直接写出的父类（如 `class C : public B` 中的 `B`；而 `B` 继承自 `A` 时，`A` 是 `C` 的间接基类）。
- **非虚**：采用普通的继承声明（即未添加 `virtual` 关键字修饰继承），区别于“虚拟继承（`virtual public Base`）”。

#### 核心结论
**派生类实例内部不再只有一个 vptr，而是有多个 vptr（有几个含有虚函数的非虚直接基类，通常就有几个 vptr）。**

---

### 2. 代码示例与对象内存布局

假设有如下类定义：
```cpp
class BaseA {
public:
    int a = 1;
    virtual void funcA() { cout << "BaseA::funcA\n"; }
    virtual void funcCommon() { cout << "BaseA::funcCommon\n"; }
    virtual ~BaseA() = default;
};

class BaseB {
public:
    int b = 2;
    virtual void funcB() { cout << "BaseB::funcB\n"; }
    virtual void funcCommon() { cout << "BaseB::funcCommon\n"; }
    virtual ~BaseB() = default;
};

class Derived : public BaseA, public BaseB {
public:
    int c = 3;
    void funcCommon() override { cout << "Derived::funcCommon\n"; }
    virtual void funcC() { cout << "Derived::funcC\n"; } // Derived 自己的新虚函数
};
```

这里 `Derived` 同时继承了 `BaseA` 和 `BaseB`，两基类各自都有虚函数。

以 64 位系统为例，`Derived` 对象在内存中按继承顺序线性排列：

```text
高地址
+-----------------------------+
| Derived::c           (4B)   |
| (对齐填充 padding)     (4B)   |
+-----------------------------+ <--- BaseB 子对象结束
| BaseB::b             (4B)   |
| (对齐填充 padding)     (4B)   |
| BaseB 的 vptr (8B)          | ---> 指向 Derived 为 BaseB 定制的次虚表
+-----------------------------+ <--- BaseB 子对象起始 (基类指针 BaseB* 指向这里)
| BaseA::a             (4B)   |
| (对齐填充 padding)     (4B)   |
| BaseA 的 vptr (8B)          | ---> 指向 Derived 的主虚表 (Primary Vtable)
+-----------------------------+ <--- Derived 对象起始 (Derived*、BaseA* 指向这里)
低地址
```

**关键点**：
1. **主基类（Primary Base）**：第一个有虚表的基类（`BaseA`）通常被选为主基类，其 `vptr` 位于整个对象的起始位置。
2. **派生类自己新增的虚函数（如 `funcC`）放在哪里？**
   派生类独有的虚函数 `funcC()`，会被追加到主基类的虚表（主虚表）末尾，而不会放入 `BaseB` 的虚表中。

---

### 3. 虚表结构：主虚表与次虚表

编译器为 `Derived` 类在 `.rodata` 段构建两张逻辑虚表：

1. **针对 BaseA 的虚表（主虚表 Primary VTable）**：
   - `&Derived::~Derived()`
   - `&BaseA::funcA()`
   - `&Derived::funcCommon()` （已覆盖 BaseA 版本）
   - `&Derived::funcC()` （Derived 自有的新增虚函数挂在此处）

2. **针对 BaseB 的虚表（次虚表 Secondary VTable）**：
   - `&Derived::~Derived()` （带 this 调整）
   - `&BaseB::funcB()`
   - `Thunk: &Derived::funcCommon()` （**注意：此处为带 this 指针调整的 Thunk 跳转入口**）

---

### 4. 最核心机制：指针偏移与 Thunk（this 指针调整）

#### (1) 指针转换时的地址偏移（Pointer Adjustment）
```cpp
Derived* d = new Derived();
BaseA* pa = d;  // pa 的地址 == d 的地址
BaseB* pb = d;  // pb 的地址 != d 的地址！偏移了 sizeof(BaseA 子对象) 字节
```
- `pa` 指向 `Derived` 对象的起始位置（偏移为 0）。
- `pb` 必须指向 `Derived` 中 `BaseB` 子对象的起始位置。
- 赋值 `BaseB* pb = d;` 时，编译器隐式插入加法运算：`pb = (d != nullptr) ? (BaseB*)((char*)d + offset) : nullptr;`。

#### (2) 虚函数调用时的 this 调整（Thunk 机制）
当执行调用：
```cpp
pb->funcCommon();
```
- `funcCommon()` 在 `Derived` 中被重写，底层实际签名类似于：`void Derived::funcCommon(Derived* const this);`，需要接受整个 `Derived` 对象的首地址作为 `this` 指针。
- 但此时调用的指针 `pb` 指向的是 `BaseB` 子对象（偏移了 +16 字节），若直接当作 `this` 传入，函数内部访问成员变量时就会发生严重错位。
- **解决方案：Thunk（跳转小胶水代码）**
  `pb` 虚表中存放的不是直接的 `Derived::funcCommon` 地址，而是一段被称为 **Thunk** 的汇编小代码片段：
  ```asm
  # Thunk 汇编逻辑伪代码：
  this = this - 16;           # 将 BaseB* 修正回 Derived* 的真实首地址
  jmp Derived::funcCommon;    # 无缝跳转至目标函数体
  ```
  通过 Thunk，程序在进入重写的虚函数前，动态且静默地将 `this` 指针拉回了 `Derived` 对象的正确起始位置。

---

### 5. 代码验证

```cpp
#include <iostream>
using namespace std;

class BaseA {
public:
    int a;
    virtual void funcA() {}
    virtual ~BaseA() = default;
};

class BaseB {
public:
    int b;
    virtual void funcB() {}
    virtual ~BaseB() = default;
};

class Derived : public BaseA, public BaseB {
public:
    int c;
    void funcB() override { cout << "Derived::funcB, this = " << this << endl; }
};

int main() {
    Derived d;
    BaseA* pa = &d;
    BaseB* pb = &d;

    cout << "d 的地址  : " << &d << endl;
    cout << "pa 的地址 : " << pa << " (与 d 相同)" << endl;
    cout << "pb 的地址 : " << pb << " (发生偏移，跳过 BaseA)" << endl;

    cout << "\n调用 pb->funcB():" << endl;
    pb->funcB(); // 打印出的 this 会被 Thunk 机制自动调整回 d 的首地址

    return 0;
}
```


## 虚基指针和

