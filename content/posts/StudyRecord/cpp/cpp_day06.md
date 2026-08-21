---
title: "new/delete表达式、输入输出流、标准IO、缓冲区、隐式转换、文件操作、vector的扩容机制"
date: 2026-08-18T19:57:15+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## new/delete
### new/delete 表达式 VS operator new/delete 函数

```c++
Foo* p = new Foo;   // 这是 "new 表达式"，由编译器完成两步
// 步骤1: 调用 operator new 分配原始内存（这一步才可被重载/私有化）
// 步骤2: 在这块内存上调用构造函数 Foo::Foo()

delete p;           // "delete 表达式"，也是两步
// 步骤1: 调用析构函数 ~Foo()（释放对象持有的资源）
// 步骤2: 调用 operator delete 释放内存
```
- new/delete 表达式：语言关键字，整体不可重载。
- operator new/operator delete 函数：分配/释放函数，可以被重载、可以设为 private。


### operator new/delete 函数为什么是静态
由于分配资源的行为发生构造函数<span style = "color:red">之前</span>，且释放资源发生在析构函数<span style = "color:red">之后</span>，都处于没有对象的阶段，所以，new/delete函数需要是静态的，而`static`的类内成员是没有**`this`**指针的，`static`类型函数，也就无法访问非静态成员。

```c++
#include <cstdlib>
#include <iostream>

class Foo {
    int data_ = 0;
public:
    // 即使不写 static，它也是 static 成员函数
    void* operator new(size_t size) {
        // 这里不能用 data_（无 this 指针）
        std::cout << "Foo::operator new, size=" << size << "\n";
        return std::malloc(size);
    }
    void operator delete(void* p) {
        std::cout << "Foo::operator delete\n";
        std::free(p);
    }
};
```

### new/delete在类内类外的区别？
- 类内定义的 `new/delete`：只对该类生效

只有 `new Foo / delete Foo*` 会走类内的 `operator new/operator delete`；其他类型走全局的。
```c++
class Foo { /* 如上面的类内 operator new/delete */ };

Foo* p = new Foo;      // -> Foo::operator new   （命中类内）
delete p;              // -> Foo::operator delete（命中类内）

int* q = new int;      // -> 全局 operator new   （不命中 Foo 的）

```

- 类外（全局）的 `new/delete`：所有 `new/delete` 都走它

```c++
void* operator new(size_t size) {
    std::cout << "global new\n";
    return std::malloc(size);
}
void operator delete(void* p) noexcept {
    std::cout << "global delete\n";
    std::free(p);
}

int* a = new int;      // global new
Foo*  b = new Foo;     // 若 Foo 没定义自己的，也走 global new
```
> 规则是：优先找类内的，找不到再找全局的。


### 只能创建栈对象的方法
私有化 operator new/delete

原理：new 表达式的第一步就是调用 operator new，私有后外部无法访问 → 编译错误。栈对象不走 operator new，所以合法。
```c++
class StackOnly {
public:
    StackOnly() {}
private:
    void* operator new(size_t) = delete;      // C++11 直接删掉，或写成 private + 不实现
    void operator delete(void*) = delete;
    void* operator new[](size_t) = delete;    // 别忘了数组版本
    void operator delete[](void*) = delete;
};

StackOnly s;                 // 正确：栈对象不调用 operator new
StackOnly* p = new StackOnly;// 错误：operator new 是私有的

```

### 只能创建堆对象的方法
私有析构函数 → 栈对象报错，堆对象可以

```c++
class HeapOnly {
    ~HeapOnly() {}          // 私有析构
public:
    HeapOnly() {}
    void destroy() { delete this; }  // 类内可以访问私有析构，合法
};

HeapOnly* p = new HeapOnly; // 正确
p->destroy();               // 正确：类内 delete，能调用私有析构

HeapOnly s;                 // 错误：作用域结束时编译器要调 ~HeapOnly()，但它是私有的
```


> 原理：栈对象在作用域结束时要自动调用析构，而调用点在外面 → 私有析构无法被访问 → 编译错误。堆对象则由你手动 destroy()，在类内完成 delete。
> > 小陷阱：HeapOnly* q = new HeapOnly;  delete q; 这样直接 delete 也会报错，因为 delete q 在外部分配点调用私有析构。必须走 destroy()。


对象的销毁和析构函数的执行二者是不是等价的？

不等价。 析构只是销毁过程的一部分。
- 析构函数 = 释放对象持有的资源（成员对象、它自己 new 出来的内存、文件句柄等）。
- 销毁（对象生命期结束） = 析构 + 回收对象自身占用的内存。这两步是分开的：
- 栈对象：作用域结束 → 析构 → 栈指针回退自动回收内存（析构不管内存）。
- 堆对象：delete → 先析构 → 再 operator delete 释放内存。
关键证据：可以手动只析构、不释放，说明两者是独立的两件事：
```c++
Foo* p = new Foo;
p->~Foo();      // 手动调用析构：对象生命期结束，但内存没释放！
// 这时 p 指向的内存还占着，只是对象已"死"
delete p;       // 若再 delete，会二次析构 -> 未定义行为（双重释放/重复析构）
```
反过来，operator delete 只释放内存，不负责调用析构。所以：

| 动作                    | 调析构 | 释放内存              |
|-------------------------|--------|-----------------------|
| 栈对象离开作用域        | ✅     | ✅（栈指针）          |
| delete p                | ✅     | ✅（operator delete） |
| 手动 p->~Foo()          | ✅     | ❌                    |
| 直接 operator delete(p) | ❌     | ✅                    |

## c++ 输入输出流
linux 的设计哲学是“万事万物皆是文件”，我们可以对一个文本文件进行读和写，对我们来讲，读，就是文本文件输入给我们，写，就是我们输出给文本文件。
对操作系统而言，磁盘可以作为一个输入输出对象；对程序而言，终端可以作为输入输出对象。

### “流”的概念
那么什么是流？流是字节序列，如果这个字节流是从如键盘、磁盘驱动器等流向内存，那就是输入；如果字节流从内存流向如显示器、打印机、磁盘驱动器等设备，就叫做输出。

### c++ 的输入与输出包括以下3方面的内容:
1. 对系统指定的标准设备的输入和输出。即从键盘输入数据，输出到显示器屏幕。这种输入输出称为标准输入输出，简称`标准I/O`。

2. 以外存磁盘文件为对象进行输入和输出，即从磁盘文件输入数据，数据输出到磁盘文件。以外存为对象的输入输出称为文件的输入输出，简称`文件I/O`。

3. 对内存中指定的空间进行输入和输出。通常指定一个字符数组作为存储空间（实际上可以利用该空间存储任何信息）。这种输入输出称为字符串输入输出，简称`串I/O`。


### 几个常用的流的类


### 流的状态
![常用类](/images/6_common_class.png)
IO操作与生俱来的一个问题是可能会发生错误，一些错误是可以恢复的，另一些是不可以的。在C++标
准库中，用iostate来表示流的状态，不同的编译器iostate的实现可能不一样，不过都有四种状态：
- badbit表示发生系统级的错误，如不可恢复的读写错误。通常情况下一旦badbit被置位，流就无法
再使用了。
- failbit表示发生可恢复的错误，如期望读取一个数值，却读出一个字符等错误。这种问题通常是可
以修改的，流还可以继续使用。
- 当到达文件的结束位置时，eofbit和 failbit都会被置位。
- goodbit被置位表示流未发生错误。如果badbit、 failbit和eofbit任何一个被置位，则检查流状态的条件会失。

```c++
#include <iostream>
#include <limits>

void printStreamStatus() {
    cout  >> "cin.bad = " << cin.bad() << endl
          >> "cin.fail = " << cin.fail() << endl
          >> "cin.eof = " << cin.eof() << endl
          >> "cin.good = " << cin.good() << endl;

}

void test() {
    int number = 30;
    printStreamStatus();
    cin << number; // 这里可以输入字符串测试
    printStreamStatus();
    cin.clear(); // 重置流的状态
    cin.ignore(1024, '\n'); // 如果加上这句，放进缓冲区的内容会被清空
    printStreamStatus();

    cout << "number = " << number << endl;

    string s1;
    // 当上面输入字符串时，fail被置位，clear又恢复，所以被放到缓冲区的
    // 字符串直接被放进了s1
    cin >> s1;
    cout << "s1 = " << s1 << endl;
}

void test2() {
    int number = 10;
    // 逗号表达式
    // 捕获 EOF 的Ctrl + d 信号
    while(cin >> number, !cin.eof()) {

        if(cin.bad()) {
            std::cerr << "The stream is bad." << endl;
            return;
        }
        else if (cin.fail()) {
            cin.clear();
            cin.ignore(std::numeric_limets<std::streamsize>::max, '\n');
            cout << "请输出 int 类型的数据" << endl;
        }
        else {
            cout << "number = " << number << endl;
        }

    }

}

int main() {
    test2();
    return 0;
}
```
## 缓冲区


## 隐式转换

## 文件操作

## vector的扩容机制

