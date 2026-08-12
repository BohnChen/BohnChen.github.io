#include <iostream>

using std::cout;
using std::endl;

struct Foo
{
	Foo()
	{
	}
	

	Foo(int)
	{
	}
	
	void fun()
	{
	}

};

int main(void)
{
/***
 *
 *因为 C++ 语法规定：任何能被解析为函数声明的语句，就必须被解析为函数声明。 和你是不是 struct 无关。
Foo b(); 从语法形式上完全匹配 "返回值类型 函数名(参数列表)" 这一模式。编译器在语法分析阶段只看形式，不看上下文——Foo 是 struct 没关系，因为函数完全可以返回 struct 类型（比如 std::string getName()）。
这是继承自 C 的语法规则：int f(); 在 C 里就是函数声明，C++ 无法改变这一点，否则会破坏兼容性。所以即使你本意是构造一个对象，编译器也只能把它当成函数声明。
这就是 Most Vexing Parse 的由来。
 *
 *
 *
 * **/
	Foo a(10);//语句1
	a.fun();//语句2
	// b 被解析为了函数声明
	Foo b();//语句3
	
	// 所以这里报错了
	b.fun();//语句4 
	return 0;
}
