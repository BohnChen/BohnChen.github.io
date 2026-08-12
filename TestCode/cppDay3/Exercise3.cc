/*
 *
		   > 元素入栈     void push(int);
		   > 元素出栈     void pop();
		   > 读出栈顶元素 int top();
		   > 判断栈空     bool emty();
		   > 判断栈满     bool full();
	 如果栈溢出，程序终止。栈的数据成员由存放10个整型数据的数组构成。（可以自己设计入栈出栈的数据）
 *
 *
 * */
#include <iostream>

using std::cout;
using std::endl;


class MyVector {
	int *_myvec;
	int _length;
	int _point;
	bool _isfull;
	bool _isempty;
public:
	void push(int x);
	void pop();
	int top();
	bool isEmpty();
	bool isFull();
	MyVector();
	~MyVector(); 
};

MyVector::MyVector()
:_myvec(new int[10]{})
,_length(0)
,_point(-1)
,_isfull(false)
,_isempty(true){
}

MyVector::~MyVector() {
	delete[] _myvec;
}

void MyVector::push(int x) {
	if(_length < 10) {
		_myvec[++_point] = x;
		++_length;
	}
	if(_length == 10)
		std::cout << "The Vec is full." << std::endl;
		_isfull = true;
}

void MyVector::pop() {
	if(_length > 0) {
		--_point;
		--_length;
	}else{
		std::cout << "the vec is empty." << std::endl;
		_isempty = true;
	}
}

int MyVector::top() {
	if(_length > 0) {
		return _myvec[_point];
	}
	else {
		std::cerr << endl << "the vec is empty." << std::endl;
		return -1;
	}
}

bool MyVector::isEmpty() {
	 return _isempty;
}
bool MyVector::isFull() {
	return _isfull;
}

int main(int argc, char *argv[]) {
	MyVector vec1;
	vec1.push(1);
	vec1.push(2);
	vec1.push(3);
	vec1.push(4);
	vec1.push(5);
	vec1.push(6);
	vec1.push(7);
	vec1.push(8);
	vec1.push(9);
	vec1.push(10);
	vec1.push(4);
	vec1.push(4);
	vec1.push(4);
	std::cout << "the top of vec is " << vec1.top() << std::endl;
	vec1.pop();
	std::cout << "the top of vec is " << vec1.top() << std::endl;
	vec1.pop();
	std::cout << "the top of vec is " << vec1.top() << std::endl;
	return 0;
}
