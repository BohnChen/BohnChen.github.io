#include <iostream>
using std::cout;
using std::endl;

class A {
 public:
	 A() {
		 std::cout << "A()" << std::endl;
	 }
	 ~A() {
		 std::cout << "~A()" << std::endl;
	 }
};


class B {
	public:
		B() {
			std::cout << "B()" << std::endl;
		}
		~B() {
			std::cout << "~B()" << std::endl;
		}
};
class C {
 public:
	 C() {
		 std::cout << "C()" << std::endl;
	 }
	 ~C() {
		 std::cout << "~C()" << std::endl;
	 }
};
class D {
 public:
	 D() {
		 std::cout << "D()" << std::endl;
	 }
	 ~D() {
		 std::cout << "~D()" << std::endl;
	 }
};

C c;

int main()
{
// 输出如下：
// C()
// A()
// B()
// D()
// ~A()
// ~B()
// ~D()
// ~C()

    A *pa=new A();
    B b;
    static D d;
    delete pa;
		return 0;
}

