---
title: "cpp概览"
date: 2026-08-04
draft: false
categories: ["cpp"]
tags: ["StudyRecord", "技术记录"]
---

## 概览
![cpp概览](/Users/bohn/04_Project/06_MyWeb/BohnChen.github.io/static/images/1_LiLi_CPP_Overlook.png)

## day1知识点
### c++发展历史
诞生的直接原因：Bjarne博士在研究Unix内核时，发现没有合适的工具可以有效的分析由于内核分布而造成的网络流量以及如何将内核模块化
![cpp历史](/Users/bohn/04_Project/06_MyWeb/BohnChen.github.io/static/images/3_cpp_history.png)
### c++应用方向
![cpp应用方向](/Users/bohn/04_Project/06_MyWeb/BohnChen.github.io/static/images/2_cpp_for_what.png)
### 命名空间
```c++
// 也许你会不经意间定义一个名为cout的函数
void cout () { // ERROR:编译器报错，因为库函数中已经有一个cout函数
}

// 为了避免不经意间的名称冲突，让重复名称被正确管理，创造了命名空间

::  叫做“作用域限定符”

/* 
正确使用命名空间，我们有三种方法：
    1. 将命名空间写完整，处处使用std::cout,std::endl;
    2. 靠经验使得不冲突(不可靠)
    3. 使用一个写一个命名空间：using std::cout;

命名空间有一些自己的特性：
    1. 可以嵌套使用，namespace nice{namespace nice {}}
    2. 可以追加，比如你可以给std空间增加内容而不会报错
    3. 就近覆盖
*/
// 1. 嵌套
namespace test {
    namespace test {
        void TestNameSpace();
    }
}

// 2. 追加
namespace std {
    void MyStdFunc() {
        std::cout << "My std Funct" << std::endl;
    }
}

// 3. 就近覆盖
// 全局变量 print
int print = 10;

namespace OverWrite {
    int print = 20;
    void test(int print = 1) {
        std::cout << "形式参数 print = " << print << std::endl;
        std::cout << "命名空间内的 print = " << OverWrite::print << std::endl;
        std::cout << "全局变量 print = " << ::print << std::endl;
    }
}
```
### const关键字
常量修饰符，这里我们需要区分两个内容**指针常量**和**常量指针**

核心区分方式：从右往左读。
const 修饰的是它左边的内容（如果左边有东西的话），如果左边没有东西，则修饰右边：
1. *常量指针 (pointer to const)*：const int *p 或 int const *p
- 读法：p 是一个指针，指向 const int
- 含义：指向的值不能改，指针本身可以改
- *p = 10; ❌ | p = &b; ✅
2. *指针常量 (const pointer)*：int * const p
- 读法：p 是一个 const 指针，指向 int
- 含义：指针本身不能改，指向的值可以改
- *p = 10; ✅ | p = &b; ❌
3. 组合：const int * const p — 都不能改
和运算符优先级无关。 区别完全取决于 const 相对于 * 的位置，跟优先级规则没有直接关系。运算符优先级影响的是带解引用的表达式求值顺序（如 *p++ vs (*p)++），而不是类型声明本身的语义。类型声明遵循的是 C/C++ 的"声明模仿使用"（declaration mimics use）语法规则。

### new和delete
new/delete与malloc/free的区别是什么?



### 引用的使用

```c++
// 引用就是对指针解引用的语法糖(甜的，用起来舒服就是甜的)。
// 引用本身就是别名
int a = 0
int &r = a;
int *p = &a;

```







## Day1作业


 C++Day1

C++语法规则很多，要落实下来，得通过多敲代码来理解，看N遍不如写一次；在写代码的过程中，会碰到其它你不曾碰到过的编译问题，切记程序是调试出来的；再就是通过练习，把敲代码的速度提升上来，熟悉键盘，培养写代码的感觉



### 一、选择题
1、在C++中执行以下4条语句后输出rad值为：（）

```C++
static int hot=200; 
int &rad=hot;
hot = hot + 100; 
cout<< rad << endl; 

A、100        B、200     C、300    D、400
```



### 二、简答题

1. const关键字与宏定义的区别是什么？
2. malloc的底层实现是怎样的？free是怎么回收内存的？
3. new/delete与malloc/free的区别与联系是什么？(面试常考)
4. 区分以下概念：内存泄漏、内存溢出、内存踩踏、野指针？(面试常考)
5. 引用与指针的区别是什么？并且将"引用"作为函数参数有哪些特点？在什么时候需要使用"常引用"？



### 三、写出下面程序的运行结果。

1、第一题：

```C++
#include <iostream>

using std::cout;
using std::endl;

void f2(int &x, int &y) 
{
	int z = x; 
	x = y; 
	y = z;
}

void f3(int *x, int *y) 
{
	int z = *x; 
	*x = *y; 
	*y = z;
}

int main() 
{
	int x, y;
	x = 10; y = 26;
	cout << "x, y = " << x << ", " << y << endl;
	f2(x, y);
	cout << "x, y = " << x << ", " << y << endl;
	f3(&x, &y);
	cout << "x, y = " << x << ", " << y << endl;
	x++; 
	y--;
	f2(y, x);
	cout << "x, y = " << x << ", " << y << endl;
	return 0;
}
```



2、以下代码输出的是__？

```C++
int foo(int x,int y)
{
    if(x <= 0 ||y <= 0)  
        return 1;
    return 3 * foo(x-1, y/2);
}

cout << foo(3,5) << endl;
```



3、若执行下面的程序时，从键盘上输入5，则输出是（）

```C++
int main(int argc, char** argv)
{
    int x;
    cin >> x;
    if(x++ > 5)
	{
		cout << x << endl;
	}      
    else
	{
		cout << x-- << endl;
	}
    
    return 0;
}
```


4、写出下面程序的结果：

```C++
int main() 
{ 
    int a[5]={1,2,3,4,5}; 
    int *ptr=(int *)(&a+1); 
    printf("%d,%d",*(a+1),*(ptr-1)); 
}
```



### 四、有段代码写成了下边这样，如何在只修改一个字符的前提下，使代码输出20个hello?

```C++
for(int i = 0; i < 20; i--)
    cout << "hello" << endl;
```

