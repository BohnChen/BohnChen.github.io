---
title: "继承、rss文件处理"
date: 2026-09-02T15:26:02+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## 继承
### 为什么要有继承

继承是面向对象编程的三大特性之一（封装、继承、多态），它解决的是类与类之间的**代码复用**与**层次关系建模**两大问题。

**复用的需求（问题场景）**：现在要设计 Student、Teacher 两个类，它们都有姓名、年龄等属性和对应的打印方法。没有继承时，公共成员在每个类里都要"抄"一遍：

```c++
class Student {
public:
  void print() const {
    cout << "姓名:" << _name << " 年龄:" << _age << " 学号:" << _id << endl;
  }
private:
  string _name;
  int _age;
  string _id;
};

class Teacher {
public:
  void print() const {
    cout << "姓名:" << _name << " 年龄:" << _age << " 工号:" << _id << endl;
  }
private:
  string _name;
  int _age;
  string _id;
};
```

姓名、年龄这些公共成员被大量复制粘贴。假如以后要给所有人都加一个"性别"字段，两个类都得改，一旦漏改就会不一致——这就是**重复代码难以维护**的痛点。

**用继承解决**：把公共部分抽取成一个基类（Base class）`Person`，`Student`、`Teacher` 作为派生类（Derived class）继承它，公共成员只写一次：

```c++
class Person {                  // 基类：抽取出的公共部分
public:
  void print() const {
    cout << "姓名:" << _name << " 年龄:" << _age << endl;
  }
private:
  string _name;
  int _age;
};

class Student : public Person { // 派生类：自动拥有 _name/_age/print()
private:
  string _id;
};

class Teacher : public Person { // 派生类：自动拥有 _name/_age/print()
private:
  string _id;
};
```

此时 `Student`、`Teacher` 内部即使一行公共成员都不写，也自动"继承"了基类的数据成员和成员函数。本质上，**派生类对象中天然包含一份完整的基类数据**（所谓"子对象"），它可以当作基类对象来使用。以后再要加公共字段，只改 `Person` 一处即可。

**继承的本质是 "is-a"（是一个）关系**：学生"是一个"人，老师"也是一个"人"，所以它们都继承自 `Person`。真实世界中"一般 → 特殊"的泛化关系，在代码里就靠继承来表达。

**继承还有一个重要职责：为多态奠定基础**。C++ 的运行时多态依赖虚函数，而虚函数必须依托继承体系——通过基类指针/引用调用派生类重写后的函数才能实现"同一种调用、不同的行为"。没有继承，就没有多态（后面章节会展开）。

> 顺带留意写法 `class Student : public Person` 中，`Person` 前的 `public` 是**继承方式**，它正是接下来"权限管理"的主角。

### 继承的权限管理

一个成员最终在派生类里的访问权限，由**两层因素**共同决定：
1. **成员自身的访问限定符**（`public` / `protected` / `private`），在基类声明时确定；
2. **继承方式**（`public` 继承 / `protected` 继承 / `private` 继承），决定基类成员进入派生类后权限被"保持"还是被"收缩"。

#### ① 三种访问限定符的可见范围（先回顾）

| 限定符 | 本类内部 | 派生类 | 类外（通过对象） |
|--------|:---:|:---:|:---:|
| public | ✅ | ✅ | ✅ |
| protected | ✅ | ✅ | ❌ |
| private | ✅ | ❌ | ❌ |

> `protected` 是"为继承而生"的权限：相比 `private` 它多放行了一类人——允许派生类直接访问，但对外部对象依然封闭。

#### ② 三种继承方式的转换规则

核心规则一句话：**取"成员在基类中的权限"与"继承方式"二者中较严格的一个**（严格程度：`private` > `protected` > `public`）。

以基类三种权限的成员为例，经过不同继承方式后，它们在派生类中的权限为（`不可见`表示不能直接访问，只能通过基类的公有/保护成员函数间接操作）：

| 基类成员 \ 继承方式 | public 继承 | protected 继承 | private 继承 |
|:---:|:---:|:---:|:---:|
| public 成员 | public | protected | private |
| protected 成员 | protected | protected | private |
| private 成员 | 不可见 | 不可见 | 不可见 |

要点：
1. **public 继承最宽松**：基类的 `public` 仍是 `public`、`protected` 仍是 `protected`，原样保留——日常最常用；
2. **protected 继承**：把基类的 `public` 成员收缩为 `protected`；
3. **private 继承最严格**：把基类所有可见成员（`public`、`protected`）一律收缩为 `private`；
4. **基类的 `private` 成员，无论哪种继承方式都"带不过来"**：它只在基类内部可见，派生类不能直接访问，只能通过基类的公有/保护成员函数间接操作。

```c++
#include <iostream>
using std::cout;
using std::endl;

class Base {
public:
  void show() { cout << "Base::show()" << endl; }
protected:
  int _prot = 1;
private:
  int _priv = 2;
};

// ---------- public 继承 ----------
class PublicDerived : public Base {
public:
  void func() {
    show();      // ✅ Base::show 原 public，继承后仍 public，内部可用
    _prot = 10;  // ✅ Base::_prot 原 protected，仍 protected，派生类可用
    // _priv = 10; // ❌ Base::_priv 是 private，派生类不可直接访问
  }
};

// ---------- private 继承 ----------
class PrivateDerived : private Base {
public:
  void func() {
    show();      // ✅ 仍可用，但在本类中 show 已降级为 private
    _prot = 10;  // ✅ 仍可用，同样降级为 private
  }
};

int main() {
  PublicDerived pd;
  pd.show();      // ✅ public 继承：show 仍是 public，类外可调

  PrivateDerived prv;
  // prv.show();  // ❌ private 继承：show 在本类中降级为 private，类外不可调
  return 0;
}
```

#### ③ 两重（多级）继承下，权限如何变化

两重继承指"基类之上还有基类"的链式结构，如 `class B : public A`、`class C : public B`，形成继承链 A → B → C。关键在于：**转换规则在每一级继承上都会被重新应用一次**。

**情形一：整条链都是 public 继承**

```c++
class A {
public:
  void fa() {}
protected:
  int _proA = 1;
private:
  int _priA = 2;
};

class B : public A {
  // fa()    : public    （保持）
  // _proA   : protected （保持）
  // _priA   : 不可见 ❌
};

class C : public B {
public:
  void fc() {
    fa();        // ✅ A::fa 一路 public，到 C 仍是 public，可直接调用
    _proA = 10;  // ✅ A::_proA 一路 protected，到 C 仍是 protected，可直接访问
    // _priA = 20; // ❌ A::_priA 是 A 的私有成员，对 B、C 都不可见
  }
};
```

逐层看：
- A 的 `public` 成员 `fa`：A→B、B→C 两次 public 继承都保持 `public`，所以对 C 以及对 C 的外部对象都"全公开"；
- A 的 `protected` 成员 `_proA`：每一级 public 继承后都保持 `protected`，因此整条链上的所有派生类（B、C 乃至 C 的后代）都能直接访问；
- **A 的 `private` 成员 `_priA` 是"断代"的**：它只在 A 内部可见。B : public A 时 B 内部已不能访问它，继承自 B 的 C 自然更不可能。`private` 成员的可见性不会跨继承链传递。

**情形二：中间某一级改用了 private 继承（断代）**

```c++
class A {
public:
  int _a = 1;
protected:
  int _proA = 2;
};

class B : private A {
  // _a    : public × private继承 → 收缩为 private
  // _proA : protected × private继承 → 收缩为 private
  void fb() { _a = 10; _proA = 20; }  // ✅ B 自己仍可直接使用
};

class C : public B {
  void fc() {
    // _a = 10;    // ❌ _a 在 B 中已是 private，对 C 不可见
    // _proA = 20; // ❌ 同上，断代了
  }
};
```

**B 用 private 继承 A 后，A 的一切成员在 B 中都被降级为 private，于是从 C 这一级开始全部不可见**。后面的类无论怎样继承 B，都拿不到 A 的任何成员——这就是权限的"断代"。

**情形三：中间某级改用 protected 继承（对外可见性被打断）**

```c++
class A {
public:
  void fa() {}
};

class B : protected A {
  // A::fa 在 B 中被收缩为 protected
};

class C : public B {
  void fc() { fa(); }  // ✅ C 仍可直接调用（B 中是 protected，对派生类可见）
};

int main() {
  B b;
  // b.fa(); // ❌ protected 继承后，fa 对外部对象不再可见
  return 0;
}
```

**两重继承的判断方法总结**：
1. 一级一级往下推导，不能跳级；每一级都重新套用"取成员权限与继承方式中较严格者"的规则；
2. 某成员一旦在某级变成 `private`，或它本身就是基类的 `private`，就从此"断代"，后面的后代全部不可见；
3. 判断能否访问要区分两种视角：**派生类内部**能访问的是 `public` + `protected`，**类外对象**只能访问 `public`。

速记：每一层都是"当前权限 × 该层继承方式 → 新的权限"；`private` 永不外传，`private` 继承必断代。




## rss 文件处理
这里用到了`tinyxml`库来处理这个 `rss` 文件，又用到`std::regex`库来进行无用信息的剔除



## 作业
