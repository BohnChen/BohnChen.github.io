/*
       > 元素入队             void push(int);
       > 元素出队             void pop();
       > 读取队头元素         int front();
       > 读取队尾元素         int back();
       > 判断队列是否为空     bool emty();
			 > 判断队列是否已满     bool full();
 */

#include <iostream>


class MyQue {
	int* _myque;
	int _length;
	int _head;
	int _tail;
public:
	MyQue(); 
	~MyQue();
	
	void push(int x);
	void pop();
	int front();
	int back();
	bool empty();
	bool full();
};

MyQue::MyQue()
	:_myque(new int[10]())
	,_length(0)
	,_head(0)
	,_tail(-1) {}

MyQue::~MyQue() {
	delete [] _myque;
}

// 必须循环起来防止踩内存
void MyQue::push(int x) {
	if(_length == 10) return;
	_tail = (_tail + 1) % 10;
	_myque[_tail] = x;
	++_length;
}

// 必须循环起来防止踩内存
void MyQue::pop() {
	if(_length == 0) return;
	_head = (_head + 1) % 10;
	--_length;
}


int MyQue::front() {
	if(_length > 0) {
		return _myque[_head];
	}else{
		std::cout << "This is a empty Queue." << std::endl;
		return -1;
	}
}
int MyQue::back() {
	if(_length > 0) {
		return _myque[_tail];
	}else{
		std::cout << "This is an empty Queue." << std::endl;
		return -1;
	}
}

bool MyQue::empty() {
	return _length == 0;
}

bool MyQue::full() {
	return _length == 10;
}

int main(int argc, char *argv[]) {
	MyQue myque;
	myque.push(1);
	myque.push(2);
	myque.push(3);
	std::cout << "the head is " << myque.front() << std::endl;
	std::cout << "the back is " << myque.back() << std::endl;
	myque.push(4);
	myque.push(5);
	myque.push(6);
	myque.push(7);
	myque.push(8);
	myque.push(9);
	myque.push(10);
	if(myque.full()) std::cout << "The queue is full." << std::endl;
	std::cout << "the head is " << myque.front() << std::endl;
	std::cout << "the back is " << myque.back() << std::endl;
	myque.pop();
	std::cout << "the head is " << myque.front() << std::endl;
	std::cout << "the back is " << myque.back() << std::endl;
	myque.pop();
	myque.pop();
	myque.pop();
	myque.pop();
	myque.pop();
	myque.pop();
	myque.pop();
	myque.pop();
	std::cout << "the head is " << myque.front() << std::endl;
	std::cout << "the back is " << myque.back() << std::endl;
	myque.pop();
	std::cout << "the head is " << myque.front() << std::endl;
	std::cout << "the back is " << myque.back() << std::endl;
	myque.push(11);
	myque.push(12);
	std::cout << "the head is " << myque.front() << std::endl;
	std::cout << "the back is " << myque.back() << std::endl;
	myque.push(13);
	myque.push(14);
	myque.push(15);
	myque.push(16);
	myque.pop();
	myque.pop();
	myque.pop();
	myque.pop();
	myque.pop();
	std::cout << "the head is " << myque.front() << std::endl;
	std::cout << "the back is " << myque.back() << std::endl;
	myque.pop();
	if(myque.empty()) std::cout << "The queue is empty." << std::endl;

	return 0;
}
