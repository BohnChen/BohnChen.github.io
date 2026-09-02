#include <iostream>

class HeapObj {
public:
  HeapObj(int a = 0) : _a(a) {}

  void *operator new(size_t sz) {
    std::cout << "void *operator new(size_t sz)" << std::endl;
    void *ptr = malloc(sz);
    return ptr;
  }

  void destroy() {
    std::cout << "destroy()" << std::endl;
    delete this; // 删掉对象本身
  }

  ~HeapObj() {}

private:
  void operator delete(void *pret) {
    std::cout << "void operator delete(void *pret)" << std::endl;
    free(pret);
  }

  int _a;
};

int main(int argc, char *argv[]) {

  HeapObj hpstack(10);
  // HeapObj *hp = new HeapObj(10);// ERROR
  // hp->destroy();

  return 0;
}
