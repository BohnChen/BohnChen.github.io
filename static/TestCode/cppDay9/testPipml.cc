#include "Pimpl.h"

int main(int argc, char *argv[]) {
  Widget w1;
  w1.print();
  Widget w2 = w1;
  w2.print();

  return 0;
}
