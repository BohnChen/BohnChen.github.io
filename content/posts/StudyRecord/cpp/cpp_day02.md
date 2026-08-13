---
title: "重载、强制类型转换、内联函数、C 风格和 C++风格的字符串、内存分配"
date: 2026-08-06
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

# 重载、强制类型转换、内联函数、C 风格和 C++风格的字符串、内存分配

## 强制类型转换
在c语言中，强制类型转换通过括号就能实现，非常自由，也非常不安全。为了避免强制类型转换出现的问题，`C++ `创造了四种**强制类型转换运算符**来告诉编译器你的意图，从而让编译器最大可能对你提醒。

### 四种 `C++ cast` 把不同语义拆分出去:

-   - 让能安全的被编译器保护（static_cast），
-   - 可能有危险的危险转换的被显式标注（const_cast、reinterpret_cast），
-   - 能运行时检查的就做运行时检查（dynamic_cast）。

C 风格括号搞定一切，等于一个工具干四种完全不同语义的活，编译器无法根据你的意图做任何区分检查。
C 的不安全在于，强制转换无论是内置类型之间还是内置类型和指针之间都可以随意转换。我们举例子说明：

#### 1. 错误地将 `double*` 转成 `int*`
`C `风格——编译通过，运行时完全错误：
```c++

double d = 3.14;
int* p = (int*)&d;   // 编译通过，但把浮点数的二进制当整数读，结果完全错误
int x = *p;          // 未定义行为
```

`C++ static_cast`——直接编译失败：
```c++
double d = 3.14;
int* p = static_cast<int*>(&d);   // 编译错误：static_cast 不允许无关指针类型互转

```
`static_cast` 在编译期就发现 `double*` 和 `int*` 之间不存在合法的转换路径，直接拒绝。
如果你真的非要做（尽管这是错的），唯一的途径是：
```c++
int* p = reinterpret_cast<int*>(&d);  // 编译通过，但 `reinterpret_cast` 这个名字就是红色警报
```
代码中看到 `reinterpret_cast`，审阅者立刻知道这里有危险操作，不会被轻易放过。

#### 2. 悄悄去除 const 属性
`C` 风格——`const` 被无声吞掉：
```c++

void modify(const int* cip) {
    int* p = (int*)cip;  // const 丢了，没有任何警告
    *p = 42;             // 未定义行为，编译器帮不了你
}

int main() {
    const int n = 10;
    modify(&n);          // n 明明声明为 const，值却被改了，程序逻辑完全混乱
}

```
`C++ const_cast`——意图被显式标记，且可能根本不该用：
```c++
void modify(const int* cip) {
    int* p = const_cast<int*>(cip);  // "const_cast" 四个字就是危险标签，你没法滥用而不自知
    *p = 42;  // ⚠️ 如果 cip 真的指向一个 const 变量，这仍然是未定义行为
}

int main() {
    const int n = 10;
    modify(&n);  // 问题仍然存在，但审查代码时一眼就看到 const_cast
}
```

更进一步：`C++ `的做法是根本不用 `const_cast`，而是区分只读和可写：
```c++
// 如果参数确实是只读的，就用 const int*
void print_only(const int* p) { 
    printf("%d\n", *p);  // 不需要转换
}

// 如果参数需要修改，就接受非 const 参数
void modify(int* p) {
    *p = 42;
}
```
类型系统替你挡掉错误调用——`const int*` 根本传不进去 `modify`。

#### 3. 沿继承链的错误向下转型
`C` 风格——不安全转换导致崩溃：
```c++
struct Base { int x; };
struct Derived { int x; int y; };

Base* b = malloc(sizeof(Base));
b->x = 10;
Derived* d = (Derived*)b;  // b 根本不是 Derived，编译完全不拦
int val = d->y;             // 访问不存在的成员，运行时 crash 或读到脏数据
C++ dynamic_cast——运行时检测，失败返回 nullptr：
struct Base {
    virtual ~Base() = default;  // 需要虚函数以启用 RTTI
};
struct Derived : Base { int y = 42; };

Base* b = new Base;
Derived* d = dynamic_cast<Derived*>(b);  // 运行时检查：b 不是 Derived
if (d == nullptr) {
    // 安全处理失败情况
} else {
    int val = d->y;   // 只有转换成功才执行，不会访问无效内存
}

```
`dynamic_cast` 查虚表发现 `b` 的真实类型是 `Base`，拒绝转换并返回 `nullptr`。
如果用 `static_cast` 做同样的事：
```c++
Derived* d = static_cast<Derived*>(b);  // 编译通过，但这是“你向我保证 b 是 Derived”
d->y;  // 你撒谎了，未定义行为——和 C 风格一样不安全
```
区别在于 `static_cast` 的名字在告诉你：“我在做一个静态假设，没有运行时检查，请确保你的逻辑是对的”。



## inline 内联函数
`inline`函数只是给编译器的一个建议，编译器如果发现你建议是 `inline`类型的函数会使代码变得臃肿，它就不会接受你的建议。

inline 只是请求，编译器在以下情况会忽略它，生成普通 call 指令：
 | 情况              | 原因                                           |
 | ---               | ----                                           |
 | 函数体过大        | 内联会导致代码膨胀，反而降低 I-cache 命中率    |
 | 递归函数          | 不可能完全展开（除非尾递归编译器能优化时）     |
 | 函数地址被取      | 必须有独立地址才能取 &add                      |
 | 虚函数调用        | 运行时通过虚表分发，编译期不知道具体函数       |
 | 包含循环/复杂分支 | 编译器认为不值得内联                           |
 | -O0 优化级别      | 不开启优化时基本不内联                         |
 | 显式禁止          | __attribute__((noinline)) 或 [[gnu::noinline]] |


  可用 `g++ -Winline` 看到编译器放弃内联的警告。 
```c++

// 递归函数，编译器会把其作为普通函数对待
inline int TestInline_1(int n) {
	if(n == 1) {
		return 0;
	}
	if (n == 2) {
		return 1;
	}
	return TestInline_1(n - 1) + TestInline_1(n - 2);
}

int main() {
    TestInline_1();
    return 0;
}
|           |
|           |
V           V
int main() {
    // 普通函数调用
    TestInline_1(4);
    return 0;
}
```

从本质上说，inline函数是为了省去函数调用的开销，直接将函数体替换到调用位置，但是其与带参宏定义又不同，因为内联函数在编译期处理，宏定义在预处理阶段做文本替换；内联函数有类型检查，宏定义没有；内联函数可设断点调试，宏定义不可以；具体还有如下不同：

|              | 内联函数               | 带参宏                     |
| --           | --                     | --                         |
| 处理阶段     | 编译期（编译器）       | 预处理期（文本替换）       |
| 类型检查     | ✅ 有                  | ❌ 无                      |
| 参数求值次数 | 1 次（正常函数语义）   | N 次（每处引用就展开一次） |
| 作用域       | 遵循命名空间、类作用域 | 无作用域概念               |
| 重载         | ✅ 支持                | ❌ 不支持                  |
| 可调试       | ✅ 可设断点            | ❌ 无法调试                |
| 地址         | 有（编译器可保留一份） | 无                         |
| #/## 操作符  | ❌ 不支持              | ✅ 支持                    R
 


```c++

// testInline.cc, 若inline函数太臃肿，编译器会把其作为普通函数对待
inline
void TestInline_1 (int a, int b) {
    cout << "a + b = " << a + b << endl;
}

int main() {
    TestInline_1();
    return 0;
}
|           |
|           |
V           V
int main() {
    // inline直接替换了
    cout << "a + b = " << a + b << endl;
    return 0;
}

// 宏定义只做文本替换的例子
#include <iostream>
#define ADD(x, y) {return (x) + (y);}

inline int add(int x, int y) {return x + y;}

int main(int argc, char *argv[]) {
#if 0
	// main 函数提前返回
	ADD(3, 4);
	// 无法输出下面这句
	std::cout << "hi" << std::endl;
#endif
	add(3,4);
    // 正常输出
	std::cout << "hi" << std::endl;
	return 0;
}

```


> 关于#/##：不过现代 C++ 中用得越来越少——模板、if constexpr、反射（C++26）在很多场景可以替代宏拼接。# 在断言/日志中还算常见，## 主要出现在那些不得不操作标识符名的底层框架代码里。


由于inline做的是直接将函数体放在调用位置的工作，因此，**要在同一个文件中同时找到声明和实现**

在同一个文件：
```c++
// math.h
#pragma once
inline int add(int a, int b) { return a + b; }  // 声明+定义都在头文件
// main.cpp
#include "math.h"
int main() { return add(1, 2); }   // 能看到函数体，可内联 ✅
// other.cpp
#include "math.h"
int other() { return add(3, 4); }  // ✅ 也不会报重定义，因为有 inline
```

声明和实现在不同的文件，那就把他们拉到同一个文件中来:
```c++
// math.h
#pragma once
inline int add(int a, int b);    // 声明
#include "math.ipp"              // 把定义"拉进来"

// math.ipp
inline int add(int a, int b) { return a + b; }  // 定义

// main.cpp — 只需包含 .h
#include "math.h"   // math.ipp 的内容被拉进 main.cpp，函数体可见 ✅

```
实现和声明不在同一个文件中时，会报错：
```c++
// math.h
#pragma once
int add(int a, int b);             // 只有声明，函数体不可见

// math.cpp
#include "math.h"
inline int add(int a, int b) { return a + b; }  // 实现在此

// main.cpp
#include "math.h"   // 只看到声明，看不到函数体
int main() { return add(1, 2); }   
// ❌ 链接错误: undefined reference to add(int, int)
// inline 函数在 math.cpp 有内部链接，对外不可见
```
## C/C++风格字符串
`C`风格字符串使用:
```c
#include <cstdio>
#include <cstring>
#include <cstdlib>

void CStringUse() {
	// 堆空间
	char *pStr = (char *)malloc(11);
	strcpy(pStr, "hello");
	printf("%s\n", pStr);
	strcat(pStr, "world");
	pStr[0] = 'H';
	printf("%s\n", pStr);
	// 手动释放
	free(pStr);
	// 置空防止再次访问
	pStr = NULL;

	// 栈数组
	char str[10];
	strcpy(str, "nice");
	printf("%s\n", str);

}

int main(int argc, char *argv[]) {
	CStringUse();
	return 0;
}

```
c++中的string写法就方便非常多，而方便的源头得益于 cpp 写了很多运算符重载函数

```c++

#include <iostream>
#include <string>

using std::string;

int main(int argc, char *argv[]) {
	string str = "hello";
	std::cout << str << std::endl;
	str = str + "world";
	std::cout << str << std::endl;
	return 0;
}

```



### C风格字符串的核心操作:
声明与初始化
```c
char s1[] = "hello";              // 栈上，大小自动为 6（含 '\0'）
char s2[10] = "hello";            // 栈上，剩余空间填 '\0'
const char* s3 = "hello";         // 指针指向 .rodata 中的字符串字面量
char s4[] = {'h', 'e', 'l', 'l', 'o', '\0'};
```

常用方法
```c
#include <cstring>

// 长度
size_t len = strlen(s1);          // 5（不含 '\0'）

// 拷贝
char dst[20];
strcpy(dst, s1);                  // 拷贝到 dst
strncpy(dst, s1, sizeof(dst)-1);  // 安全版，限制长度
dst[sizeof(dst)-1] = '\0';        // 手动加终止符

// 拼接
strcat(dst, " world");            // dst = "hello world"
strncat(dst, src, n);             // 安全版

// 比较
if (strcmp(s1, s2) == 0)          // 相等返回 0，不能用 s1 == s2
if (strncmp(s1, s2, 3) == 0)      // 只比前 3 个字符

// 搜索
char* p = strchr(s1, 'e');        // 找字符，返回指针
char* p = strstr(s1, "ll");       // 找子串
```
输入输出:
```c
char buf[100];
scanf("%s", buf);                 // 遇到空格停止（不安全）
fgets(buf, sizeof(buf), stdin);   // 保留空格，但会包含换行
printf("%s", buf);
puts(buf);                        // 自动加换行
``` 
常见坑:
1. == 比较的是地址，不是内容：
```c
if (s1 == s2)          // 错！比的是指针地址
if (strcmp(s1, s2) == 0)  // 对
```
2. strcpy 没有边界检查，容易溢出：
```c
char buf[5];
strcpy(buf, "hello world");  // 缓冲区溢出，未定义行为
```
3. 修改字符串字面量是 UB：
```c
char* s = "hello";
s[0] = 'H';             // 未定义行为，"hello" 在 .rodata 中不可写
// 应改为
char s[] = "hello";
s[0] = 'H';             // 正确，s 在栈上可写
```
4. 结尾 \0 被遗忘：
```c
char buf[5] = {'h', 'e', 'l', 'l', 'o'};  // 没有 '\0'
printf("%s", buf);  // 读到 '\0' 才停，越界！
```
用 \<string> 里的 std::string 可以避开上述大部分问题。

### CPP中的字符串核心操作：

C++ 用 std::string（<string>），自动管理内存，不需要手动申请、释放、担心 \0：
```c++

#include <string>
#include <iostream>

// 初始化
std::string s1 = "hello";
std::string s2("world");
std::string s3(10, 'x');          // "xxxxxxxxxx"

// 拼接
std::string s4 = s1 + " " + s2;   // "hello world"
s1 += " cpp";                      // 直接 +=

// 长度
size_t len = s1.size();            // 或 s1.length()
bool empty = s1.empty();

// 比较 — 直接用 == != < >，不再需要 strcmp
if (s1 == s2) { }
if (s1 < s2) { }                   // 字典序

// 访问字符
char c = s1[0];                    // 不检查越界
char c = s1.at(0);                 // 越界抛 std::out_of_range
char c = s1.front();               // 首字符
char c = s1.back();                // 尾字符

// 子串
std::string sub = s1.substr(0, 3); // 从位置0起取3个字符
size_t pos = s1.find("ll");        // 查找子串位置

// 插入删除
s1.insert(2, "xxx");               // 在位置2插入
s1.erase(2, 3);                    // 从位置2删3个字符

// C 风格互转
const char* cs = s1.c_str();       // string → C string
std::string s5 = cs;               // C string → string

```
核心区别： std::string 是 RAII 对象，出作用域自动释放，拷贝/赋值/拼接都是值语义，不用管 malloc/free/\0/边界越界。这是 C++ 中用字符串的首选方式，除非你在写纯 C 或与 C 库接口。

## 内存分配
关于内存分配，我们通过一张图和一份代码测试进行理解
![virtual_memory](/images/4_virtual_memory.png)

```c++

#include <iostream>

using std::cout;
using std::endl;

int globalint;//全局变量，位于全局区,初始化为0
char *globalpointer_1;//全局变量，位于全局区,初始化为nullptr
const int globalConstInt = 100;

int main(int argc, char **argv)
{
    const int LocalConstInt = 0;//局部常量位于栈上
    int localInt;//局部变量，位于栈区,初始化为随机值
    char *localPointer_2;//localPointer_2本身也是位于栈区
    char localStr_1[] = "hello";//localStr_1位于栈上
    static int LocalStaticInt = 10;//静态变量位于静态区

    int *localPointerToHeap = new int(10);//localPointerToHeap本身位于栈上， localPointerToHeap指向堆区
    const char *stringToText = "helloworld";//stringToText本身位于栈上，stringToText指向变量位于文字常量区的

    printf("\n打印变量的地址\n");
    printf("&globalint = %p\n", &globalint);
    printf("&globalpointer_1 = %p\n", &globalpointer_1);
    printf("globalpointer_1 = %p\n", globalpointer_1);
    printf("&localInt = %p\n", &localInt);
    printf("&localPointer_2 = %p\n", &localPointer_2);
    printf("localPointer_2 = %p\n", localPointer_2);
    printf("&localStr_1 = %p\n", &localStr_1);
    printf("localStr_1 = %p\n", localStr_1);
    printf("&LocalStaticInt = %p\n", &LocalStaticInt);
    printf("&localPointerToHeap = %p\n", &localPointerToHeap);
    printf("localPointerToHeap = %p\n", localPointerToHeap);
    printf("&stringToText = %p\n", &stringToText);
    printf("stringToText= %p\n", stringToText);
    printf("\"helloworld\"= %p\n", &"helloworld");//文字常量区
    printf("&main = %p\n", &main);//程序代码区
    printf("main = %p\n", main);
    printf("&globalConstInt = %p\n", &globalConstInt);
    printf("&LocalConstInt = %p\n", &LocalConstInt);

    printf("\n打印变量的值\n");
    printf("globalint = %d\n", globalint);
    printf("localInt = %d\n", localInt );

    delete localPointerToHeap;
    localPointerToHeap = nullptr;

    return 0;
}




```

输出：
```c++

// output

打印变量的地址
&globalint = 0x100b38008
&globalpointer_1 = 0x100b38010
globalpointer_1 = 0x0
&localInt = 0x16f2ce628
&localPointer_2 = 0x16f2ce620
localPointer_2 = 0xfffffffffffffff0
&localStr_1 = 0x16f2ce618
localStr_1 = 0x16f2ce618
&LocalStaticInt = 0x100b38000
&localPointerToHeap = 0x16f2ce610
localPointerToHeap = 0x1011a1b00
&stringToText = 0x16f2ce608
stringToText= 0x100b308ca
"helloworld"= 0x100b308ca
&main = 0x100b305d0
main = 0x100b305d0
&globalConstInt = 0x100b30a80
&LocalConstInt = 0x16f2ce62c

打印变量的值
globalint = 0
localInt = 1865213656
```


