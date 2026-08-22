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
缓冲区又叫做缓存，它是内存空间的一部分，也就是说，在内存空间中预留了一定的存储空间，这些存
储空间用来缓冲输入或输出的数据，这部分预留的空间就叫做缓冲区。缓冲区根据其对应的是输入设备
还是输出设备，分为输入缓冲区和输出缓冲区。

### 引入缓冲区的原因
由于cpu读写速度非常快，但是磁盘IO是很慢的。为了让高速运行的 cpu 可以跟慢速的磁盘打印机协同工作，缓冲区应运而生。
缓冲区就是一块内存区，它用在输入输出设备和CPU之间，用来缓存数据。它使得低速的输入输出
设备和高速的CPU能够协调工作，避免低速的输入输出设备占用CPU，解放出CPU，使其能够高效率工
作。

### 缓冲区的类型
缓冲区分为三种类型：全缓冲、行缓冲和不带缓冲。
- 全缓冲：在这种情况下，当填满标准I/O缓存后才进行实际I/O操作。全缓冲的典型代表是对磁盘文
件的读写。

- 行缓冲：在这种情况下，当在输入和输出中遇到换行符时，执行真正的I/O操作。这时，我们输入
的字符先存放在缓冲区，等按下回车键换行时才进行实际的I/O操作。典型代表是键盘输入数据。

- 不带缓冲：也就是不进行缓冲，标准出错情况cerr/stderr是典型代表，这使得出错信息可以直接尽
快地显示出来。


### C++ 标准IO
- 标准输入流
    - istream 类定义了 1 个输入流对象，即 cin，代表的是标准输入，它从标准输入设备（键盘）获取数据
    - 程序中的变量通过流提取符 `>>` 从流中提取数据，流提取符`>>`从流中提取数据时通常跳过输入流中的空格、tab键、换行符等空白字符。 
    - 只有在输入完数据再按回车键后，该行数据才被送入键盘缓冲区，形成输入流，提取运算符`>>`才能从中提取数据

这个例子中，每次从 cin 中获取一个字符
```c++
void test()
{
    char ch;
    
    while((ch = cin.get()) != '\n')
    {
        cout << ch;
    }
    cout << ch;
}
```

获取一行用`getline()`




```c++
void test4()
{
    char buffer[1024] = {0};
    cout << "pls input a line string:" << endl;
    // 获取一行数据，在 linux 上，一个缓冲区大小默认是 1024 字节
    cin.getline(buffer, 1024);
    cout << "the line is:" << buffer << endl;
}


```

- 标准输出流
    * ostream类定义了3个全局输出流对象，即cout,cerr,clog，平常用的最多的就是cout,即标准输出。 
    * cout将数据输出到终端，它与标准C输出stdout关联。cerr是标准错误流（非缓冲），clog也是标准错误流（带缓冲）。
    * 注意：在C语言中，标准输入、标准输出和标准错误分别用0, 1, 2文件描述符代表。

    > clog,cerr 它们俩都是标准错误流，区别在于cerr不经过缓冲区，直接向终端输出信息，而clog中的信息是存放在缓冲区的，缓冲区满后或遇到endl向终端输出。

- 输出缓冲区
    - 输出缓冲区内容刷新的意思是：输出缓冲区的内容写入到真实的输出设备或者文件。
    - 如下几种情况会导致输出缓冲区内容被刷新：
        * 1. 程序正常结束（有一个收尾操作就是清空缓冲区）；
        * 2. 缓冲区满（包含正常情况和异常情况）；
        * 3. 使用操纵符显式地刷新输出缓冲区，如：endl、flush、ends(没有刷新功能)；
        * 4. 使用unitbuf操纵符设置流的内部状态；
        * 5. 输出流与输入流相关联，此时在读输入流时将刷新其关联的输出流的输出缓冲区。
        ```c++
        // 演示缓冲区满
        void test7()
        {
            //在Ubuntu上演示，因在linuxs行默认是1024
            for(size_t idx = 0; idx < 1024; ++idx)
            {
                cout << 'a';
            }
            sleep(5);
            cout << 'b';
        }

        ```
    - 使用操纵符，ifstream（文件输入流）, ofstream（文件输出流）, fstream（文件输入输出流），他们的构造函数形式都很类似:
    ```c++
    void test8()
    {
        // endl: 用来完成换行，并刷新缓冲区
        cout << "hello, world!" << endl;//立刻换行输出
        cout << "hello, 德玛西亚Garen";//不确定啥时候会输出
        sleep(5);
        // ends: 在输入后加上一个空字符，但是没有刷新缓冲区（这个需要注意，很多书上说可以刷新缓冲区）
        cout << "hello, 寒冰射手Ashe" << ends;
        // flush: 用来直接刷新缓冲区的
        cout << "hello, 无极剑圣Master Yi" << flush;
        // unitbuf: 在每次执行完写操作后都刷新输出缓冲区
        // nounitbuf: 让流回到正常的缓冲方式
        cout << unitbuf << "hello, 齐天大圣Wukong" << nounitbuf;
    }
    ```
### tie 操作
为了让输出的提示能够出现在让用户输入之前，即使没有遇到“全缓冲（满）、行缓冲（换行等）”，也要把缓冲区的内容刷新出来。为什么我们平时没有注意到必须要用tie呢？因为<span style = "color:red">标准库默认已经把 cin tie 到了 cout（和 cerr）</span>。

<span style="color:#3b82f6">这里我测试下来，只有在cin解绑了cerr和cout时，会出现缓冲被刷不及时的情况</span>

| 机制                               | 作用              | 能否用 tie/sync 关掉           |
| ---                                 | --                | ---                            |
| tie()（cin/cerr/clog→cout）        | 读/写前刷 cout    | ✅ 能（tie(nullptr)）          |
| 行缓冲（终端才有的 flush-on-read） | 读终端前刷 stdout | ❌ 关不掉，这是终端/stdio 行为 |

```c++
#include <iostream>
#include <string>

using std::string;
using std::cin;
using std::cout;


void test4() {
  std::ios::sync_with_stdio(false); // 全程序第一行，任何 IO 之前
  cin.tie(nullptr);
  cerr.tie(nullptr);

  string s;
  cout << "PROMPT: ";     // 应留在缓冲区，不显示
  cerr << "[读前打点]\n"; // cerr 无缓冲，立刻显示
  cin >> s;               // 光标等输入，此刻看不到 PROMPT:
  cerr << "[读后打点]\n";
  cout << "you typed: " << s << endl;
}
void test5() {
  // 程序结束后，才进行刷新
  cout << "test";
  sleep(5);
}

int main() {

    test4();

    test5();

}


```
## 隐式转换
cpp 中有独特的隐式转换，比如：
```c++
#include <iostream>

using std::cin;
using std::cout;

class Test {
private:
  int _a;
  int _b;

public:
  Test(int a = 0, int b = 0) : _a(a), _b(b) {}
  void print() { std::cout << _a << "  " << _b << std::endl; }
  ~Test() {}
};

int main(int argc, char *argv[]) {
  // 这里发生了隐式转换
  // 实际上被转换为了 Test test = Test(5);
  Test test = 5;
  test.print();

  return 0;
}
```

很多时候，我们并不想要这样的转换，因为这并不是我们设定好的行为，有时候，这样的行为会增加二义性。
```c++
void f(Test t);
void f(int x);

f(5);   // 到底调哪个？这里 int 更匹配 f(int)，但规则复杂易踩坑
```
因此我们有时会使用`explicit`关键字，禁止隐式转换，代码就写成了
```c++
#include <iostream>

using std::cin;
using std::cout;

class Test {
private:
  int _a;
  int _b;

public:
  explicit
  Test(int a = 0, int b = 0) : _a(a), _b(b) {}
  void print() { std::cout << _a << "  " << _b << std::endl; }
  ~Test() {}
};

int main(int argc, char *argv[]) {
  // 这里发生了隐式转换
  // 实际上被转换为了 Test test = Test(5);
  // 给构造函数加上 explicit 后，这句会报错
  Test test = 5; // ERROR
  test.print();

  return 0;
}


```


## 文件操作

### c++ 文件IO
所谓“文件”，一般指存储在外部介质上数据的集合。一批数据是以文件的形式存放在外部介质上的。
操作系统是以文件为单位对数据进行管理的。要向外部介质上存储数据也必须先建立一个文件（以文件名标识），才能向它输出数据。根据文件中数据的组织形式，可分为ASCII文件和二进制文件。
外存文件包括磁盘文件、光盘文件和U盘文件。目前使用最广泛的是磁盘文件。
文件流是以外存文件为输入输出对象的数据流。文件输入流是从外存文件流向内存的数据，文件输出流是从内存流向外存文件的数据。每一个文件流都有一个内存缓冲区与之对应。
文件流本身不是文件，而只是以文件为输入输出对象的流。若要对磁盘文件输入输出，就必须通过文件流来实现。

C++对文件进行操作的流类型有三个: ifstream（文件输入流）, ofstream（文件输出流）, fstream（文件输入输出流），他们的构造函数形式都很类似:

```c++
ifstream();
explicit ifstream(const char *filename, openmode mode = in);
explicit ifstream(const string &filename, openmode mode = in);

ofstream();
explicit ofstream(const char *filename, openmode mode = out);
explicit ofstream(const string &filename, openmode mode = out);

fstream();
explicit fstream(const char *filename, openmode mode = in|out);
explicit fstream(const string &filename, openmode mode = in|out)

```
### 文件模式
根据不同的情况，对文件的读写操作，可以采用不同的文件打开模式。文件模式在GNU GCC7.4源码实
现中，是用一个叫做openmode的枚举类型定义的，它位于ios_base类中。文件模式一共有六种，它们分别是:
- in: 输入，文件将允许做读操作；如果文件不存在，打开失败
- out: 输出，文件将允许做写操作；如果文件不存在，则直接创建一个
- app: 追加，写入将始终发生在文件的末尾
- ate: 末尾，读操作始终发生在文件的末尾
- trunc: 截断，如果打开的文件存在，其内容将被丢弃，其大小被截断为零
- binary: 二进制，读取或写入文件的数据为二进制形式如果文件不存在，则直接创建一个


简单读文件可以用ifstream的相关方法>>，简单写文件可以用ofstream的相关方法<<:

```c++
void test()
{
    ifstream ifs("testvec.cc");
    if(!ifs.good())
    {
        cerr << ">> ifstream open file error!\n";
        return;
    }
    string line;
    vector<string> vec;
    while(getline(ifs, line)
    {
        //cout << line << endl;
        vec.push_back(line);
    }
    ifs.close();
    ofstream ofs("a.txt");
    if(!ofs.good())
    {
        cerr << ">> ofstream open file error!\n";
        return;
    }
    for(auto &elem : vec)
    {
        ofs << elem << '\n';
    }
    ofs.close();
}


```
***一些常用的操作：***

- seek = 移动指针（seekg 读 / seekp 写），tell = 读指针位置（tellg/tellp）。
- 方向：beg 开头、cur 当前、end 末尾。

```c++
// seek 系列：移动读写指针
// fstream fs("a.txt", ios::in | ios::out);

fs.seekg(0);                        // 把"读指针"移到文件开头（绝对位置）
fs.seekp(10, ios::beg);             // 把"写指针"从开头偏移 10 字节
fs.seekg(-5, ios::cur);             // 读指针从当前位置回退 5 字节
fs.seekp(0, ios::end);              // 写指针移到文件末尾

// tell 系列：获取当前指针位置
streampos pos = fs.tellg();         // 读指针当前位置
streampos wpos = fs.tellp();        // 写指针当前位置

// 经典用途：获取文件大小
fs.seekg(0, ios::end);   // 移到末尾
auto size = fs.tellg();  // 末尾偏移 = 文件大小
fs.seekg(0, ios::beg);   // 记得移回来

// C 标准库（fseek/ftell）
FILE* fp = fopen("a.txt", "r");
fseek(fp, 0, SEEK_END);          // 移到末尾
long size = ftell(fp);           // 得到大小
fseek(fp, 0, SEEK_SET);          // 移回开头
rewind(fp);                      // 等价 fseek(fp, 0, SEEK_SET)

```
 方向参数（三个，C++ 和 C 通用）
| 常量     | 含义         | C 里对应 |
|----------|--------------|----------|
| ios::beg | 从开头算     | SEEK_SET |
| ios::cur | 从当前位置算 | SEEK_CUR |
| ios::end | 从末尾算     | SEEK_END |


<mark>两个常见的坑:</mark>

<mark>1. 读写切换要刷新/定位：fstream 在写和读之间切换，标准要求先 seekg/seekp（或 flush），否则行为未定义。</mark>

<mark>2. 大文件（>2GB）：ftell 返回 long（32 位系统只有 2GB），要用 ftello/fseeko（off_t）或 C++ 的 streampos（本身支持大文件）。</mark>


一次性拿到短文件数据：
```c++
void test12()
{
    ifstream ifs("a.txt");

    if(!ifs.is_open())
    {
        cerr << "ifstream open file error!\n";
        return;
    }

    ifs.seekg(0, std::ios_base::end);// 以 end 为基准移动0个位置：0 + end = end
    std::streamsize length = ifs.tellg();// 
    ifs.seekg(0, std::ios_base::beg);
    char *buff = new char[length + 1]();
    ifs.read(buff, length + 1);
    string content(buff, length + 1);
    cout << content << endl;

    delete [] buff;
}

```

复制文件：


***复制文件应该用ios::binary(二进制模式)，原因是使用二进制文件模式时，程序将数据从内存传递给文件，将不会发生任何隐藏的转换，而默认状态下是文本模式，复制的内容可能会发生改变。***

```c++

void test13()
{
    fstream in("a.txt", std::ios::in|std::ios::binary);

    if(!in.is_open())
    {
        cerr << "fstream open file error!\n";
        return;
    }
    fstream out("a.txt", std::ios::out|std::ios::binary);

    if(!out.is_open())
    {
        cerr << "fstream open file error!\n";
        return;
    }
    out << in.rdbuf();//流的重定向
    out.close();
    in.close();
}


```
## vector的扩容机制
在linux/mac上，vector的扩容机制是每次两倍扩大，实现时候，是将旧的小空间值复制到新空间去，然后丢掉旧空间。

```c++
#include <iostream>
#include <vector>

using std::vector;

void printVec(const vector<int> &vec) {
  std::cout << "the size of the vec is " << vec.size() << std::endl;
  std::cout << "the capacity of the vec is " << vec.capacity() << std::endl;
}

int main(int argc, char *argv[]) {
  vector<int> ivec;

  for (int i = 0; i < 9; i++) {
    ivec.push_back(i);
    printVec(ivec);
  }

  return 0;
}
```

## 作业

一、选择题

1、执行以下程序

D, str没有指向的空间,直接访问，会出错
```C++
char *str;
cin >> str;
cout << str;
若输入abcd 1234，则输出（ ）
A. abcd        B. abcd 1234         C.1234         D. 输出乱码或错误
```



2、执行以下程序

basic_istream& getline( char_type* s, std::streamsize count, char_type delim );
(2)
A getline 的结束符号是 ‘ ’ 。
```C++
char a[200];
cin.getline(a, 200, ' ');
cout << a;
若输入abcd 1234，则输出（ ）
A. abcd      B. abcd 1234      C.1234      D. 输出乱码或错误
```



二、编程题

1、实现new/delete表达式中只能生成栈对象的代码和只能生成堆对象的代码。

对象只能在堆不能在栈，那么需要构造函数可用，new/delete操作符函数可用，析构函数私有，不可用
[objonheap.cc](/TestCode/cppDay6/objonheap.cc)
```c++
#include <iostream>

class HeapObj {
public:
  HeapObj(int a = 0) : _a(a) {}
  void operator delete(void *pret) {
    std::cout << "void operator delete(void *pret)" << std::endl;
    free(pret);
  }

  void destroy() {
    std::cout << "destroy()" << std::endl;
    delete this; // 删掉对象本身
  }

  void *operator new(size_t sz) {
    std::cout << "void *operator new(size_t sz)" << std::endl;
    void *ptr = malloc(sz);
    return ptr;
  }

private:
  ~HeapObj() {}
  int _a;
};

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  // HeapObj hpstack(10);
  HeapObj *hp = new HeapObj(10);

  return 0;
}
```

对象在栈上，那么需要构造函数可用，但是new/delete至少一个不可用。
[objonstack](/TestCode/cppDay6/objonstack.cc)
```c++
#include <iostream>

class HeapObj {
public:
  HeapObj(int a = 0) : _a(a) {}

  void *operator new(size_t sz) {
    std::cout << "void *operator new(size_t sz)" << std::endl;
    void *ptr = malloc(sz);
    return ptr;
  }

  void destroy() {
    std::cout << "destroy()" << std::endl;
    delete this; // 删掉对象本身
  }

  ~HeapObj() {}

private:
  void operator delete(void *pret) {
    std::cout << "void operator delete(void *pret)" << std::endl;
    free(pret);
  }

  int _a;
};

int main(int argc, char *argv[]) {

  HeapObj hpstack(10);
  // HeapObj *hp = new HeapObj(10);// ERROR
  // hp->destroy();

  return 0;
}

```

2、统计一篇英文(The_Holy_Bible.txt)文章中出现的单词和词频，
输入：某篇文章的绝对路径
输出：词典（词典中的内容为每一行都是一个“单词 词频”）

[dictionary.cc](/TestCode/cppDay6/dictionary.cc)

词典的存储格式如下

```C++

|   a 66          |
|   abandon 77    |
|   public 88     |
|    ......	      |
|_________________|

struct Record
{
	string _word;
	int _frequency;
};

class Dictionary
{
public:
	//......
    void read(const std::string &filename);
    void store(const std::string &filename);
private:
	vector<Record> _dict;
};
```

提示：因为我们需要统计圣经文件中单词以及该单词在文件中出现的次数，所以可以看去读圣经文件，然后将单词存到数据结构中，并记录单词的次数，如果单词第二次出现的时候，只需要修改单词的次数（也就是这里说的单词的频率），这样当统计完整个圣经文件后，数据都存在数据结构vector了。接着遍历vector数据结构就可以将单词以及单词次数(也就是频率)存储到另外一个文件。(当然如果不存到另外一个文件，就只能打印到终端了)

注意：在读圣经文件的时候，有可能字符串是不合法的，比如：abc123   abc？这样的字符串，处理方式两种：直接不统计这样的字符串或者将非法字母去掉即可。

最终得到结果类似：

a    10
public   20
welcome   30
.......
