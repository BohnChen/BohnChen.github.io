#include <iostream>

using std::cin;
using std::cout;
using std::endl;

namespace test2 {
void f2(int &x, int &y) {
  int z = x;
  x = y;
  y = z;
}

void f3(int *x, int *y) {
  int z = *x;
  *x = *y;
  *y = z;
}
} // end of namespace test2

int foo(int x, int y) {
  if (x <= 0 || y <= 0)
    return 1;
  return 3 * foo(x - 1, y / 2);
}

int main() {
#if 0
	int x, y;
	x = 10; y = 26;
	cout << "x, y = " << x << ", " << y << endl;
	test2::f2(x, y);
	cout << "x, y = " << x << ", " << y << endl;
	test2::f3(&x, &y);
	cout << "x, y = " << x << ", " << y << endl;
	x++; 
	y--;
	test2::f2(y, x);
	cout << "x, y = " << x << ", " << y << endl;
#endif

#if 0
	cout << foo(3,5) << endl;
#endif

#if 0
	// 输出为 6
	int x;
	cin >> x;
	if(x++ > 5)
	{
		cout << x << endl;
	}      
	else
	{
		cout << x-- << endl;
	}
#endif
#if 0
	int a[5]={1,2,3,4,5}; 
	int *ptr=(int *)(&a+1); 
	printf("%d,%d",*(a+1),*(ptr-1));
#endif
  for (int i = 0; i < 20; i++)
    cout << "hello" << endl;

  return 0;
}
