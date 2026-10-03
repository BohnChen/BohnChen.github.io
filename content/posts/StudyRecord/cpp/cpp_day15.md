---
title: "移动语义、移动构造、移动赋值、左值右值、move函数、右值引用"
date: 2026-09-17T19:04:16+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## 移动语义
为什么要有移动语义？ 移动语义实际上是为了解决**不必要的深拷贝性能损耗**和**独占资源的所有权转移困难**这两个核心痛点创造的。
在 c++98/03 中，对象的复制只有一种机制——拷贝构造和拷贝赋值，对于拥有堆内存或其他外部资源的对象，如（std::vector,std::string），在发生拷贝时，它的底层机制实现过程就是：***分配新内存->逐元素复制->销毁原对象并释放原内存***。
举例来讲：
```c++
// 在没有移动语义时
std::vector<int> CreateLargeVec() {
    std::vector<int> tempVec(10000, 1);
    return tempVec;
}

std::vector<int> v;

// 返回值： 1.  createLargeVector() 执行完毕，返回一个无名的临时 vector 对象（我们称它为 __temp_ret）。这个临时对象在堆上持有着 100 万个 int（约 4MB 内存）
// 拷贝赋值： 2. 调用 v.operator=(const std::vector<int>&)，将 __temp_ret 传给它。如果 v 原来有数据，那么就先释放自己旧的堆内存，然后申请 4MB 的空间，然后将临时对象的值复制过去
// 析构临时对象：3. 将临时对象析构掉
v = CreateLargeVec(); // 这里会发生赋值拷贝，很可能涉及深拷贝与析构
// 这个存了 4MB 数据的临时对象，竟然在一行代码之间，已经被析构了，这正是移动语义想要优化的地方。

```
在有了移动语义后，我们，我们就可以将临时对象的空间直接接管过来，而无需一个一个的复制。

```c++
// 有了移动语义后
// createLargeVector() 是按值返回的函数调用表达式。在 C++ 规则中，按值返回的临时对象天然就是右值（严格来说是纯右值 prvalue）。
// 编译器一看到右侧是一个右值临时对象，就会自动选择移动赋值函数（Move Assignment Operator）
v = CreateLargeVec();

```
在移动赋值函数内部：
以 std::vector 的真实核心实现原理为例（GCC 的 libstdc++ 和 LLVM 的 libc++ 实现逻辑一致）。

std::vector 内部本质上就是3 个裸指针：
```c++
T* start;         // 指向堆内存首地址
T* finish;        // 指向当前有效元素的末尾
T* end_of_storage;// 指向分配容量的末尾

```
当执行 v = createLargeVector(); 时，调用的正是：
`vector& operator=(vector&& other) noexcept;`

它内部具体执行以下三步：

1. 释放 v 自己的旧资源（如果 v 之前有数据）
如果 v 之前已经分配过内存，必须先释放掉它持有的旧堆内存，否则会导致内存泄漏：
```c++
if (this->start) {
    this->deallocate(this->start, this->end_of_storage - this->start);
}

```
2. “偷”取临时对象的指针（极其廉价的浅拷贝）
直接把临时对象 other 内部的 3 个指针地址复制给 v：
```c++
this->start = other.start;
this->finish = other.finish;
this->end_of_storage = other.end_of_storage;


```
这一步只拷贝了 3 个指针（24 个字节），无论 vector 里有 1 个元素还是 100 万个元素，耗时完全一样（几纳秒）。

3. 将临时对象置空（保护现场）
```c++
other.start = nullptr;
other.finish = nullptr;
other.end_of_storage = nullptr;

```
为什么要置空？
因为前面提到的步骤三（临时对象析构）依然会发生！表达式执行完分号 ; 时，临时对象 other 的析构函数依然会被调用：
```c++
~vector() {
    if (start) {
        deallocate(start, end_of_storage - start); // 释放内存
    }
}

```
因为在移动赋值函数里已经将 other.start 置为了 nullptr，所以当临时对象析构时：
- if (start) 判定为假，或者 free(nullptr)（C++ 中 delete nullptr 是安全的空操作）；
- 原先装有 100 万个数据的堆内存完好无损地留在了 v 中，没有触发真正的 free！

## 关键概念
+ 左值： 可以取地址的值
```c++
int ia;
&ia; // 不报错，所以， ia 可以取地址，ia 是左值
```
+ 右值：不可以取地址的值
右值包括：临时对象、匿名对象、字面值常量
```c++
&"hello"; // 报错， "hello" 不可以取地址，字面值常量是右值

```
+ const 左值引用：又被称作万能引用，它既可以接受左值，也可以接受右值。但是从它自己是看不出来到底接收了左值还是接收了右值的。
```c++
string str = "nice";
const &str_1 = "hello"; // 对的
const &str_2 = str; // 对的

// 从 str_1 和 str_2 本身是看不来接收的是左值还是右值的
```
+ 右值引用：如果可以叫的出来名字，该右值引用就是一个左值。右值引用只能绑定到右值。
这个与右值引用本身有没有名字有关，如果是 `int &&rref = 10` ,右值引用本身就是左值，因为有名字。如果右值引用本身没有名字，那右值引用就是右值，如右值引用作为函数返回值。
```c++
int &&func() {
    return 10;
}

```


## 移动构造

```c++
String(String &&rhs)
:_pstr(rhs._pstr)
{
    cout << "String(String &&rhs)" << endl;
    rhs._pstr = nullptr;
    _pstr = nullptr;
}

```

## 移动赋值
```c++
String &operator=(String &&rhs) 
{
    cout << "String &operator=(String &&)" << endl;
    if (this != &rhs) // 1. 防止自移动
    {
        delete [] _pstr;   // 2. 释放左操作数
        _pstr = nullptr;
        _pstr = rhs._pstr; // 3. 浅拷贝
        rhs._pstr = nullptr;
    }
}


```

## move 函数
move 函数实际上只是做了一个强制转换，将一个左值强制转换为一个右值而已。被转移之后的对象需要重新赋值后使用
```c++
void test() {
  std::cout << "hello world!" << std::endl;
  string str = "nice";
  string str_2 = std::move(str);
  // str 已经被移动了， 所以下面这行什么也不会输出
  cout << str << endl;
  // str_2 实际上已经拿到了原本给 str 的 “nice”
  cout << str_2 << endl;
  // 当然 str 并没有消亡，它只是资源被转移了，若要重新使用，需要重新赋值
  str = "hi";
  cout << str << endl;
}

```

