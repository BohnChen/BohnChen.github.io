#include <iostream>

class HeapObj {
public:
  HeapObj(int a = 0) : _a(a) {}
  void operator delete(void *pret) {
    std::cout << "void operator delete(void *pret)" << std::endl;
    free(pret);
  }

  void destroy() {
    std::cout << "destroy()" << std::endl;
    delete this; // 删掉对象本身
  }

  void *operator new(size_t sz) {
    std::cout << "void *operator new(size_t sz)" << std::endl;
    void *ptr = malloc(sz);
    return ptr;
  }

private:
  ~HeapObj() {}
  int _a;
};

int main(int argc, char *argv[]) {
  std::cout << "hello world!" << std::endl;
  // HeapObj hpstack(10);
  HeapObj *hp = new HeapObj(10);

  return 0;
}
