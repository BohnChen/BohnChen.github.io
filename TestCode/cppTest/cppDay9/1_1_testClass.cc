// 未见类的声明，因此报错
Widget w();

// 类名 Widget 声明在全局作用域
class Widget {
public:
  Widget() {}
  ~Widget() {}

  // 类体内直接写 Widget
  Widget *self = nullptr;
};

// 类已经声明，所以可用
Widget w();

int main(int argc, char *argv[]) { return 0; }
