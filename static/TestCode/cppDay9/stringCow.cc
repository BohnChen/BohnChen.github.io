#include <cstring>
#include <iostream>
#include <ostream>

using std::cout;
using std::endl;

class String {
public:
  // 空参构造函数
  String() : _pstr(new char[5]() + 4) {
    cout << "this is String() ." << endl;
    *(int *)(_pstr - 4) = 1;
  }

  // c 风格字符串参数的构造函数
  String(const char *str) : _pstr(new char[strlen(str) + 5]() + 4) {
    cout << "this is String(const char * str)." << endl;
    strcpy(_pstr, str);
    *(int *)(_pstr - 4) = 1;
  }

  // 拷贝构造函数
  String(const String &rhs) : _pstr(rhs._pstr) { ++*(int *)(_pstr - 4); }

  // 赋值运算符函数
  String &operator=(const String &rhs) {
    cout << "Now, we are in operator=()." << endl;
    if (this != &rhs) {

      --*(int *)(_pstr - 4);
      if (0 == *(int *)(_pstr - 4)) {
        delete[] (_pstr - 4);
      }
      _pstr = rhs._pstr;
      ++*(int *)(_pstr - 4);
    }
    return *this;
  }

  void printRefcnt() {
    cout << "the ref count is " << *(int *)(_pstr - 4) << endl;
  }

  ~String() {
    decreaseRefCount();
    if (*(int *)(_pstr - 4) == 0) {
      delete[] (_pstr - 4);
    }
  }

  char &operator[](size_t idx) {

    if (idx < size()) {
      if (getRefCount() > 1) {
        char *ptmp = new char[strlen(_pstr) + 5]() + 4;
        strcpy(ptmp, _pstr);
        decreaseRefCount();
        _pstr = ptmp;
        initRefCount();
      }
      return _pstr[idx];
    } else {
      static char nullchar = '\0';
      return nullchar;
    }
  }

  const char &operator[](size_t idx) const {
    if (idx < size()) {
      return _pstr[idx];
    } else {
      static char nullchar = '\0';
      return nullchar;
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const String &rhs);

  const size_t size() const { return strlen(_pstr); }
  int getRefCount() const { return *(int *)(_pstr - 4); }
  void initRefCount() { *(int *)(_pstr - 4) = 1; }
  void increaseRefCount() { ++*(int *)(_pstr - 4); }
  void decreaseRefCount() { --*(int *)(_pstr - 4); }
  const char *c_str() const { return _pstr; }

private:
  char *_pstr;
};

std::ostream &operator<<(std::ostream &os, const String &rhs) {
  if (rhs._pstr) {
    os << rhs._pstr << endl;
  }
  return os;
}
void test() {
  String s1("hello");
  cout << "s1 = " << s1 << endl;
  printf("s1's address: %p\n", s1.c_str());
  printf("s1's RefCount = %d\n", s1.getRefCount());

  cout << endl;
  String s2 = s1;
  cout << "s1 = " << s1 << endl;
  cout << "s2 = " << s2 << endl;
  printf("s1's address: %p\n", s1.c_str());
  printf("s2's address: %p\n", s2.c_str());
  printf("s1's RefCount = %d\n", s1.getRefCount());
  printf("s2's RefCount = %d\n", s2.getRefCount());

  cout << endl;
  String s3("world");
  cout << "s3 = " << s3 << endl;
  printf("s3's address: %p\n", s3.c_str());
  printf("s3's RefCount = %d\n", s3.getRefCount());

  cout << endl << "执行s3 = s1操作" << endl;
  s3 = s1;
  cout << "s1 = " << s1 << endl;
  cout << "s2 = " << s2 << endl;
  cout << "s3 = " << s3 << endl;
  printf("s1's address: %p\n", s1.c_str());
  printf("s2's address: %p\n", s2.c_str());
  printf("s3's address: %p\n", s3.c_str());
  printf("s1's RefCount = %d\n", s1.getRefCount());
  printf("s2's RefCount = %d\n", s2.getRefCount());
  printf("s3's RefCount = %d\n", s3.getRefCount());

  cout << endl << "对s3[0] = \'H\'" << endl;
  // s3[0] = 'H';
  s3[0];
  cout << "s1 = " << s1 << endl;
  cout << "s2 = " << s2 << endl;
  cout << "s3 = " << s3 << endl;
  printf("s1's address: %p\n", s1.c_str());
  printf("s2's address: %p\n", s2.c_str());
  printf("s3's address: %p\n", s3.c_str());
  printf("s1's RefCount = %d\n", s1.getRefCount());
  printf("s2's RefCount = %d\n", s2.getRefCount());
  printf("s3's RefCount = %d\n", s3.getRefCount());
}

int main(int argc, char *argv[]) {
  test();

  return 0;
}
