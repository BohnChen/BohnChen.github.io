#include <iostream>

using std::cin;
using std::cout;
using std::endl;

class MySingleton {
public:
  void print() { cout << "This print()" << endl; }

  static MySingleton &getInstance() {
    if (nullptr == _pinstance) {
      _pinstance = new MySingleton();
    }
    return *_pinstance;
  }

  MySingleton(const MySingleton &c) = delete;
  MySingleton &operator=(const MySingleton &c) = delete;

  static void destroy() {
    if (nullptr != _pinstance) {
      delete _pinstance;
      _pinstance = nullptr;
    }
  }

private:
  MySingleton() { cout << "this is MySingleton()." << endl; }
  ~MySingleton() { cout << "this is ~MySingleton()." << endl; }
  static MySingleton *_pinstance;
};

MySingleton *MySingleton::_pinstance = nullptr;

int main(int argc, char *argv[]) {
  MySingleton &ms1 = MySingleton::getInstance();
  MySingleton &ms2 = MySingleton::getInstance();
  MySingleton &ms3 = MySingleton::getInstance();

  ms1.destroy();
  ms2.destroy();
  ms3.destroy();

  return 0;
}
