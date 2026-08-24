---
title: "运算符重载、友元"
date: 2026-08-23T21:17:55+08:00
draft: false
categories: ["编程语言"]
tags: ["c/c++", "技术学习"]
---

## 运算符重载

### 为什么要运算符重载

由于现有的运算符含义只针对内置类型有效，要对自己定义的对象进行操作，就必须要有运算符重载。

```c++
class MyClass {};

int main() {
    MyClass c1;
    MyClass c2;

    // 未重载 + 运算符之前，对象无法直接相加
    MyClass c3 = c1 + c2; /*  ERROR 无法直接相加 */

}
```

### 重载规则

运算符重载不能改变符号原本的定义，也不能改变原本符号的操作数数量和顺序，且操作数中至少需要一个自定义类型或者枚举类型。

```c++
// ERROR 
// * overload operator+ must have at least
// one parameter of class or enumeration type
int operator+(int a, int b) {
    return a + b;
}
```

### 普通函数重载
普通函数重载访问私有成员，需要用类内的 `get`、`set` 函数。

```c++
/*  这里是重点：普通函数重载 operator+
    不能直接访问私有成员 _x、_y，必须通过 get/set

MyClass operator+(MyClass &a, MyClass &b) {
    MyClass tmp;
    tmp._x = a._x + b._x;         // ERROR 无法访问私有成员
    tmp._y = a._y + b._y;         // ERROR
    tmp._x = a.getx() + b.getx(); // 正确：通过 get/set
    tmp._y = a.gety() + b.gety();
}

*/ // 下面是完整代码

#include <iostream>

using std::cout;
using std::endl;

class MyClass {
public:
  MyClass() : _x(0), _y(0) {}
  MyClass(int a, int b) : _x(a), _y(b) {}
  ~MyClass() {}

  int getx() const { return _x; }
  int gety() const { return _y; }

  void setx(int x) { _x = x; }
  void sety(int y) { _y = y; }

  void print() const {
    cout << "_x is " << _x << ", _y is " << _y << ". " << endl;
  }

private:
  int _x;
  int _y;
};

MyClass operator+(const MyClass &a, const MyClass &b) {
  MyClass tmp;
  tmp.setx(a.getx() + b.getx());
  tmp.sety(a.gety() + b.gety());
  return tmp;
}

int main(int argc, char *argv[]) {
  cout << "Today is a nice day." << endl;

  MyClass c1(1, 1);
  MyClass c2(2, 2);
  MyClass c3 = c1 + c2;
  c1.print();
  c2.print();
  c3.print();

  return 0;
}


```
### 友元函数重载
友元函数可以直接访问私有成员。所以，通过友元函数，上面的代码就可以写成：
```c++
/*
类中的 friend 声明，然后实现时候直接访问 private 成员
MyClass operator+(const MyClass &a, const MyClass &b) {
  MyClass tmp;
  tmp._x = a._x + b._x;   // 直接访问私有成员
  tmp._y = a._y + b._y;
  return tmp;
}
*/
#include <iostream>

using std::cout;
using std::endl;

class MyClass {
public:
  MyClass() : _x(0), _y(0) {}
  MyClass(int a, int b) : _x(a), _y(b) {}
  ~MyClass() {}

  friend MyClass operator+(const MyClass &a, const MyClass &b);
  void print() const {
    cout << "_x is " << _x << ", _y is " << _y << ". " << endl;
  }

private:
  int _x;
  int _y;
};

MyClass operator+(const MyClass &a, const MyClass &b) {
  MyClass tmp;
  tmp._x = a._x + b._x;
  tmp._y = a._y + b._y;

  return tmp;
}

int main(int argc, char *argv[]) {
  cout << "Today is a nice day." << endl;

  MyClass c1(1, 1);
  MyClass c2(2, 2);
  MyClass c3 = c1 + c2;
  c1.print();
  c2.print();
  c3.print();

  return 0;
}
```


### 成员函数重载
由于非静态成员函数的第一个位置是 `this` 指针，所以作为成员函数重载时参数个数要**减一**：比如 `+` 运算符本来有两个操作数，用成员函数重载就只需要写一个参数（另一个是 `this`）。

```c++
MyClass operator+(const MyClass &rhs) {   // 成员函数：隐藏了 this（第一个操作数）
  MyClass tmp;
  tmp._x = _x + rhs._x;
  tmp._y = _y + rhs._y;
  return tmp;
}
```

![类内的二元运算符函数报错](/images/7_binary_operator_within_class.png)

### 复合赋值运算符
普通的运算符重载，比如 `+` 运算符，两个值返回一个新的值，推荐以友元函数进行运算符重载。

特殊的运算符重载，比如复合运算符 `*=、+=、/=` 等，由于需要修改第一个操作数（即 `*this`），所以建议用成员函数的方式进行重载。


```c++
MyClass &MyClass::operator*=(const MyClass &rhs) {
  this->_x *= rhs._x;
  this->_y *= rhs._y;
  return *this;
}


```

### 自增运算符重载
由于
```c++
int a = 3;
++a; // 表达式为 4， a = 4
a++; // 表达式为 4, a = 5
```
所以我们要考虑两个不同的 `++` 运算符重载

```c++
MyClass &operator++() {
  cout << "MycClass operator++ " << endl;
  _x++;
  _y++;

  return *this;
}

// 用一个无用的int来做前置++和后置++的区分
MyClass operator++(int) {
  cout << "MycClass operator++(int) " << endl;
  MyClass tmp(*this);
  _x++;
  _y++;
  // 返回临时对象，调用拷贝构造函数
  return tmp;
}

```

### 输出流运算符重载
不能把 `operator<<` 放到类中去，因为对于输出流运算符，第一个操作数是 `ostream`，如果放到类内，就改变了操作数顺序，这是不可以的。

另外，把输出流运算符的重载放到全局中作为普通函数实现时，会发现数据成员是私有的，想要访问，需要使用类内的 `get`、`set` 函数，因此我们最好将其声明为友元，这样重载函数可以直接对其进行操作。

```c++
// ERROR：不能作为成员函数重载，否则会改变操作数顺序
// std::ostream& operator<<(std::ostream& os, const MyClass& rhs);

// 正确：声明为友元的全局函数，可以直接访问私有成员
std::ostream &operator<<(std::ostream &os, const MyClass &rhs) {
  os << "x = " << rhs._x << " , y = " << rhs._y << ". " << std::endl;
  return os;   // 返回流引用，支持连续输出
}

int main(int argc, char *argv[]) {
  MyClass c1(1, 2);
  std::cout << c1;                       // 等价于 operator<<(std::cout, c1)
  return 0;
}
```


### 输入流运算符重载

与输出流运算符一样，第一个操作数是 `istream`，不能放到类内，只能以友元函数重载。与输出流不同的是，第二个操作数需要**写入修改**，所以不能加 `const`。

```c++
#include <iostream>

std::istream &operator>>(std::istream &is, MyClass &rhs) {
  is >> rhs._x >> rhs._y;   // 友元，直接访问私有成员
  return is;                // 返回流引用，支持连续输入
}

int main(int argc, char *argv[]) {
  MyClass c;
  std::cin >> c;          // 等价于 operator>>(std::cin, c)
  std::cout << c << std::endl;
  return 0;
}
```

> 输出流运算符的第二个参数是 `const MyClass&`（只读），输入流运算符的第二个参数是 `MyClass&`（要写入），这是两者唯一的差别。

### 函数调用运算符（小括号 / 仿函数）

重载 `operator()` 后，对象就能像函数一样被"调用"，这样的对象称为**函数对象 / 仿函数（functor）**。它只能以**成员函数**的形式重载，参数个数不限，也可以多次重载。

```c++
#include <iostream>

class Add {
public:
  int operator()(int a, int b) const {
    return a + b;
  }
};

int main(int argc, char *argv[]) {
  Add add;
  int sum = add(3, 4);      // 等价于 add.operator()(3, 4)
  std::cout << sum << std::endl;   // 7
  return 0;
}
```

函数对象最大的价值是**可以携带状态**（成员变量），比普通函数指针更灵活，常用于 `sort`、`for_each` 等算法的第三个参数。

### 下标访问运算符（中括号）

`operator[]` 只能以**成员函数**的形式重载。为了支持 `arr[0] = x` 这样的赋值，必须返回**引用**。

```c++
#include <iostream>

class MyArray {
public:
  MyArray() : _data(new int[10]) {}
  ~MyArray() { delete[] _data; }

  int &operator[](size_t idx) {
    return _data[idx];      // 返回引用，可读可写
  }

private:
  int *_data;
};

int main(int argc, char *argv[]) {
  MyArray arr;
  arr[0] = 42;              // 等价于 arr.operator[](0) = 42
  std::cout << arr[0] << std::endl;   // 42
  return 0;
}
```

> 如果返回值不加引用，`arr[0] = 42` 会编译失败——不能给一个右值（临时值）赋值。

### 总结

**重载形式的选择：**

| 运算符 | 重载形式 | 原因 |
|---|---|---|
| `=`、`()`、`[]`、`->` | 必须成员函数 | 语言规定，本质是对对象本身操作 |
| `+=`、`*=`、`/=` 等复合赋值 | 建议成员函数 | 需要修改第一个操作数（即 `*this`） |
| `++`、`--` | 建议成员函数 | 前置返回引用、后置返回临时对象，依赖对象自身状态 |
| `+`、`-`、`*`、`/` 等二元运算 | 推荐友元（普通函数） | 左右操作数对称，支持隐式转换 |
| `<<`、`>>` 流运算符 | 必须友元（普通函数） | 第一个操作数是 `ostream`/`istream`，无法放入类内 |
| `==`、`<` 等比较运算 | 推荐友元（普通函数） | 与 `+` 同理，左右对称 |

**前置 `++` 与后置 `++`：**

- 前置 `++`：`MyClass &operator++()`，返回 `*this` 的引用，`++(++a)` 连续使用合法。
- 后置 `++`：`MyClass operator++(int)`，带一个无名的 `int` 参数用于区分，返回自增**前**的临时对象（拷贝构造），因此效率略低。

**不能被重载的运算符：** `::`（作用域）、`.`（成员访问）、`.*`（成员指针访问）、`?:`（三目）、`sizeof`、`typeid`。

**操作数限制：** 重载不能改变运算符原本的含义、操作数数量和顺序，且操作数中至少一个必须是自定义类型或枚举类型。

## 友元

### 什么是友元

友元不受 `public、private、protected` 控制，友元破坏了封装性，所以应该尽量合理使用。

### 友元之普通函数

将类外的**普通（全局）函数**在类内声明为友元后，该函数就可以访问类的私有成员。

```c++
#include <iostream>

class MyClass {
public:
  friend void show(const MyClass &rhs);   // 类内声明友元
  MyClass(int x = 0, int y = 0) : _x(x), _y(y) {}

private:
  int _x;
  int _y;
};

// 类外定义，无需再加 friend 关键字
void show(const MyClass &rhs) {
  std::cout << "_x = " << rhs._x
            << ", _y = " << rhs._y << std::endl;   // 直接访问私有成员
}

int main(int argc, char *argv[]) {
  MyClass c(1, 2);
  show(c);
  return 0;
}
```

### 友元之成员函数

把另一个类**的某个成员函数**声明为友元，只有这一个函数能访问私有成员。由于 A 的成员函数参数用到了 B 的引用，需要**前置声明** B，并注意类的定义顺序。

```c++
#include <iostream>

class B;   // 前置声明，A 中才能写 B 的引用参数

class A {
public:
  void print(const B &b);   // 先声明 A 的成员函数
};

class B {
public:
  friend void A::print(const B &b);   // 把 A::print 声明为 B 的友元
private:
  int _value = 10;
};

// 在 B 定义完之后再实现 A::print
void A::print(const B &b) {
  std::cout << b._value << std::endl;   // 可以访问 B 的私有成员
}

int main(int argc, char *argv[]) {
  B b;
  A a;
  a.print(b);   // 10
  return 0;
}
```

### 友元之友元类

把整个类声明为友元，该类的**所有成员函数**都可以访问另一个类的私有成员。

```c++
#include <iostream>

class B {
  friend class A;   // A 类的所有成员函数都是 B 的友元
public:
  B() : _value(10) {}

private:
  int _value;
};

class A {
public:
  void show(const B &b) const {
    std::cout << b._value << std::endl;   // A 的任意成员函数都能访问 B 的私有成员
  }
};

int main(int argc, char *argv[]) {
  B b;
  A a;
  a.show(b);   // 10
  return 0;
}
```

> 三者的区别：友元普通函数是"一个全局函数"，友元成员函数是"另一个类的某一个函数"，友元类是"另一个类的所有函数"，访问权限依次扩大。

### 友元的特性

友元函数需要单独设计为友元，不会因为函数名相同，就设计了一批友元函数。

单向：而且友元是单向的，在 A 类中设计了别的友元后，别的友元可以访问 A 中的私有成员，但是 A 中的函数无法访问这个友元中的值。

无传递性：A -> B -> C，但是 C 不一定是 A 的友元

无继承：友元不能被继承

## 总结

### 运算符重载

- 本质：运算符重载就是编写一个名为 `operator符号` 的特殊函数，让自定义类型拥有与内置类型一致的运算语法。
- 三种形式：**普通函数**（访问私有成员要 `get/set`）、**友元函数**（可直接访问私有成员）、**成员函数**（隐藏 `this`，参数个数减一）。
- 选择口诀：二元运算用友元，复合赋值与自增自减用成员函数，`= / () / [] / ->` 必须成员函数，`<< / >>` 必须友元。
- 前后置 `++`：前置返回 `*this` 引用、效率高；后置多一个 `int` 占位参数、返回旧值临时对象、效率略低。

### 友元

- 声明位置：类内任意位置（`public/private/protected` 皆可，效果相同）；不是类的成员，没有 `this`。
- 三种形式：普通函数（全局函数）、成员函数（另一个类的单个成员）、友元类（另一个类的全部成员）。
- 三条特性：**单向**（A 给 B 授权，B 能访问 A，A 不能访问 B）、**无传递性**（A→B→C 时 C 不是 A 的友元）、**无继承**（友元不能被子类继承）。
- 注意：每个函数/类都要**单独**声明为友元，同名函数不会自动获得权限。
- 代价：友元破坏了封装性，能不用就不用；主要使用场景是运算符重载，以及两个类需要紧密配合（如迭代器访问容器内部）。


## 作业

### 一、选择题
1.关于友元的描述中，( A )是错误的。

A．友元函数是成员函数，它被说明在类体内

B．友元函数可直接访问类中的私有成员

C．友元函数破坏封装性，使用时尽量少用

D．友元类中的所有成员函数都是友元函数

2.下面对于友元函数描述正确的是（ C ）。

A.友元函数的实现必须在类的内部定义

B.友元函数是类的成员

C.友元函数破坏了类的封装性和隐藏性

D.友元函数不能访问类的私有成员

3.下列的各类函数中，（ C ）不是类的成员函数。

A. 构造函数   B. 析构函数  C. 友元函数  D. 拷贝构造函数

4．友元的作用是。（ A ）

A.提高程序的运行效率  B.加强类的封装性

C. 实现数据的隐蔽       D. 增加成员函数的种类

5、如果类A被说明成类B的友元，则（ B D E ）。(多选题)

A、类A的成员即类B的成员 

B、类B的成员即类A的成员

C、类A的成员函数不能访问类B的成员 

D、类A的成员函数可以访问类B的成员

E、类B不一定是类A的友元

### 二、写出下列程序的结果

33 88

```C++
#include <iostream>

using std::endl;
using std::cout;

class B 
{  
   int y;
public:
	  friend class  A; 
};
class A
{ 
      int x;
 public:  
     A(int a,B &r, int b)  
	 {
		x=a; 
		r.y=b;
	 } 
     void Display( B & ); 
};
void A::Display(B &r)
{
    cout<<x<<" "<<r.y<<endl;
}

int main( )
{   B Obj2;
    A Obj1(33,Obj2,88);
    Obj1.Display(Obj2);
	

	return 0;

} 
```


### 三、简答题

1、什么是友元？友元的存在形式有？友元有何特点？

用`friend`关键字修饰的，类，函数。这就有友元的成员函数，友元的普通函数，友元类三种存在形式。
友元无视“public, protected, private” 规则，破坏了封装型，因此，我们要注意合理使用，不要过度。

2、运算符重载的原则是什么？有哪些规则？

    - 为了防止用户对标准类型进行运算符重载，C++规定重载的运算符的操作对象必须至少有一个是自
    - 定义类型或枚举类型
    - 重载运算符之后，其优先级和结合性还是固定不变的。
    - 重载不会改变运算符的用法，原来有几个操作数、操作数在左边还是在右边，这些都不会改变。
    - 重载运算符函数不能有默认参数，否则就改变了运算符操作数的个数。
    - 重载逻辑运算符（&&,||）后，不再具备短路求值特性。
    - 不能臆造一个并不存在的运算符，如@、$等

3、不能重载的运算符有哪几个？

四个，都带点，分别是

    - 成员访问控制符 `.`
    - 作用域限定符 `::`
    - 成员指针访问运算符 `.*`
    - 三目运算符 `?:`
    - 长度运算符 `sizeof`
    - (AI补充的一个)`typeid`

4、运算符重载的形式有哪几种？

    - 全局普通函数重载
    - 全局友元函数重载
    - 类内成员函数重载

5、自增运算符的前置形式和后置形式有什么区别?返回值类型分别是什么？

    - ++a 先自增后引用，表达式的值和 a 的值保持一致
    - a++ 先引用后自增，表达式先引用 a 自增之前的值，a 再自增

### 四、编程题
1、问题描述，编写Base类使下列代码输出为1

```C++
int i=2;int j=7;
Base x(i);
Base y(j);
cout << (x+y == j - i) << endl;
```

提示：本题考查的其实就是运算符重载的知识点。



2、实现String类的其它运算符的重载

```C++
class String 
{
public:
	String();
	String(const char *);
	String(const String &);
	~String();
	String &operator=(const String &);
	String &operator=(const char *);

	String &operator+=(const String &);
	String &operator+=(const char *);
	
	char &operator[](std::size_t index);
	const char &operator[](std::size_t index) const;
	
	std::size_t size() const;
	const char* c_str() const;
	
	friend bool operator==(const String &, const String &);
	friend bool operator!=(const String &, const String &);
	
	friend bool operator<(const String &, const String &);
	friend bool operator>(const String &, const String &);
	friend bool operator<=(const String &, const String &);
	friend bool operator>=(const String &, const String &);
	
	friend std::ostream &operator<<(std::ostream &os, const String &s);
	friend std::istream &operator>>(std::istream &is, String &s);

private:
	char * _pstr;
};

String operator+(const String &, const String &);
String operator+(const String &, const char *);
String operator+(const char *, const String &);
```

提示：将上面自定义String的所有函数重新实现一下，注意有些函数是可以相互调用的，这个代码不难，但是相对来说比较繁琐，可以写一个测试一个，降低错误率。

