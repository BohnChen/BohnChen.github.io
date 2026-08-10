---
title: "面向对象、代码规范、"
date: 2026-08-07
draft: true
categories: ["编程语言"]
tags: ["StudyRecord", "c/c++", "技术学习"]
---

# 面向对象、代码规范

## 面向对象

## 代码规范
c++代码规范可以参考google C++ 开发标准
### 头文件引用规范：
```c++
// 最先引入自己写的头文件
#include "Myhead.h"

// 其次引入c 的头文件
#include <cstring>

// 最后是 cpp 的头文件
#include <iostream>

// 成员函数用小驼峰
class MyClass {
protected:
    void getParam(); 

// 数据成员以_开头，容易发现
private:
    int _myin;
};
```

## 类外成员函数的定义

## 对象的创建——构造函数
构造函数完成了初始化工作
不写构造函数时，编译器会生成默认的无参构造函数
写了构造函数后，新建对象时候，必须调用构造函数初始化对象
数据成员被初始化的顺序，只与声明顺序有关，与初始化顺序无关。
自动进行，不能加.调用

c的struct和cpp的struct有什么区别
c 的struct 和 cpp 的class 有什么区别和联系


## 对象的销毁—析构函数
默认情况下，编译器自动生成析构函数
对象在销毁时候，自动调用，在函数中时，函数栈销毁前调用析构函数回收资源

自动进行，但是能加.调用

## 对象在不同的位置
对象在栈上，静态空间内，可以被自动管理；但是当对象中有指针时，需要将其放在堆上上，必须手动申请和释放。

### 浅拷贝和深拷贝
浅拷贝只拷贝值，内存空间指向同一片位置，但是深拷贝构造函数不止拷贝值，还会申请一个新的内存空间。

调用拷贝构造函数的时机：
1. 当用已经存在的对象初始化一个新对象时候调用
2. 当函数参数类型为对象类型，实参和形参结合时候，会调用拷贝构造函数
3. 当返回类型是对象类型时，会调用拷贝构造函数

所以，为什么拷贝构造函数的参数类型是(const MyType &typeUseCase)?
这个&引用符号不能被省略，是因为如果省略
那么:

```c++
#include <string>
#include <iostream>

class Point() {
    Point(string word, int intPara)
    :_word(word)
    ,_int(intPara){
        
    }
    Point(const Point &rhs) {

    }
private:
    string _word;
    int _int;
};

Point p1("nice", 10);

// Point(const Point rhs) 
Point p2 = p1; // 当调用拷贝构造函数时，rhs = p1，p1已存在，但是rhs是一个新对象，所以重新调用拷贝构造函数，陷入循环，并且没有出口，程序知道栈溢出崩溃。

```
