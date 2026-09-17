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

除此以外，还有另外一种形式的抽象类。对一个类来说，如果只定义了 protected 型的构造函数而没有提供 public 构造函数，无论是在外部还是作为其他类的普通对象成员都不能直接创建该类的对象，但可以由其派生出新的类。这种能派生新类、却不能直接创建自己对象的类是另一种形式的抽象类。

```c++
// 形式二：通过 protected 构造函数实现的抽象基类
class AbstractBase {
protected:
  AbstractBase(int a) : _ia(a) { cout << "AbstractBase(int a)" << endl; }
  ~AbstractBase() { cout << "~AbstractBase()" << endl; }

private:
  int _ia;
};

class AbstractDerive : public AbstractBase {
public:
  // 合法：派生类构造函数初始化列表中调用基类的 protected 构造函数
  AbstractDerive(int a, int b) : AbstractBase(a), _id(b) {}
  ~AbstractDerive() {}

  // 1. 语法错误：
  // 在类体内部，Type a(...) 会被编译器当作成员函数声明解析。
  // 为了避免歧义，C++ 明确禁止在类内使用圆括号 () 进行成员就地初始化。
  // AbstractBase a(1);                    // ERROR: expected ')' before numeric constant

  // 2. 访问权限与类型错误：
  // new 返回指针类型 AbstractBase*，且此处属于外部调用，不能直接调用 protected 构造函数。
  // AbstractBase b = new AbstractBase(1); // ERROR: 调用了 protected 构造函数，且类型不匹配

  void display() { cout << "AbstractDerive() :: display() " << endl; }

private:
  int _id;
};

int main(int argc, char *argv[]) {
  // 外部直接实例化失败：调用了 protected 构造函数
  // AbstractBase a(1); // ERROR: 'AbstractBase::AbstractBase(int)' is protected within this context

  // 正确：通过子类实例化并使用
  AbstractDerive b(1, 2);

  return 0;
}
```

> **思考：为什么在类内写 `AbstractBase a(1);` 和在 main 函数中写 `AbstractBase a(1);` 报错完全不同？**
>
> 1. **在 main 函数中（报语义错误 / 类型检查）**：函数体内 `Type var(args);` 是标准的对象直接初始化语法。语法解析通过，随后编译器做语义类型检查，发现 `AbstractBase` 是抽象类（或构造函数是 protected），故报错 `cannot declare variable 'a' to be of abstract type...` 或 `is protected within this context`。
> 2. **在类定义内（报语法错误 / Parsing Error）**：在类体中，`Type a(1)` 会被编译器当作成员函数声明来解析。函数形参列表括号内期待的是参数类型（如 `int`），而括号里却是字面常量 `1`。编译器在**词法/语法解析阶段**就报错终止（如 `expected ')' before numeric constant`），甚至根本还没进入语义类型检查阶段。C++11 规定类内成员就地初始化只允许使用 `=` 或 `{}`（列表初始化）。

## 虚析构函数
基类的析构函数被设置为虚函数之后，派生类的析构函数自动变为虚函数。

### 为什么派生类析构函数名不同却能形成虚函数重写？
编译器为了统一虚函数的动态分派机制，在底层内部将类的析构函数统一命名为类似 `destructor` 的内部标识符号。因此虽然表面上基类析构函数叫 `~Base()`，派生类析构函数叫 `~Derive()`，但编译器认为它们签名匹配，满足重写（override）规则，并在派生类虚表中覆盖基类的析构函数地址。

### 虚析构函数的核心价值：防止内存泄漏
当使用**基类指针指向派生类对象**并通过 `delete` 释放资源时：
- 如果基类析构函数**不是虚函数**，编译器采用静态联编，只会调用基类析构函数，导致派生类独有的动态资源未被释放，造成**内存泄漏**或**未定义行为**。
- 如果基类析构函数**是虚函数**，通过指针释放时会触发动态绑定，先调用派生类的析构函数，再自动调用基类的析构函数，确保资源完整释放。

#### 对比示例代码：

```c++
#include <iostream>

using std::cout;
using std::endl;

class Base {
public:
  Base() { cout << "Base() 资源分配" << endl; }

  // 关键：声明为虚析构函数
  virtual ~Base() { cout << "~Base() 资源清理" << endl; }
};

class Derive : public Base {
public:
  Derive(int capacity = 10) : _data(new int[capacity]) {
    cout << "Derive() 分配堆内存资源" << endl;
  }

  ~Derive() override {
    delete[] _data;
    cout << "~Derive() 成功释放堆内存资源" << endl;
  }

private:
  int *_data;
};

int main() {
  cout << "=== 基类指针指向派生类对象 ===" << endl;
  Base *ptr = new Derive(100);

  // 释放资源
  // 若 Base 析构函数无 virtual: 只打印 ~Base()，_data 泄漏！
  // 若 Base 析构函数有 virtual: 依次调用 ~Derive() -> ~Base()
  delete ptr;

  return 0;
}
```

> **最佳实践准则**：
> 凡是具有虚函数（即打算作为多态基类使用）的类，其析构函数**必须**声明为 `virtual`；如果一个类不作为基类使用，则无需声明为虚析构，避免不必要的虚指针开销。

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

要深入理解多态的底层机制，首先需要分清以下几个关键概念：

1. **虚函数指针 (`_vfptr` / `vptr`)**：
   - 当类中包含虚函数时，编译器会在该类对象的**内存空间首地址**处安插一个指针，即虚函数指针（占用 8 字节，64 位系统下）。
   - 每个对象实例各自持有一个 `_vfptr`。
2. **虚函数表 (`vftable` / `vtable`)**：
   - 虚函数表是编译器在**编译期**为每个含有虚函数的类生成的函数指针数组。
   - 虚函数表属于**类级别**（同一个类的所有对象实例共享同一张虚表），存放在进程的**只读数据段（`.rodata`）**。
   - 虚表中的每一项都是一个函数指针，指向该类对应的虚函数机器指令起始地址。
3. **区别：虚基指针 (`_vbptr`) vs 虚函数指针 (`_vfptr`)**：
   - `_vfptr`（Virtual Function Pointer）：服务于**虚函数动态绑定（多态）**，指向虚函数表。
   - `_vbptr`（Virtual Base Pointer）：服务于**虚基类继承（解决菱形继承的数据二义性和冗余）**，指向虚基类表（vbtable），记录虚基类子对象相对于当前子对象的偏移量。

### 动态绑定的调用流程

当执行 `ptr->display()` 时：
1. 取出指针 `ptr` 所指对象首地址中的 `_vfptr`（虚函数指针）；
2. 依据虚函数在表中的固定偏移量（比如第 0 项），寻址获取对应的虚函数指针 `_vfptr[0]`；
3. 跳转到该函数指针所指向的函数代码段地址执行，并将对象的 `this` 指针作为隐式第一个参数传入。

底层实质等价于：
```c++
// 伪代码表示动态分发
(*(ptr->_vfptr[offset]))(ptr);
```

### 多态原理内存结构全貌

![C++ 多态虚函数表与内存模型](/images/cppstudy/8_polymorphism_vtable.png)

通过上面的示意图可以清晰地看到：
- `Base` 类对象 `baseObj` 与 `Derive` 类对象 `deriveObj` 内部都拥有属于自己的 `_vfptr`。
- `Base` 对象的 `_vfptr` 指向 `vtable for Base`，派生类对象的 `_vfptr` 指向 `vtable for Derive`。
- 派生类重写了 `display()`，因此派生类虚表第 0 项被替换为 `&Derive::display`；未重写的虚函数依然指向基类的函数实现。
- 无论通过基类指针还是引用调用，最终都顺着对象自己的 `_vfptr` 找到实际所属类的虚表，从而实现“同一个接口，不同的表现行为”。

## 加上虚函数后的多继承

当一个派生类同时继承多个含有虚函数的基类时，其虚函数表和对象内存布局会比单继承更复杂。

### 1. 多继承虚表的核心规则
1. **多个虚函数指针**：派生类对象内部会包含**多个虚指针 (`_vfptr`)**，每个继承自带有虚函数的基类子对象各对应一个 `_vfptr`。
2. **虚表挂载**：派生类自己声明的、未重写基类的全新虚函数，默认挂载在**第一个基类**（主基类 Primary Base）的虚函数表中。
3. **this 指针调整（Thunk 技术）**：当通过第二个基类的指针调用被派生类重写的虚函数时，编译器会自动在虚表调用中间插入一段代码（thunk），用于调整 `this` 指针的偏移量，确保派生类成员函数访问正确的对象内存。

### 2. 代码示例与内存结构验证

```c++
#include <iostream>

using std::cout;
using std::endl;

class Base1 {
public:
  virtual void f() { cout << "Base1::f()" << endl; }
  virtual void g() { cout << "Base1::g()" << endl; }
  int _b1 = 1;
};

class Base2 {
public:
  virtual void h() { cout << "Base2::h()" << endl; }
  virtual void k() { cout << "Base2::k()" << endl; }
  int _b2 = 2;
};

class Derived : public Base1, public Base2 {
public:
  // 重写来自 Base1 的 f()
  void f() override { cout << "Derived::f()" << endl; }
  // 重写来自 Base2 的 h()
  void h() override { cout << "Derived::h()" << endl; }

  // Derived 自身特有的虚函数（挂载在 Base1 的虚表中）
  virtual void derived_only() { cout << "Derived::derived_only()" << endl; }

  int _d = 3;
};

int main() {
  Derived obj;
  cout << "sizeof(Derived) = " << sizeof(Derived) << endl;

  Base1 *p1 = &obj;
  Base2 *p2 = &obj;

  cout << "Derived 对象起始地址: " << &obj << endl;
  cout << "p1 指针指向的地址:    " << p1 << endl;
  cout << "p2 指针指向的地址:    " << p2 << endl; // 注意：p2 地址比 p1 偏移了 sizeof(Base1)

  p1->f(); // 输出 Derived::f()
  p2->h(); // 输出 Derived::h()（内部触发 this 指针修正）

  return 0;
}
```

### 3. 多继承下对象的内存布局示意

```text
Derived 对象内存模型:
+-------------------------------+  <-- &obj / p1 (Base1 子对象起始地址)
| _vfptr (指向 Base1 虚表)       |  8 字节
| int _b1                       |  4 字节 (+4 字节填充对齐)
+-------------------------------+  <-- p2 (Base2 子对象起始地址，发生偏移)
| _vfptr (指向 Base2 虚表)       |  8 字节
| int _b2                       |  4 字节 (+4 字节填充对齐)
+-------------------------------+  <-- Derived 专属数据区
| int _d                        |  4 字节 (+4 字节填充对齐)
+-------------------------------+
```

---

## 多态的常用与高级用法

除了基础的基类指针调用虚函数，多态在现代 C++ 架构设计与工程实践中有着极为广泛且深度的应用。

### 1. 接口类（Interface）与面向接口编程
C++ 中没有 Java 的 `interface` 关键字，但可以通过**纯抽象类（只包含纯虚函数与虚析构函数，不包含成员变量）**来定义接口协议：

```c++
class ISerializable {
public:
  virtual ~ISerializable() = default;
  virtual std::string serialize() const = 0;
  virtual void deserialize(const std::string &data) = 0;
};
```
在现代分层架构中，高层模块依赖抽象接口而非具体实现，依赖注入（Dependency Injection）和插件式架构正是依托接口多态实现的。

### 2. 动静多态结合：RAII 与智能指针管理多态对象
裸指针手动调用 `delete` 容易遗漏，配合标准库的智能指针可以安全高效地管理多态生命周期：

```c++
#include <memory>
#include <vector>

std::vector<std::unique_ptr<Base>> vec;
vec.push_back(std::make_unique<Derive>(1, 2));

for (const auto &item : vec) {
  item->display(); // 自动动态绑定，作用域退出时自动调用虚析构，无泄漏风险
}
```

### 3. 动态多态 vs 静态多态（CRTP 奇异递归模板模式）

C++ 中存在两种多态：
- **动态多态（运行时多态）**：基于虚函数表，灵活性高，但调用存在间接寻址开销，且编译器无法进行内联优化（Inlining）。
- **静态多态（编译期多态）**：利用模板元编程或 **CRTP (Curiously Recurring Template Pattern)** 在编译期完成分发，**零运行时代价**。

#### CRTP 示例：
```c++
#include <iostream>

template <typename Derived>
class Shape {
public:
  void draw() {
    // 在编译期静态转换为具体派生类，调用具体实现，无需虚指针与虚表
    static_cast<Derived*>(this)->draw_impl();
  }
};

class Circle : public Shape<Circle> {
public:
  void draw_impl() { std::cout << "Drawing Circle (Zero Virtual Overhead)!" << std::endl; }
};

template <typename T>
void render(Shape<T>& s) {
  s.draw(); // 编译期内联，高性能
}
```

### 4. 运行时类型识别与转换：`dynamic_cast`
在继承多态体系中，有时需要从基类指针安全地向下转换为派生类指针：

```c++
Base *basePtr = new Derive(1, 2);

// dynamic_cast 依赖虚表中的 RTTI (type_info) 信息
if (Derive *derPtr = dynamic_cast<Derive*>(basePtr)) {
  derPtr->custom(); // 转换成功，安全调用子类特有接口
} else {
  // 转换失败返回 nullptr
}
delete basePtr;
```

### 5. 高级用法：std::function 与 std::any 实现的“类型擦除”（Type Erasure）
现代 C++（C++11/14/17）非常推崇**类型擦除**设计模式（例如 `std::function`、`std::any`）。它结合了模板与继承，对外提供值语义，对内隐藏具体类型的动态多态细节，使非继承关系的类型也能表现出一致的多态行为。
