#include <cstring>
#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class String {
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
  const char *c_str() const;

  friend String operator+(const String &a, const String &b);
  friend String operator+(const String &a, const char *b);
  friend String operator+(const char *a, const String &b);

  friend bool operator==(const String &a, const String &b);
  friend bool operator!=(const String &a, const String &b);

  friend bool operator<(const String &a, const String &b);
  friend bool operator>(const String &a, const String &b);
  friend bool operator<=(const String &a, const String &b);
  friend bool operator>=(const String &a, const String &b);

  friend std::ostream &operator<<(std::ostream &os, const String &s);
  friend std::istream &operator>>(std::istream &is, String &s);

private:
  char *_pstr;
};

// [批注] 默认构造固定分配 10 字节，与其它构造"恰好 strlen+1"的容量不一致，
//        是本类多处溢出问题的根源。更稳妥：默认按空串分配 new char[1]()。
// [原实现（已注释）]
/*  String::String() : _pstr(new char[10]()) {}
 */
// [修复] 空串也只需 1 字节（存放 '\0'）。
String::String() : _pstr(new char[1]()) {}

// [批注] 按源串长度精确分配，写法正确。
String::String(const char *a) : _pstr(new char[strlen(a) + 1]()) {
  strcpy(_pstr, a);
}

// [批注] 拷贝构造按源串长度精确分配 + strcpy，正确。
String::String(const String &a) : _pstr(new char[strlen(a._pstr) + 1]) {
  strcpy(_pstr, a._pstr);
}

String::~String() {
  if (_pstr) {
    delete[] _pstr;
    _pstr = nullptr;
  }
}

// [批注] ⚠ 原实现两处问题（ASAN 已复现）：①源串比目标长 → strcpy 堆溢出；
//  ②自赋值 a = a → 内存重叠，未定义行为。
// [原实现（已注释）]
/*  String &String::operator=(const String &a) {
  strcpy(this->_pstr, a._pstr);
  return *this;
}
*/
// [修复] 深拷贝：先判断自赋值，再释放旧内存、按新长度重新分配。
String &String::operator=(const String &a) {
  if (this != &a) {
    delete[] _pstr;
    _pstr = new char[strlen(a._pstr) + 1]();
    strcpy(_pstr, a._pstr);
  }
  return *this;
}
// [批注] ⚠ 原实现 strcpy 无边界检查，目标缓冲区装不下时溢出。
// [原实现（已注释）]
/*  String &String::operator=(const char *a) {
  strcpy(this->_pstr, a);
  return *this;
}
*/
// [修复] 按源串长度重新分配。
String &String::operator=(const char *a) {
  delete[] _pstr;
  _pstr = new char[strlen(a) + 1]();
  strcpy(_pstr, a);
  return *this;
}

// [批注] 借助 operator+ 生成临时对象再赋值，逻辑正确（依赖上面的深拷贝
// operator=）。
String &String::operator+=(const String &a) {
  *this = *this + a;
  return *this;
}

String &String::operator+=(const char *a) {
  *this = *this + a;
  return *this;
}

// [批注] 无边界检查，行为与 std::string 一致（越界属未定义行为），可接受。
char &String::operator[](std::size_t index) { return this->_pstr[index]; }
const char &String::operator[](std::size_t index) const {
  return this->_pstr[index];
}

// [批注] 每次调用 strlen 都是 O(n)，可接受；频繁使用可缓存长度。
std::size_t String::size() const { return strlen(this->_pstr); }

const char *String::c_str() const { return this->_pstr; }

// [批注] ⚠ 原实现只分配 strlen(a)+1 字节，却用 strcat 追加 b，必然越界
//        （ASAN 已复现堆溢出，崩溃点 strcat）。
// [原实现（已注释）]
/*  String operator+(const String &a, const String &b) {
  char *pstr = new char[strlen(a._pstr) + 1]();
  strcpy(pstr, a._pstr);
  String str(strcat(pstr, b._pstr));
  if (pstr) {
    delete[] pstr;
    pstr = nullptr;
  }
  return str;
}
*/
// [修复] 分配总长 strlen(a) + strlen(b) + 1，strcpy 后再 strcat 就安全了。
String operator+(const String &a, const String &b) {
  char *pstr = new char[strlen(a._pstr) + strlen(b._pstr) + 1]();
  strcpy(pstr, a._pstr);
  strcat(pstr, b._pstr);
  String str(pstr);
  delete[] pstr;
  return str;
}

// [批注] ⚠ 原实现与上面相同的 strcat 越界问题。
// [原实现（已注释）]
/*  String operator+(const String &a, const char *b) {
  char *pstr = new char[strlen(a._pstr) + 1]();
  strcpy(pstr, a._pstr);
  String str(strcat(pstr, b));
  if (pstr) {
    delete[] pstr;
    pstr = nullptr;
  }
  return str;
}
*/
// [修复] 三个 operator+ 代码几乎重复，这里直接用 (String,String) 版本派生。
String operator+(const String &a, const char *b) { return a + String(b); }
// [批注] ⚠ 原实现与上面相同的 strcat 越界问题。
// [原实现（已注释）]
/*  String operator+(const char *a, const String &b) {
  char *pstr = new char[strlen(a) + 1]();
  strcpy(pstr, a);
  String str(strcat(pstr, b._pstr));
  if (pstr) {
    delete[] pstr;
    pstr = nullptr;
  }
  return str;
}
*/
// [修复] 同样用临时 String 派生，一行搞定。
String operator+(const char *a, const String &b) { return String(a) + b; }

// [批注] strcmp 相等返回 0，!0 → true，正确。
bool operator==(const String &a, const String &b) {
  return !strcmp(a._pstr, b._pstr);
}

// [批注] 原实现直接 return strcmp(...)：int 隐式转 bool，功能对但可读性差。
// [原实现（已注释）]
/*  bool operator!=(const String &a, const String &b) {
  return strcmp(a._pstr, b._pstr);
}
*/
// [修复] 更清晰的写法。
bool operator!=(const String &a, const String &b) {
  return strcmp(a._pstr, b._pstr) != 0;
}

// [批注] 原实现 if-else 冗余，可简化为一行。
// [原实现（已注释）]
/*  bool operator<(const String &a, const String &b) {
  if (strcmp(a._pstr, b._pstr) < 0) {
    return true;
  } else {
    return false;
  }
}
*/
// [修复] 直接返回比较结果。
bool operator<(const String &a, const String &b) {
  return strcmp(a._pstr, b._pstr) < 0;
}

// [批注] 原实现 if-else 冗余，可简化为一行。
// [原实现（已注释）]
/*  bool operator>(const String &a, const String &b) {
  if (strcmp(a._pstr, b._pstr) > 0) {
    return true;
  } else {
    return false;
  }
}
*/
// [修复] 直接返回比较结果。
bool operator>(const String &a, const String &b) {
  return strcmp(a._pstr, b._pstr) > 0;
}

// [批注] 用 < 与 == 组合实现，正确。
bool operator<=(const String &a, const String &b) {
  if (a < b || a == b)
    return true;
  else
    return false;
}
bool operator>=(const String &a, const String &b) {
  if (a > b || a == b)
    return true;
  else
    return false;
}
// [批注] 原实现内部输出 endl，调用方再写 endl 会双换行；流运算符不该自带换行。
// [原实现（已注释）]
/*  std::ostream &operator<<(std::ostream &os, const String &s) {
  os << s._pstr << endl;
  return os;
}
*/
// [修复] 只输出内容，是否换行交给调用方控制。
std::ostream &operator<<(std::ostream &os, const String &s) {
  os << s._pstr;
  return os;
}

// [批注] ⚠ 原实现无边界读取：is >> s._pstr 直接写原始缓冲区，超长即溢出。
// [原实现（已注释）]
/*  std::istream &operator>>(std::istream &is, String &s) {
  is >> s._pstr;
  return is;
}
*/
// [修复] 先读入临时缓冲，再整体赋值（走修复后的深拷贝 operator=）。
std::istream &operator>>(std::istream &is, String &s) {
  char buf[1024];
  is >> buf;
  s = buf;
  return is;
}

int main(int argc, char *argv[]) {
  // std::cout << "hello world!" << std::endl;
  String a("hi");
  String b("today.");
  String c = a + b;  // [批注] 修复后 operator+ 按总长分配，不再越界。
  cin >> a;          // [批注] 修复后 operator>> 读入临时缓冲，不再溢出。
  cout << c << endl; // [批注] operator<< 已去掉内部 endl，这里一个 endl 正好。

  return 0;
}
