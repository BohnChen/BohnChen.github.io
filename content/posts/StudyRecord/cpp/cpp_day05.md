---
title: "单例模式、内存对齐"
date: 2026-08-13
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

# 单例模式、内存对齐

## 单例模式
- 为什么要有单例模式？
    - 资源全局唯一：日志、配置文件、线程池、连接池、设备句柄、事件总线等，天生只该存在一份，到处传引用又麻烦又容易传错。
    - 共享状态：所有模块访问同一份状态，天然实现协作，不用靠参数层层传递。
    - 懒加载：第一次用到才创建，启动时不占资源。
    - 控制生命周期：由单例自己管理创建/销毁时机。

    > 
    > 反面提醒：它本质是"换皮的全局变量"，难测试、隐藏依赖。现代工程常用依赖注入替代，但单例仍有大量适用场景。
    > > 关于依赖注入，需要到学习了继承、多态之后，回头来讨论。[testdi.cc](/TestCode/cppDay5/testdi.cc)

- 单例模式的核心思想是什么？
    - 单例 = 私有构造函数（外部无法 `new`）+ 一个静态入口（`getInstance`）保证全局只有一份实例。你的直觉"同一个实例"方向对，但实现手段不是"构造函数返回"，而是"封装 + 静态持有"。

- 单例的写法  [MeyerSingleton.cc](/TestCode/cppDay5/MeyerSingleton.cc)：
    ```c++
    #include <string>

    class Logger {
      Logger() = default;
      ~Logger() = default;

    public:
      static Logger &getInstance() {
        static Logger instance; // 首次调用后创建唯一实例
        return instance;
      }

      void log(const std::string &msg) { /** Test Code **/ }
      Logger(const Logger &) = delete;
      Logger &operator=(const Logger &) = delete;
    };

    int main(int argc, char *argv[]) {
      Logger::getInstance().log("Hello, this is singleton. ");

      return 0;
    }
        

    ```
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
### 1、实现堆上单例模式的代码
```c++
#include <iostream>

class TestSingleton {

public:
  static TestSingleton &getInstance() {
    // 两个线程同时调用可能会破坏单例模式
    // 两个线程同时判断可能会有竞态
    // 线程不安全，若要线程安全，参考 ImproveSingleton.cc
    if (_pInstance == nullptr) {
      _pInstance = new TestSingleton();
    }
    return *_pInstance;
  }

  void TestFunc() { std::cout << "Test Func" << std::endl; }

  static void destroy() {
    if (_pInstance) {
      delete _pInstance;
      _pInstance = nullptr;
    }
  }

  TestSingleton(const TestSingleton &) = delete;
  TestSingleton &operator=(const TestSingleton &) = delete;

private:
  TestSingleton();
  ~TestSingleton();

  // 必须是静态，才能让所有对象共享一个内容。
  static TestSingleton *_pInstance;
};

/************************************************************************
             如果此处没有堆_pInstance 进行赋值，那么会产生链接错误。
*************************************************************************/
TestSingleton *TestSingleton::_pInstance = nullptr;

TestSingleton::TestSingleton() { std::cout << "TestSingleton()" << std::endl; }
TestSingleton::~TestSingleton() {
  std::cout << "~TestSingleton()" << std::endl;
}

int main(int argc, char *argv[]) {
  // 如果 getInstance 返回类型是 * 而不是 &，那么
  // 下面的三个指针在各自调用了 destroy 后，会悬空，
  // 重新访问就崩溃。所以用完需要置空。
  TestSingleton &ps1 = TestSingleton::getInstance();
  TestSingleton &ps2 = TestSingleton::getInstance();
  TestSingleton &ps3 = TestSingleton::getInstance();
  ps1.TestSingleton::destroy();
  ps2.TestSingleton::destroy();


  return 0;
}

#if 0
int main(int argc, char *argv[]) {
  TestSingleton *ps1 = TestSingleton::getInstance();
  TestSingleton *ps2 = TestSingleton::getInstance();
  TestSingleton *ps3 = TestSingleton::getInstance();
  ps1->TestSingleton::destroy();
  ps2->TestSingleton::destroy();
  ps3->TestSingleton::destroy();
  ps1 = nullptr;
  ps2 = nullptr;
  ps3 = nullptr;

  return 0;
}
#endif

```


### 2、其他实现。
上课的时候，单例模式的代码中，对象是放在堆上的，大家可以看看除了堆还有哪些地方可以存放这个唯一的对象，可以写出这样的代码？(提示：可以看看全局的、静态的、栈上的。)


> 这是对象在静态区的 Meyer 版本
```c++

#include <string>

class Logger {
  Logger() = default;
  ~Logger() = default;

public:
  static Logger &getInstance() {
    static Logger instance; // 首次调用后创建唯一实例
    return instance;
  }

  void log(const std::string &msg) { /** Test Code **/ }
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;
};

int main(int argc, char *argv[]) {
  Logger::getInstance().log("Hello, this is singleton. ");

  return 0;
}


```


> 要让单例实例在栈上，那就必须是在main函数中的一个唯一对象，传参调用
```c++
#include <iostream>

class Logger {
public:
  void log(const std::string &msg) { std::cout << msg << std::endl; }
};

void doWorkA(Logger &log) { log.log("doWorkA"); }
void doWorkB(Logger &log) { log.log("doWorkB"); }

int main() {
  Logger logger;          // 栈上唯一实例，存活到 main 结束 = 程序结束
  doWorkA(logger);        // 传引用共享
  doWorkB(logger);
  return 0;
}


```


