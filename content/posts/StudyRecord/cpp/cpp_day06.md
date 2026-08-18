---
title: "new/delete运算符"
date: 2026-08-18T19:57:15+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## new/delete 运算符
new 操作符函数和delete操作符函数是默认静态的，也就是说，它们没有 `this` 指针

new 操作符函数在类内时候，只有new一个类对象时候，会被调用，同理，只有销毁类对象时候，才会调用delete操作符函数；
当new/delete操作符函数在类外时，所有调用new的地方，都会调用类外的new/delete操作符函数。

只能创建堆对象：创建一个堆对象可以，创建一个栈对象错误的方式是“将构造函数和析构函数其中一个设置为私有”；当构造函数私有时，堆对象和栈对象都无法正确，析构私有后，栈对象不能被销毁。

只能创建栈对象：创建一个栈对象可以，创建一个堆对象错误的方式是“将new/delete操作符函数私有”

对象的销毁和析构函数的执行二者是不是等价的？


## c++ 输入输出流

linux 的设计哲学是“万事万物皆是文件”，我们可以对一个文本文件进行读和写，对我们来讲，读，就是文本文件输入给我们，写，就是我们输出给文本文件。
对操作系统而言，磁盘可以作为一个输入输出对象；对程序而言，终端可以作为输入输出对象。
那么什么是流？流是字节序列，如果这个字节流是从如键盘、磁盘驱动器等刘翔内存，那就是输入；如果字节流从内存流向如显示器、打印机、磁盘驱动器等设备，就叫做输出。
c++ 的输入与输出包括以下 3 方面的内容：
1. 对系统指定的标准设备的输入和输出。即从键盘输入数据，输出到显示器屏幕。这种输入输出称为标准输入输出，简称**`标准I/O`**。

2. 以外存磁盘文件为对象进行输入和输出，即从磁盘文件输入数据，数据输出到磁盘文件。以外存为对象的输入输出称为文件的输入输出，简称**`文件I/O`。**

3. 对内存中指定的空间进行输入和输出。通常指定一个字符数组作为存储空间（实际上可以利用该空间存储任何信息）。这种输入输出称为字符串输入输出，简称串I/O。
 cc


### 几个常用的流的类

### 流的状态

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
