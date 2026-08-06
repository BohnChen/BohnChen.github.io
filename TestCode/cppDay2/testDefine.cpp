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
	std::cout << "hi" << std::endl;
	return 0;
}
