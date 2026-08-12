#include <string>
#include <iostream>

using std::string;

class Point {
	public:
    Point(string word, int intPara)
    :_word(word)
    ,_myInt(intPara){
        
    }
    Point(const Point &rhs) {
			_word = rhs._word;
			_myInt = rhs._myInt;
    }

		void getMyInt() {
			std::cout << "The int is " << _myInt << std::endl;
		}

		void getMyWord() {
			std::cout << "The int is " << _word << std::endl;
		}
private:
    string _word;
    int _myInt;
};

int main(int argc, char *argv[]) {
	std::cout << "This file is for testing constructor." << std::endl;
	Point p1("nice", 10);

	Point p2 = p1; // 当调用拷贝构造函数时，rhs = p1，p1已存在，但是rhs是一个新对象，所以重新调用拷贝构造函数，陷入循环，并且没有出口，程序知道栈溢出崩溃。
	p2.getMyInt();
	p2.getMyWord();
	return 0;
}
