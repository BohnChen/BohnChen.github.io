#include <iostream>
inline int TestInline_2(int n) {
	if(n == 1) {
		return 0;
	}
	if (n == 2) {
		return 1;
	}
	return TestInline_2(n - 1) + TestInline_2(n - 2);
}

int main(int argc, char *argv[]) {
	
	return 0;
}
