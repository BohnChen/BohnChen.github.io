---
title: "单例模式、内存对齐"
date: 2026-08-13
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

# 单例模式、内存对齐

## 单例模式


## 内存对齐
- 为什么要内存对齐？
    - 平台移植原因：不是所有的硬件平台都能访问任意地址上的任意数据的；某些硬件平台只能在某些地址处取某些特定类型的数据，否则抛出硬件异常
    - 数据结构，尤其是栈，应该尽可能地在自然边界上对齐。原因在于，为了访问未对齐的内存，处理器需要作两次内存访问；而对齐的内存访问仅需要一次访问。

- 内存对齐有三个规则：
    - 1. `struct`和`union`的数据成员中，从`offset`为`0`的地方开始存，对齐单位取`#pragma pack`指定数值与最大数据成员两者中，最小的那个值进行对齐。
    ```c++
    #include <iostream>

    using std::cout;
    using std::endl;

    struct TestStruct_1 {
      int a1;
      char b1;
      short c1;
    }; // 4 + 1 + 2 = 4 + 4 = 8

    void TestFunc() {
    
      cout << "The size of the TestStrcutn is " << sizeof(TestStruct_1) << endl;
    }

    int main(int argc, char *argv[]) {
      TestFunc();
      return 0;
    }
        
    ```
    
    - 2. 在数据成员各自对齐后，`struct`和`union`本身也要对齐，对齐单位取`#pragma pack`指定数值和最大成员长度两个值中的最小值。
    ```c++
    //#pragma pack(4)  时，size  = 8
    //#pragma pack(1)  时，size  = 7 单位取 pack 与内部最大值的那个较小值
    
    #include <iostream>
    #pragma pack(4)

    using std::cout;
    using std::endl;

    struct TestStruct_1 {
      int a1;
      char b1;
      short c1;
    };

    void TestFunc() {
    
      cout << "The size of the TestStrcutn is " << sizeof(TestStruct_1) << endl;
    }

    int main(int argc, char *argv[]) {
      TestFunc();
      return 0;
    }
        
    ```
    - 3. 如果一个`struct`中有某些结构体成员，则结构体成员要从其内部最大元素大小的整数倍地址开始存储。
    ```c++
    struct TestStruct_2 {
      int a1;
      // int a2;
      struct TestStruct_3 {
        double d1;
        char c1;
        short s1;
      } a, b; // 至少要加上一个实例, 否则是一个声明
    } MyStruct;

    void Test_1() {
      cout << "The size of the TestStrcutn is " << sizeof(TestStruct_1) << endl;
    }
    void Test_2() {
      // 如果是 struct T1{ int a1; strcut T2{double d1; char c1; short s1;}; }
      // 输出的大小将是 4，只有一个int 大小，因为内部的struct只是一个声明，不占空间
      // cout << "The size of the TestStrcutn is " << sizeof(TestStruct_2) << endl;
      // 只有当内部的struct 的;前有名字后，内部struct才开始占用空间
      cout << "The size of the TestStrcutn is " << sizeof(MyStruct) << endl;
    }
        
    ```


## 作业




