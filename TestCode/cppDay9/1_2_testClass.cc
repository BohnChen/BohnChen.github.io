class Widget {
public:
  int x = 1;
};

Widget g; // ① 全局对象：声明在全局作用域，程序结束才销毁

int main() {
  Widget w1;   // ② 块对象：名字只在 main 的块作用域内可见
  {            //    —— 内层块开始
    Widget w2; // ③ 内层块对象：只在内层块内可见
  } //    w2 在此销毁（离开块作用域）
  // w2 在这里已不存在，访问会报错
  return 0; // w1 在此销毁，g 在 main 之后销毁
}
