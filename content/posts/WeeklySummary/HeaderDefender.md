---
title: "头文件卫士在防什么？"
date: 2026-08-13
draft: false
categories: ["编程语言"]
tags: ["WeeklySummary", "c/c++", "技术学习"]
---

# 辨析重定义错误

## 头文件卫士在防什么?


前面我们讲到，全局变量（静态数据成员）的初始化要放在 `.cc` 实现文件中，避免重定义。那问题来了：**头文件卫士不是已经能防止重复包含了吗？为什么多个 `.cc` 文件 include 同一个 `.h` 还是会出问题？**

要回答清楚，得先搞清楚三个概念：TU、ODR，以及 `inline` / `extern` 是如何分工的。

### 什么是 TU（翻译单元）

**TU（Translation Unit，翻译单元）**：编译器真正处理的编译单位——**一个 `.cc` 源文件 + 它 `#include` 进来的所有头文件**，经过预处理展开后形成的完整内容。

```c++
// a.cc
#include <iostream>
#include "util.h"
int main() { /* ... */ }
// 上面这个 a.cc 预处理展开后形成的完整文本，就是一个 TU
```

| 特性 | 说明 |
|---|---|
| 一个 .cc = 一个 TU | 每个 .cc 独立编译成一个 `.o` 目标文件 |
| 预处理状态不跨 TU | 头文件卫士的宏（如 `__UTIL_H__`）在每个 TU 里重新定义 |
| 编译互不影响 | a.o 和 b.o 各自编译，互不知道对方的存在 |

这就点出了头文件卫士的"边界"：它只能在**同一个 TU 内**起作用。

### 头文件卫士只能防住什么

我们经常在头文件中写：

```c++
#ifndef __COMPUTER_H__
#define __COMPUTER_H__

// 头文件的内容

#endif
```

```c++
// a.cc
#include "util.h"
#include "util.h"   // 第二次，__UTIL_H__ 已定义 → 内容被跳过 ✅ 防住了
```

这里的 `#define __UTIL_H__` 只在 `a.cc` 这一个 TU 的预处理过程中生效。编译完 `a.cc`，这个宏就"消失"了。所以：

- **同一个 TU 内重复包含** → 卫士能防 ✅
- **多个 TU 各自包含一次** → 卫士防不住，但也**不需要防**（本来每个 .cc 就都应该各自包含一次，才能看到声明）

### 什么是 ODR 规则

**ODR（One Definition Rule，单一定义规则）**：任何实体（函数、全局变量、类等）在整个程序中**只能有一个定义，但可以有多个声明**。

违反 ODR 有两种表现：

| 阶段 | 例子 | 错误 |
|---|---|---|
| 编译期（同一 TU 内） | 同一文件里写 `int x; int x;` | `redefinition` |
| 链接期（跨 TU） | a.cc 和 b.cc 都定义了 `int global;` | `multiple definition` |

真正和头文件有关的是**链接期**：头文件里如果写了**定义**（而非声明），每个 include 它的 `.o` 里就各有一份定义，链接时冲突。

```c++
// util.h —— ❌ 错误：头文件里写定义
int globalVar = 10;
void helper() { /* ... */ }

// a.cc include util.h → a.o 里含一份 globalVar + helper
// b.cc include util.h → b.o 里含一份 globalVar + helper
// 链接 a.o 与 b.o → ❌ multiple definition
```

### inline 怎么管：多份定义，链接器合并去重

`inline` 告诉链接器：**这些定义是等价的，多个 TU 出现同名定义时合并成一份，不要报错。**

```c++
// util.h —— ✅ inline 做法：定义直接写头文件
inline int globalVar = 10;          // C++17 起支持 inline 变量
inline void helper() { /* ... */ }

// a.cc、b.cc 都 include util.h
// 各自 .o 里都有一份，但 inline 语义允许重复
// 链接器任选一份保留 → ✅ 不冲突
```

但有个**必须满足的前提**：每个 TU 里的那份定义必须**完全相同**（类型、代码、初始值都一致）。否则是未定义行为，编译能过但结果不可靠。

```c++
// a.cc
inline int v = 10;
// b.cc —— 与 a.cc 的 v 内容不同
inline int v = 20;   // ⚠️ 未定义行为，编译能过，结果不可预测
```

### extern 怎么管：声明与定义分离

`extern` 声明"这个变量存在，但定义在别处"，真正的定义只放在**一个** `.cc` 文件里。

```c++
// util.h —— ✅ extern 做法：只有声明
extern int globalVar;
void helper();

// util.cc —— 唯一的定义
int globalVar = 10;
void helper() { /* ... */ }

// a.cc、b.cc 都 include util.h
// 各自只拿到声明，不产生定义
// 链接时都指向 util.o 里唯一的一份 → ✅ 不冲突
```

## 三者对比总表

| 机制 | 解决什么问题 | 定义放哪 | 结果 |
|---|---|---|---|
| 头文件卫士 | 同一 TU 内重复包含 | — | 管预处理，不涉及链接 |
| `inline` | 头文件里写定义导致的跨 TU 重定义 | 定义直接写头文件 | 链接器合并去重 |
| `extern` | 跨 TU 共享一份定义 | 头文件只有声明，定义放某个 .cc | 全程序唯一一份定义 |

## 为什么类定义天然允许多个 TU 出现

前面建议里提到"类定义天然允许多个 TU 出现"，那该怎么理解？**同一个类名，可以在多个 TU 中有相同的定义，这正是标准明确允许的**。原因在于 ODR 对"类"有专门的特殊规则。

ODR 规则其实把实体分成了两类：

| 实体 | 多个 TU 可否各有一份 | 前提 |
|---|---|---|
| 类定义、模板、内联函数、枚举 | ✅ 允许 | 每份定义必须完全相同 |
| 普通函数、全局变量 | ❌ 不允许 | 全程序只能一份定义 |

```c++
// Point.h
class Point {
public:
    void print();
private:
    int _ix;
    int _iy;
};

// a.cc               b.cc
#include "Point.h"    #include "Point.h"
// a.o 里一份 Point    b.o 里一份 Point
// 链接时：两份内容完全相同 → 不冲突 ✅
```

**为什么类可以，函数/变量不行？** 根本区别在于，类定义通常**不生成代码或存储**，只是告诉编译器"这个类型长什么样"。而函数体和变量定义会在 `.o` 里生成实际的机器码或数据，多份就会冲突。

```c++
class Point {
    int _ix;              // 只是声明成员，不生成符号
    void print();         // 只是声明成员函数，不生成符号
    // static int count;  // 注意：这只是声明，定义还得在 .cc 里单独写
};
```

类定义里只要不含 `static` 数据成员的定义、不含非内联函数体，它就是个"蓝图"，不会生成任何符号，所以每个 `.o` 各持一份"蓝图"没有危害。

不过有两个容易踩坑的边界情况：

```c++
// ✅ 类内直接写函数体的成员函数，其实隐式 inline，合法
class Point {
    void print() { /* 类内定义 = 隐式 inline */ }
};

// ❌ C++17 之前，静态数据成员类内只能声明，定义必须放 .cc
// Point.h
class Point {
    static int count;    // 只是声明
};
// Point.cc
int Point::count = 0;    // 唯一的定义，放 .cc 里
```

> <mark>所谓"类定义天然允许多个 TU 出现"，就是头文件里写 class 定义、被 N 个 .cc include 是唯一正确的做法，链接器会接受这 N 份相同定义；前提是这 N 份来自同一个头文件、内容一致。这也正是为什么类定义不需要也不能加 inline。</mark>

## 总结

> <mark>头文件卫士管"预处理"，只防同一 TU 内的重复包含；ODR 管"链接"，防的是跨 TU 的重复定义；inline 靠"多份等价定义合并去重"，extern 靠"多份声明指向一份定义"。</mark>

## 正确做法

实际工程里不用纠结，按下面几条做就基本不会出错：

1. **所有头文件都加卫士**（或用 `#pragma once`），这属于无脑必做：

   ```c++
   #ifndef __UTIL_H__
   #define __UTIL_H__
   // ...
   #endif
   ```

2. **函数**：普通函数只在头文件里写**声明**，实现放 `.cc` 里；确需"头文件即实现"的小函数（如模板、极简 getter/setter）才用 `inline`。

3. **全局变量**：
   - 优先用 `const` / `constexpr`——C++ 里命名空间的 `const` 全局变量默认**内部链接**，每个 TU 各一份，天然不冲突，可直接写头文件；
   - 需要共享的可变全局变量：头文件里用 `extern` 声明，某个 `.cc` 里定义；
   - C++17 起确实需要"在头文件里定义"的，才用 `inline` 变量。

4. **类定义**：天然允许多个 TU 出现（这就是为什么头文件里写 class 不用加 inline），不需要也不能加 `inline`。

> <span style="color:red">一句话经验：声明进头文件，定义进 .cc，跨文件共享用 extern，必须内联才用 inline。</span>
